#ifndef HTTP_PARSER_HPP
#define HTTP_PARSER_HPP

#include <string>
#include <map>

enum ParseStatus {
    PARSE_INCOMPLETE = 0,
    PARSE_OK = 1,
    PARSE_ERROR = -1
};

class HttpRequest {
    private:
        std::string _method;
        std::string _uri;
        std::string _query_string;
        std::string _http_version;
        std::map<std::string, std::string> _headers;
        std::string _body;

        bool _body_complete;
        size_t _content_length;
        bool _chunked;

    public:
        HttpRequest()
            : _body_complete(false), _content_length(0), _chunked(false) {}
        const std::string &getMethod() const { return _method; }
        const std::string &getUri() const { return _uri; }
        const std::string &getQueryString() const { return _query_string; }
        const std::string &getHttpVersion() const { return _http_version; }
        const std::map<std::string, std::string> &getHeaders() const { return _headers; }
        const std::string &getBody() const { return _body; }
        bool isBodyComplete() const { return _body_complete; }
        size_t getContentLength() const { return _content_length; }
        bool isChunked() const { return _chunked; }
        void setMethod(const std::string &m) { _method = m; }
        void setUri(const std::string &u) { _uri = u; }
        void setQueryString(const std::string &q) { _query_string = q; }
        void setHttpVersion(const std::string &v) { _http_version = v; }
        void setBody(const std::string &b) { _body = b; }
        void setBodyComplete(bool val) { _body_complete = val; }
        void setContentLength(size_t len) { _content_length = len; }
        void setChunked(bool val) { _chunked = val; }
        void addHeader(const std::string &key, const std::string &value) {
            _headers[key] = value;
        }
        bool hasHeader(const std::string &key) const {
            return _headers.find(key) != _headers.end();
        }
        std::string getHeaderValue(const std::string &key) const {
            std::map<std::string, std::string>::const_iterator it = _headers.find(key);
            if (it != _headers.end())
                return it->second;
            return "";
        }
        void clear() {
            _method.clear();
            _uri.clear();
            _query_string.clear();
            _http_version.clear();
            _headers.clear();
            _body.clear();
            _body_complete = false;
            _content_length = 0;
            _chunked = false;
        }
    };

    ParseStatus parseHttpMessage(const std::string &buffer, HttpRequest &req);
    ParseStatus parseHttpBody(const std::string &buffer, HttpRequest &req);

#endif

