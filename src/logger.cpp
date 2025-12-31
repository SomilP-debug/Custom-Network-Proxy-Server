#include "logger.h"

#include <fstream>
#include <mutex>
#include <ctime>
#include <sys/stat.h>
#include <cstdio>

static std::mutex log_mutex;

static const std::string LOG_FILE = "proxy.log";
static const std::string ROTATED_LOG = "proxy.log.1";
static const size_t MAX_LOG_SIZE = 5 * 1024 * 1024; // 5 MB


static std::string now() {
    std::time_t t = std::time(nullptr);
    char buf[64];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&t));
    return buf;
}


static bool needs_rotation() {
    struct stat st;
    if (stat(LOG_FILE.c_str(), &st) != 0)
        return false;
    return st.st_size >= (off_t)MAX_LOG_SIZE;
}


static void rotate_log_if_needed() {
    if (!needs_rotation())
        return;

    std::remove(ROTATED_LOG.c_str());           // remove old .1
    std::rename(LOG_FILE.c_str(), ROTATED_LOG.c_str());
}


void log_access(const std::string& client_ip,
                const std::string& host,
                const std::string& path,
                int status_code,
                size_t bytes_transferred) {

    std::lock_guard<std::mutex> lock(log_mutex);

    rotate_log_if_needed();

    std::ofstream log(LOG_FILE, std::ios::app);
    if (!log)
        return;

    log << now()
        << " | " << client_ip
        << " | " << host
        << " | " << path
        << " | " << status_code
        << " | " << bytes_transferred
        << "\n";
}
void log_shutdown() {
    std::lock_guard<std::mutex> lock(log_mutex);
    std::ofstream log(LOG_FILE, std::ios::app);
    if (log)
        log << now() << " | PROXY SHUTDOWN\n";
}

