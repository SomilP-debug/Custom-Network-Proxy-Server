#include "http_parser.h"

#include <sstream>
#include <algorithm>
#include <cctype>

using namespace std;

// lowercase a string
static string to_lower(string s) {
    transform(s.begin(), s.end(), s.begin(),
              [](unsigned char c) { return std::tolower(c); });
    return s;
}

bool parse_and_normalize_http(const string& raw, HttpRequest& req) {
    istringstream iss(raw);

    // ---- Parse request line ----
    string method, target, version;
    iss >> method >> target >> version;

    if (method.empty() || target.empty())
        return false;

    req.method = method;

    string host;
    string path;
    int port = 80; // default HTTP port
    int content_length = 0;

    // ---- Absolute-form (proxy request) ----
    // GET http://example.com/index.html HTTP/1.1
    if (target.find("http://") == 0) {
        string url = target.substr(7); // remove http://

        size_t slash = url.find('/');
        if (slash == string::npos) {
            host = url;
            path = "/";
        } else {
            host = url.substr(0, slash);
            path = url.substr(slash);
        }
    }
    // ---- Origin-form ----
    // GET /index.html HTTP/1.1
    else {
        path = target;
    }

    // ---- Parse headers ----
    string line;
    while (getline(iss, line)) {
        if (line == "\r" || line.empty())
            break;

        if (line.back() == '\r')
            line.pop_back();

        string lower = to_lower(line);

        if (lower.find("host:") == 0) {
            host = line.substr(5);
        } 
        else if (lower.find("content-length:") == 0) {
            content_length = stoi(line.substr(15));
        }
    }

    if (host.empty())
        return false;

    // ---- Normalize host ----
    host.erase(0, host.find_first_not_of(" \t"));
    host.erase(host.find_last_not_of(" \t\r\n") + 1);
    host = to_lower(host);

    // Remove port if present
    size_t colon = host.find(':');
    if (colon != string::npos) {
        int parsed_port = stoi(host.substr(colon + 1));
        if (parsed_port != 80) {
            port = parsed_port;
        }
        host = host.substr(0, colon);
    }

    // ---- Normalize path ----
    if (path.empty())
        path = "/";

    if (path[0] != '/')
        path = "/" + path;

    if (path.length() > 1 && path.back() == '/')
        path.pop_back();

    // ---- Fill request struct ----
    req.host = host;
    req.path = path;
    req.port = port;
    req.content_length = content_length;

    return true;
}
