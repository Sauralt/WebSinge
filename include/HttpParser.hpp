#ifndef HTTP_PARSER_HPP
#define HTTP_PARSER_HPP

#include <string>
#include <map>

// === DATA STRUCTURE ===

struct HttpRequest {
    std::string method;
    std::string uri;
    std::string query_string;
    std::string http_version;
    std::map<std::string, std::string> headers;
    std::string body;

    bool body_complete;
    size_t content_length;
    bool chunked;

    HttpRequest() : body_complete(false), content_length(0), chunked(false) {}
};

// === ENUM STATUS ===
enum ParseStatus {
    PARSE_INCOMPLETE = 0,
    PARSE_OK = 1,
    PARSE_ERROR = -1
};

// === PARSER API ===
ParseStatus parseHttpMessage(const std::string &buffer, HttpRequest &req);
ParseStatus parseHttpBody(const std::string &buffer, HttpRequest &req);

#endif // HTTP_PARSER_HPP
