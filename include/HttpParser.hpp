#ifndef HTTP_PARSER_HPP
#define HTTP_PARSER_HPP

#include "header.hpp"

enum ParseStatus {
	PARSE_INCOMPLETE = 0,
	PARSE_OK = 1,
	PARSE_ERROR = -1
	};

class HttpRequest
{
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
		HttpRequest();
		~HttpRequest();
		HttpRequest(HttpRequest& copy);
		HttpRequest&	operator=(HttpRequest& copy);
		const std::string &getMethod() const;
		const std::string &getUri() const;
		const std::string &getQueryString() const;
		const std::string &getHttpVersion() const;
		const std::map<std::string, std::string> &getHeaders() const;
		const std::string &getBody() const;
		bool isBodyComplete() const;
		size_t getContentLength() const;
		bool isChunked() const;
		void setMethod(const std::string &m);
		void setUri(const std::string &u);
		void setQueryString(const std::string &q);
		void setHttpVersion(const std::string &v);
		void setBody(const std::string &b);
		void setBodyComplete(bool val);
		void setContentLength(size_t len);
		void setChunked(bool val);
		void addHeader(const std::string &key, const std::string &value);
		bool hasHeader(const std::string &key) const;
		std::string getHeaderValue(const std::string &key) const;
		void clear();
};

	ParseStatus parseHttpMessage(const std::string buffer, HttpRequest &req);

#endif

