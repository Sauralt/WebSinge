#include "../include/HttpParser.hpp"

HttpRequest::HttpRequest() : _body_complete(false), _content_length(0), _chunked(false)
{}

HttpRequest::~HttpRequest()
{}

HttpRequest::HttpRequest(HttpRequest& copy)
{
	*this = copy;
}

HttpRequest& HttpRequest::operator=(HttpRequest& copy)
{
	this->_body = copy._body;
	this->_body_complete = copy._body_complete;
	this->_chunked = copy._chunked;
	this->_content_length = copy._content_length;
	this->_headers = copy._headers;
	this->_http_version = copy._http_version;
	this->_method = copy._method;
	this->_query_string = copy._query_string;
	this->_uri = copy._uri;
	return *this;
}

const std::string &HttpRequest::getMethod() const
{ return _method; }

const std::string &HttpRequest::getUri() const
{ return _uri; }

const std::string &HttpRequest::getQueryString() const
{ return _query_string; }

const std::string &HttpRequest::getHttpVersion() const
{ return _http_version; }

const std::map<std::string, std::string> &HttpRequest:: getHeaders() const
{ return _headers; }

const std::string &HttpRequest::getBody() const
{ return _body; }

bool HttpRequest::isBodyComplete() const
{ return _body_complete; }

size_t HttpRequest::getContentLength() const
{ return _content_length; }

bool HttpRequest::isChunked() const
{ return _chunked; }

void HttpRequest::setMethod(const std::string &m)
{ _method = m; }

void HttpRequest::setUri(const std::string &u)
{ _uri = u; }

void HttpRequest::setQueryString(const std::string &q)
{ _query_string = q; }

void HttpRequest::setHttpVersion(const std::string &v)
{ _http_version = v; }

void HttpRequest::setBody(const std::string &b)
{ _body = b; }

void HttpRequest::setBodyComplete(bool val)
{ _body_complete = val; }

void HttpRequest::setContentLength(size_t len)
{ _content_length = len; }

void HttpRequest::setChunked(bool val)
{ _chunked = val; }

void HttpRequest::addHeader(const std::string &key, const std::string &value)
{ _headers[key] = value; }

bool HttpRequest::hasHeader(const std::string &key) const
{ return _headers.find(key) != _headers.end(); }

std::string HttpRequest::getHeaderValue(const std::string &key) const
{
	std::map<std::string, std::string>::const_iterator it = _headers.find(key);
	if (it != _headers.end())
		return it->second;
	return "";
}

void HttpRequest::clear()
{
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

ParseStatus parseHttpMessage(const std::string buffer, HttpRequest &req)
{
	std::istringstream iss(buffer);
	std::string line;
	if (!std::getline(iss, line))
		return PARSE_INCOMPLETE;
	std::istringstream request_line(line);
	std::string method, uri, version;
	if (!(request_line >> method >> uri >> version))
		return PARSE_ERROR;
	req.setMethod(method);
	req.setUri(uri);
	req.setHttpVersion(version);
	size_t q = uri.find('?');
	if (q != std::string::npos)
	{
		req.setQueryString(uri.substr(q + 1));
		req.setUri(uri.substr(0, q));
	}
	else
		req.setQueryString("");
	std::string key, val;
	while (std::getline(iss, line))
	{
		if (line == "\r" || line.empty())
			break;
		size_t colon = line.find(':');
		if (colon == std::string::npos)
			continue;
		key = line.substr(0, colon);
		val = line.substr(colon + 1);
		while (!val.empty() && (val[0] == ' ' || val[0] == '\t'))
			val.erase(0, 1);
		while (!val.empty() && (val[val.size() - 1] == '\r' || val[val.size() - 1] == ' '))
			val.erase(val.size() - 1);
		req.addHeader(key, val);
		if (key == "Content-Length")
			req.setContentLength(std::atoi(val.c_str()));
		else if (key == "Transfer-Encoding" && val == "chunked")
			req.setChunked(true);
	}
	return PARSE_OK;
}
