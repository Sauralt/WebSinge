#include "../include/header.hpp"
#include "../include/ServerConfig.hpp"

static std::string trim(const std::string &s)
{
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos)
        return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

static std::string buildHttpResponse(const std::string &status,
									const std::string &contentType,
									const std::string &body)
{
	std::ostringstream ss;
	ss << "HTTP/1.1 " << status << "\r\n";
	ss << "Content-Type: " << contentType << "\r\n";
	ss << "Content-Length: " << body.size() << "\r\n";
	ss << "Connection: close\r\n\r\n";
	ss << body;
	return ss.str();
}

static std::string readFileContent(const std::string &path)
{
    std::ifstream file(path.c_str());
    if (!file.is_open())
        return "";
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string	handleClient(const Server &srv, std::string buffer)
{
	HttpRequest req;
	if (parseHttpMessage(buffer, req) != PARSE_OK)
	{
		std::string err = buildHttpResponse("400 Bad Request", "text/plain", "Bad Request");
		return err;
	}
	std::string file_path;
	bool found = false;
	const std::vector<Location> &locations = srv.getLocations();
	for (size_t i = 0; i < locations.size(); ++i)
	{
		const Location &loc = locations[i];
		std::string loc_path = loc.getPath();
		trim(loc_path);
		if (req.getUri() == loc_path || (loc_path == "/" && req.getUri() == "/"))
		{
			if (req.getUri() == "/" && !loc.getIndexFile().empty())
				file_path = srv.getRoot() + "/" + loc.getIndexFile();
			else
				file_path = srv.getRoot() + req.getUri();
			found = true;
			break;
		}
	}
	if (!found)
		file_path = srv.getRoot() + req.getUri();
	std::string body = readFileContent(file_path);
	if (body.empty())
	{
		std::string not_found = buildHttpResponse("404 Not Found", "text/plain", "404 Not Found");
		return not_found;
	}
	std::string content_type = "text/html";
	if (file_path.find(".css") != std::string::npos)
		content_type = "text/css";
	else if (file_path.find(".jpg") != std::string::npos || file_path.find(".jpeg") != std::string::npos)
		content_type = "image/jpeg";
	std::string response = buildHttpResponse("200 OK", content_type, body);
	return response;
}
