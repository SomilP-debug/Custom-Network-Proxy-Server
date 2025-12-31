#include "https_tunnel.h"
#include "net_utils.h"
#include <string>
#include <unistd.h>
#include <thread>
#include <iostream>
#include <sys/socket.h>
#include <cstring>
using namespace std;


static void relay(int from_fd, int to_fd) {
    char buffer[4096];
    ssize_t n;

    while ((n = recv(from_fd, buffer, sizeof(buffer), 0)) > 0) {
        send(to_fd, buffer, n, 0);
    }

    shutdown(to_fd, SHUT_WR);
}

void handle_https_tunnel(int client_fd,
                         const string& host,
                         int port) {
    
    int server_fd = connect_to_host(host, port);
    if (server_fd < 0) {
        close(client_fd);
        return;
    }

    
    const char* response =
        "HTTP/1.1 200 Connection Established\r\n\r\n";
    send(client_fd, response, strlen(response), 0);

   

   
    thread t1(relay, client_fd, server_fd); // client → server
    thread t2(relay, server_fd, client_fd); // server → client

   
    t1.join();
    t2.join();

    close(client_fd);
    close(server_fd);
}
