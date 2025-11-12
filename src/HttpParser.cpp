#include "../include/HttpParser.hpp"
#include <sstream>
#include <iostream>
#include <cstdlib>


ParseStatus parseHttpMessage(const std::string &buffer, HttpRequest &req)
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

ParseStatus parseHttpBody(const std::string &buffer, HttpRequest &req)
{
    if (req.isChunked())
    {
        std::istringstream iss(buffer);
        std::string result;
        std::string line;
        size_t chunk_size = 0;
        while (std::getline(iss, line))
        {
            std::stringstream ss;
            ss << std::hex << line;
            ss >> chunk_size;

            if (chunk_size == 0)
                break;

            std::string chunk_data(chunk_size, '\0');
            iss.read(&chunk_data[0], chunk_size);
            result += chunk_data;
            iss.ignore(2);
        }
        req.setBody(result);
        req.setBodyComplete(true);
        return PARSE_OK;
    }
    if (req.getContentLength() == 0)
    {
        req.setBody("");
        req.setBodyComplete(true);
        return PARSE_OK;
    }
    if (buffer.size() < req.getContentLength())
        return PARSE_INCOMPLETE;
    req.setBody(buffer.substr(0, req.getContentLength()));
    req.setBodyComplete(true);
    return PARSE_OK;
}
