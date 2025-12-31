#include "lru_cache.h"
#include "proxy.h"
#include "http_parser.h"
#include "https_tunnel.h"
#include "net_utils.h"
#include "logger.h"
#include "blocklist.h"
#include "metrics.h"
#include <unistd.h>
#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>

using namespace std;

static LRUCache cache(100);



static bool recv_until_headers_end(int fd, std::string& data) {
    char buffer[4096];
    while (data.find("\r\n\r\n") == std::string::npos) {
        ssize_t n = recv(fd, buffer, sizeof(buffer), 0);
        if (n <= 0)
            return false;
        data.append(buffer, n);
    }
    return true;
}

static bool recv_exact(int fd, std::string& data, int bytes) {
    char buffer[4096];
    int received = 0;

    while (received < bytes) {
        int to_read = min((int)sizeof(buffer), bytes - received);
        ssize_t n = recv(fd, buffer, to_read, 0);
        if (n <= 0)
            return false;

        data.append(buffer, n);
        received += n;
    }
    return true;
}

static string make_cache_key(const HttpRequest& req) {
    return req.method + "|" +
           req.host + "|" +
           to_string(req.port) + "|" +
           req.path;
}



void handle_client(int client_fd, string client_ip) {

    string request;
    if (!recv_until_headers_end(client_fd, request)) {
        close(client_fd);
        return;
    }

    // ---- CONNECT ----
    if (request.find("CONNECT") == 0) {
        size_t space = request.find(' ');
        size_t colon = request.find(':', space + 1);
        size_t space2 = request.find(' ', colon + 1);

        string host = request.substr(space + 1, colon - space - 1);
        int port = stoi(request.substr(colon + 1, space2 - colon - 1));

        if (is_blocked_domain(host)) {
            metrics_record_request(host);
            const char* resp = "HTTP/1.1 403 Forbidden\r\n\r\n";
            send(client_fd, resp, strlen(resp), 0);
            close(client_fd);
            log_access(client_ip, host, "CONNECT", 403, 0);
            return;
        }

        metrics_record_request(host);
        handle_https_tunnel(client_fd, host, port);
        log_access(client_ip, host, "CONNECT",200,0);
        return;
    }

    // ---- HTTP ----
    HttpRequest req;
    if (!parse_and_normalize_http(request, req)) {
        close(client_fd);
        return;
    }

    if (is_blocked_domain(req.host)) {
        metrics_record_request(req.host);
        const char* resp =
            "HTTP/1.1 403 Forbidden\r\n"
            "Content-Length: 0\r\n\r\n";
        send(client_fd, resp, strlen(resp), 0);
        close(client_fd);
        log_access(client_ip, req.host, req.path, 403, 0);
        return;
    }
    metrics_record_request(req.host);

    // ---- Read body if present ----
    size_t header_end = request.find("\r\n\r\n");
    int already_read = request.size() - (header_end + 4);
    int remaining = req.content_length - already_read;

    if (remaining > 0) {
        if (!recv_exact(client_fd, request, remaining)) {
            close(client_fd);
            return;
        }
    }

    

    string cache_key = make_cache_key(req);
    string cached_response;

    if (req.method == "GET" && cache.get(cache_key, cached_response)) {
        send(client_fd, cached_response.c_str(), cached_response.size(), 0);
        close(client_fd);
        log_access(client_ip,
           req.host,
           req.path,
           200,
           cached_response.size());
        return;
    }

    int server_fd = connect_to_host(req.host, req.port);
    if (server_fd < 0) {
        close(client_fd);
        return;
    }

    send(server_fd, request.c_str(), request.size(), 0);

    char buffer[4096];
    ssize_t n;
    string full_response;
    size_t bytes_sent = 0;
    int status_code = 0;

   while ((n = recv(server_fd, buffer, sizeof(buffer), 0)) > 0) {
    full_response.append(buffer, n);

    if (status_code == 0) {
        size_t line_end = full_response.find("\r\n");
        if (line_end != string::npos) {
            string status_line = full_response.substr(0, line_end);
            size_t sp = status_line.find(' ');
            if (sp != string::npos)
                status_code = stoi(status_line.substr(sp + 1, 3));
        }
    }

    send(client_fd, buffer, n, 0);
    bytes_sent += n;
}

    
    

    if (req.method == "GET" && status_code == 200) {
        cache.put(cache_key, full_response);
    }


log_access(client_ip,
           req.host,
           req.path,
           status_code,
           bytes_sent);

    close(server_fd);
    close(client_fd);
}
