#include "../include/HttpParser.hpp"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <cstdlib>

// === Utils ===
static std::string toLower(const std::string &s)
{
	std::string r = s;
	for (size_t i = 0; i < r.size(); ++i)
		r[i] = std::tolower(r[i]);
	return r;
}

static std::string trim(const std::string &s)
{
	size_t start = 0;
	while (start < s.size() && std::isspace(s[start]))
		start++;
	if (start >= s.size())
		return "";

	size_t end = s.size() - 1;
	while (end > start && std::isspace(s[end]))
		end--;

	return s.substr(start, end - start + 1);
}

// === Internal header parser ===
static bool parseHeaderLine(const std::string &line, HttpRequest &req)
{
	size_t pos = line.find(':');
	if (pos == std::string::npos)
		return false;

	std::string key = trim(line.substr(0, pos));
	std::string val = trim(line.substr(pos + 1));

	key = toLower(key);
	req.headers[key] = val;

	if (key == "content-length")
		req.content_length = std::atoi(val.c_str());
	if (key == "transfer-encoding" && toLower(val) == "chunked")
		req.chunked = true;

	return true;
}

// === Internal request line parser ===
static bool parseRequestLine(const std::string &line, HttpRequest &req)
{
	std::istringstream iss(line);
	if (!(iss >> req.method >> req.uri >> req.http_version))
		return false;

	size_t q = req.uri.find('?');
	if (q != std::string::npos)
	{
		req.query_string = req.uri.substr(q + 1);
		req.uri = req.uri.substr(0, q);
	}
	else
		req.query_string = "";

	return true;
}

// === parseHttpMessage ===
ParseStatus parseHttpMessage(const std::string &buffer, HttpRequest &req)
{
	size_t pos = buffer.find("\r\n\r\n");
	if (pos == std::string::npos)
		return PARSE_INCOMPLETE;

	std::string header_block = buffer.substr(0, pos);
	std::istringstream iss(header_block);
	std::string line;

	if (!std::getline(iss, line))
		return PARSE_ERROR;
	if (!line.empty() && line[line.size() - 1] == '\r')
		line.erase(line.size() - 1);
	if (!parseRequestLine(line, req))
		return PARSE_ERROR;

	while (std::getline(iss, line))
	{
		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);
		if (line.empty())
			break;
		if (!parseHeaderLine(line, req))
			return PARSE_ERROR;
	}

	return PARSE_OK;
}

static ParseStatus parseChunkedBody(const std::string &body_part, HttpRequest &req)
{
	size_t pos = 0;
	std::string result;

	while (true)
	{
		size_t end_line = body_part.find("\r\n", pos);
		if (end_line == std::string::npos)
			return PARSE_INCOMPLETE;

		std::string size_str = body_part.substr(pos, end_line - pos);
		size_t chunk_size = std::strtol(size_str.c_str(), NULL, 16);

		pos = end_line + 2; // skip \r\n

		if (chunk_size == 0)
		{
			req.body = result;
			req.body_complete = true;
			return PARSE_OK;
		}

		if (pos + chunk_size + 2 > body_part.size())
			return PARSE_INCOMPLETE;

		result.append(body_part.substr(pos, chunk_size));
		pos += chunk_size + 2; // skip data + \r\n
	}

	return PARSE_INCOMPLETE;
}

// === parseHttpBody ===
ParseStatus parseHttpBody(const std::string &buffer, HttpRequest &req)
{
	size_t header_end = buffer.find("\r\n\r\n");
	if (header_end == std::string::npos)
		return PARSE_INCOMPLETE;

	size_t body_start = header_end + 4;
	size_t remaining = buffer.size() - body_start;

	if (req.chunked)
	{
		std::string body_part = buffer.substr(header_end + 4);
		return parseChunkedBody(body_part, req);
	}

	if (req.content_length == 0)
	{
		req.body = "";
		req.body_complete = true;
		return PARSE_OK;
	}

	if (remaining < req.content_length)
		return PARSE_INCOMPLETE;

	req.body = buffer.substr(body_start, req.content_length);
	req.body_complete = true;
	return PARSE_OK;
}


