#include <iostream>
#include <thread>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include "proxy.h"
#include "blocklist.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include "metrics.h"
#include <atomic>
#include<signal.h>
#include "logger.h"
using namespace std;

atomic<bool> running(true);
int server_fd = -1;  

void handle_signal(int sig) {
    
        (void)sig;
    running = false;

    if (server_fd != -1) {
        close(server_fd);   
        server_fd = -1;
    }

}


int main() {
    load_blocked_domains("blocked_domains.txt");
    
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        cerr << "[Error] Failed to create socket\n";
        return 1;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
 
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8000);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        cerr << "[Error] Failed to bind to port 8000\n";
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, 10) < 0) {
        cerr << "[Error] Failed to listen on socket\n";
        close(server_fd);
        return 1;
    }

    cout << "[Proxy] Listening on port 8000...\n";
    metrics_init();
    signal(SIGINT, handle_signal);    
    signal(SIGTERM, handle_signal);  

    while (running) {
    sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);

    int client_fd = accept(server_fd,
                            (sockaddr*)&client_addr,
                            &len);

    if (client_fd < 0) {
        if (!running){
            break;
           }   // shutdown initiated
        continue;
    }

    std::string client_ip = inet_ntoa(client_addr.sin_addr);

    std::thread(handle_client, client_fd, client_ip).detach();
}

     log_shutdown();       
    metrics_print_summary(); 
    
    return 0;
}
