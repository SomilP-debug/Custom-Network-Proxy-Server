#ifndef HTTPS_TUNNEL_H
#define HTTPS_TUNNEL_H

#include <string>


void handle_https_tunnel(int client_fd,
                         const std::string& host,
                         int port);

#endif
