#include "metrics.h"
#include <unordered_map>
#include <mutex>
#include <chrono>
#include <iostream>
#include <vector>
#include <algorithm>

static std::mutex metrics_mutex;
static std::unordered_map<std::string, int> host_counts;
static long total_requests = 0;
static std::chrono::steady_clock::time_point start_time;

void metrics_init() {
    start_time = std::chrono::steady_clock::now();
}

void metrics_record_request(const std::string& host) {
    std::lock_guard<std::mutex> lock(metrics_mutex);
    total_requests++;
    host_counts[host]++;
}

void metrics_print_summary() {
    std::lock_guard<std::mutex> lock(metrics_mutex);

    auto now = std::chrono::steady_clock::now();
    auto duration_sec =
        std::chrono::duration_cast<std::chrono::seconds>(now - start_time).count();

    double rpm = 0.0;
    if (duration_sec > 0)
        rpm = (total_requests * 60.0) / duration_sec;

    std::vector<std::pair<std::string, int>> hosts(host_counts.begin(),
                                                    host_counts.end());

    std::sort(hosts.begin(), hosts.end(),
              [](auto& a, auto& b) { return a.second > b.second; });

    std::cout << "\n===== Proxy Metrics Summary =====\n";
    std::cout << "Total requests: " << total_requests << "\n";
    std::cout << "Requests per minute: " << rpm << "\n";
    std::cout << "Top requested hosts:\n";

    int limit = 5;
    for (int i = 0; i < (int)hosts.size() && i < limit; ++i) {
        std::cout << "  " << hosts[i].first << " : "
                  << hosts[i].second << "\n";
    }

    std::cout << "================================\n";
}
