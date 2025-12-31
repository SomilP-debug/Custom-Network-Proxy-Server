#ifndef LOGGER_H
#define LOGGER_H

#include <string>

void log_access(const std::string& client_ip,
                const std::string& host,
                const std::string& path,
                int status_code,
                size_t bytes_transferred);


void log_shutdown();
#endif
