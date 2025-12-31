#ifndef HTTP_PARSER_H
#define HTTP_PARSER_H

#include <string>

struct HttpRequest {
    std::string method;   
    std::string host;    
    std::string path;     
    int port;             
    int content_length = 0;
};


bool parse_and_normalize_http(const std::string& raw,
                              HttpRequest& req);

#endif
