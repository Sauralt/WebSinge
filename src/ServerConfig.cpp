#include "../include/header.hpp"
#include "../include/ServerConfig.hpp"
#include "../include/CGI.hpp"
#include "../include/Mime.hpp"

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

static std::string	uploadFile(const std::string buffer)
{
	std::istringstream file(buffer);
	std::string line;
	std::string filename = "uploaded/ca a echoue clochard";
	std::string content;
	while (getline(file, line))
	{
		size_t equal = line.find("filename=");
		if (equal != std::string::npos)
		{
			filename = "uploaded/" + line.substr(equal + 10, line.size() - (equal + 12));
			getline(file, line);
			getline(file, line);
			while (getline(file, line))
			{
				content += line + "\n";
			}
		}
	}
	std::ofstream outfile(filename.c_str());
	outfile << content << std::endl;
	outfile.close();
	return "File uploaded successfully";
}

static std::string	deleteFile(const std::string buffer)
{
    size_t pos = buffer.find("delete=");
    if (pos == std::string::npos)
        return "No file specified for deletion.";
    size_t end = buffer.find_first_of("& \r\n", pos + 7);
    std::string filename = buffer.substr(pos + 7, end - (pos + 7));
    if (filename.empty())
        return "No file specified for deletion.";
    filename = "uploaded/" + filename;
    if (remove(filename.c_str()) != 0)
        return "Error deleting file.";
    else
        return "File deleted successfully.";
}

static std::string readFileContent(const std::string &path, const std::string buffer, const Server &srv)
{
	std::ifstream file(path.c_str());
	if (!file.is_open())
	{
		if (path.find("/uploaded/") != std::string::npos)
		{
			if (srv.isAllowed("/uploaded", "POST") == false)
				return ("405 method not allowed");
			return uploadFile(buffer);
		}
		else if (path.find("/delete/") != std::string::npos)
		{
			if (srv.isAllowed("/uploaded", "DELETE") == false)
				return ("405 method not allowed");
			return deleteFile(buffer);
		}
		return "";
	}
	std::ostringstream ss;
	ss << file.rdbuf();
	return ss.str();
}

std::string getMimeType(const std::string &path)
{
	if (path.rfind(".html") != std::string::npos)
		return "text/html";
	if (path.rfind(".css") != std::string::npos)
		return "text/css";
	if (path.rfind(".js") != std::string::npos)
		return "application/javascript";
	if (path.rfind(".jpg") != std::string::npos || path.rfind(".jpeg") != std::string::npos)
		return "assets/images/jpeg";
	return "application/octet-stream";
}

static std::string wrapHtmlPage(const std::string &title, const std::string &content)
{
	std::ostringstream ss;
	ss << "<!DOCTYPE html>\n<html>\n<head>\n";
	ss << "<meta charset=\"utf-8\">\n";
	ss << "<title>" << title << "</title>\n";
	ss << "<link rel=\"stylesheet\" href=\"/assets/index.css\">\n";
	ss << "</head>\n<body>\n";
	ss << content;
	ss << "\n</body>\n</html>";
	return ss.str();
}

static std::string buildErrorPage(const std::string &statusCode, const std::string &message)
{
	std::ostringstream ss;
	ss << "<h1>" << statusCode << "</h1>\n";
	ss << "<p>" << message << "</p>\n";
	ss << "<a href=\"/index.html\">Retour à l'accueil</a>\n";
	return wrapHtmlPage(statusCode, ss.str());
}

static	std::string errorPage(const std::string &statusCode, const Server &srv)
{
	std::stringstream s(statusCode.substr(6));
	int code;
	s >> code;

	std::map<std::string, std::string> Errors = srv.getError();
	std::string fullpath = srv.getRoot() + Errors[statusCode];
	std::ifstream file (fullpath.c_str());
	std::string line;
	std::string res;

	if (!file.is_open())
	{
		switch (code)
		{
			case 400:
				return buildHttpResponse("400 Bad Request", "text/html",
				buildErrorPage("400 Bad Request", "Invalid syntaxe"));
			case 403:
				return buildHttpResponse("403 Forbidden", "text/html",
				buildErrorPage("403 Forbidden", "Forbidden access"));
			case 404:
				return buildHttpResponse("404 Not Found", "text/html",
				buildErrorPage("404 Not Found", "Ressource not found"));
			case 405:
				return buildHttpResponse("405 Method Not Allowed", "text/html",
				buildErrorPage("405 Method Not Allowed", "Can not use this functionality, see the config file"));
			case 500:
				return buildHttpResponse("500 Internal Server Error", "text/html",
				buildErrorPage("500 Internal Server Error", "Error while treating request"));
			case 501:
				return buildHttpResponse("501 Not Implemented", "text/html",
				buildErrorPage("501 Not Implemented", "Do not support this request type"));
			case 502:
				return buildHttpResponse("502 Bad Gateway", "text/html",
				buildErrorPage("502 Bad Gateway", "Invalid response"));
		}
	}
	while (getline(file, line))
	{
		res += line;
		res += '\n';
	}
	switch (code)
	{
		case 400:
			return buildHttpResponse("400 Bad request", "text/html", res);
		case 403:
			return buildHttpResponse("403 Forbidden", "text/html", res);
		case 404:
			return buildHttpResponse("404 Not Found", "text/html", res);
		case 405:
			return buildHttpResponse("405 Method Not Allowed", "text/html", res);
		case 500:
			return buildHttpResponse("500 Internal Server Error", "text/html", res);
		case 501:
			return buildHttpResponse("501 Not Implemented", "text/html", res);
		case 502:
			return buildHttpResponse("502 Bad Gateway", "text/html", res);
	}
	return buildHttpResponse("500 Internal Server Error", "text/html", res);
}

std::string handleClient(const Server &srv, std::string buffer, std::vector<pollfd>& _pollfd)
{
	HttpRequest req;
	if (parseHttpMessage(buffer, req) != PARSE_OK)
		return errorPage("Error 400", srv);
	std::string uri = req.getUri();
	if (uri == "/" || uri.empty())
		uri = "/index.html";
	std::string fullPath = srv.getRoot() + uri;

	if (fullPath.find(".py") != std::string::npos && access(fullPath.c_str(), F_OK) != -1)
	{
		CGI temp;
		CGI cgi(temp.ScriptFileName(buffer), req, srv);
		std::string content = cgi.execCGI(buffer, srv, _pollfd);
		if (content.empty())
			return errorPage("Error 502", srv);
		return buildHttpResponse("200 OK", "text/html", content);
	}
	std::string body = readFileContent(fullPath, buffer, srv);
	if (body.empty() && req.getMethod() == "GET")
		return errorPage("Error 404", srv);
	if (fullPath.find("/uploaded/") != std::string::npos)
		return (buildHttpResponse("201 OK", "text/html", body));
	if (fullPath.find("/delete/") != std::string::npos)
		return (buildHttpResponse("200 OK", "text/html", body));
	return buildHttpResponse("200 OK", getMimeType(fullPath), body);
}
