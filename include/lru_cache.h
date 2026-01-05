#ifndef LRU_CACHE_H
#define LRU_CACHE_H

#include <unordered_map>
#include <list>
#include <mutex>
#include <string>

class LRUCache {
public:
    explicit LRUCache(size_t capacity)
        : capacity_(capacity) {}

   
    bool get(const std::string& key, std::string& value) {
        std::lock_guard<std::mutex> lock(mutex_);

        auto it = map_.find(key);
        if (it == map_.end())
            return false;

       
        cache_.splice(cache_.begin(), cache_, it->second);
        value = it->second->second;
        return true;
    }

  
    void put(const std::string& key, const std::string& value) {
        std::lock_guard<std::mutex> lock(mutex_);

        auto it = map_.find(key);

       
        if (it != map_.end()) {
            it->second->second = value;
            cache_.splice(cache_.begin(), cache_, it->second);
            return;
        }

       
        if (cache_.size() >= capacity_) {
            auto last = cache_.back();
            map_.erase(last.first);
            cache_.pop_back();
        }

        
        cache_.emplace_front(key, value);
        map_[key] = cache_.begin();
    }

private: 
    size_t capacity_;
    std::list<std::pair<std::string, std::string>> cache_;
    std::unordered_map<
        std::string,
        std::list<std::pair<std::string, std::string>>::iterator
    > map_;

    std::mutex mutex_;
};

#endif
