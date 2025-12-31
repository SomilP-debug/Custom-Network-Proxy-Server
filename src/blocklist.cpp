#include "blocklist.h"
#include<iostream>
#include <unordered_set>
#include <fstream>
#include <mutex>

static std::unordered_set<std::string> blocked;
static std::mutex block_mutex;

void load_blocked_domains(const std::string& filename) {
    std::lock_guard<std::mutex> lock(block_mutex);

    blocked.clear();
    std::ifstream file(filename);
    std::string domain;

    while (file >> domain) {
        blocked.insert(domain);
    }
    
}

bool is_blocked_domain(const std::string& host) {
    std::lock_guard<std::mutex> lock(block_mutex);
    return blocked.count(host) > 0;
}
