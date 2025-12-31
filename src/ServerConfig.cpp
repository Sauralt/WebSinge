#include "../include/header.hpp"
#include "../include/CGI.hpp"
#include "../include/Mime.hpp"
#include "../include/poll.hpp"

std::string buildHttpResponse(const std::string &status,
									const std::string &contentType,
									const std::string &body,
									const Location &loc)
{
	if (status == "200 OK" || status == "201 Created")
	{
		if (loc.getReturnValue() != "0")
		{
			Location temp;
			return buildHttpResponse(loc.getReturnValue(), "text/html", loc.getBody(), temp);
		}
	}
	std::ostringstream ss;
	ss << "HTTP/1.1 " << status << "\r\n";
	ss << "Content-Type: " << contentType << "\r\n";
	ss << "Content-Length: " << body.size() << "\r\n";
	ss << "Connection: close\r\n\r\n";
	ss << body;
	return ss.str();
}

static std::string	uploadFile(const std::string buffer, const Server &srv)
{
	std::string path = "uploaded/";
	for (size_t i = 0; i < srv.getLocations().size(); i++)
	{
		if (srv.getLocations()[i].getPath() == "/uploaded")
			path = srv.getLocations()[i].getUploadedStore();
	}
	if (access(path.c_str(), F_OK) != 0)
		return "Error 404";
	if (access(path.c_str(), X_OK | W_OK) != 0)
		return "Error 403";
	std::istringstream file(buffer);
	std::string line;
	std::string filename;
	std::string content;
	while (getline(file, line))
	{
		size_t equal = line.find("filename=");
		if (equal != std::string::npos)
		{
			filename = path + line.substr(equal + 10, line.size() - (equal + 12));
			if (filename == "uploaded/")
				return "No file to upload.";
			getline(file, line);
			getline(file, line);
			while (getline(file, line))
			{
				if (line.find("WebKit") == std::string::npos)
					content += line + "\n";
			}
		}
	}
	if (filename == "")
		return "No file to upload";
	std::ofstream outfile(filename.c_str());
	outfile << content << std::endl;
	outfile.close();
	return "File uploaded successfully";
}

static std::string	deleteFile(const std::string buffer, const Server &srv, std::string fullpath)
{
	std::string path = "uploaded/";
	for (size_t i = 0; i < srv.getLocations().size(); i++)
	{
		if (srv.getLocations()[i].getPath() == "/uploaded")
			path = srv.getLocations()[i].getUploadedStore();
	}
	size_t pos = buffer.find("delete=");
	if (pos == std::string::npos)
	{
		path = ".";
		path += fullpath.substr(6);
		if (access(path.c_str(), F_OK) != 0)
			return "No file specified for deletion.";
		if (access(path.c_str(), X_OK | W_OK) != 0)
			return "Error 403";
		if (remove(path.c_str()) != 0)
			return "Error 404";
		else
			return "File deleted successfully.";
	}
	size_t end = buffer.find_first_of("& \r\n", pos + 7);
	std::string filename = buffer.substr(pos + 7, end - (pos + 7));
	if (filename.empty())
		return "No file specified for deletion.";
	if (access(path.c_str(), X_OK | W_OK) != 0)
		return "Error 403";
	filename = path + filename;
	if (remove(filename.c_str()) != 0)
		return "Error 404";
	else
		return "File deleted successfully.";
}

static std::string readFileContent(const std::string &path, const std::string buffer, const Server &srv, HttpRequest &req)
{
	std::ifstream file(path.c_str());
	if (!file.is_open())
	{
		if (path.find("/uploaded/") != std::string::npos && req.getMethod() == "POST")
		{
			if (srv.isAllowed("/uploaded", "POST") == false)
				return ("Error 405");
			return uploadFile(buffer, srv);
		}
		else if (path.find("/uploaded") != std::string::npos && req.getMethod() == "DELETE")
		{
			if (srv.isAllowed("/uploaded", "DELETE") == false)
				return ("Error 405");
			return deleteFile(buffer, srv, path);
		}
		else if (path.find("/delete/") != std::string::npos)
		{
			if (srv.isAllowed("/uploaded", "DELETE") == false)
				return ("Error 405");
			return deleteFile(buffer, srv, path);
		}
		return "";
	}
	std::string temp = "temp";
	for (size_t i = 0; i < srv.getLocations().size(); i++)
	{
		if (srv.getLocations()[i].getPath().find(path) != std::string::npos)
			temp = srv.getLocations()[i].getPath();
	}
	if (srv.isAllowed(temp, req.getMethod()) == false)
		return "Error 405";
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
	return "text/html";
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

std::string errorPage(const std::string &statusCode, const Server &srv, const Location &loc)
{
	std::stringstream s(statusCode.substr(6));
	int code;
	s >> code;
	std::map<std::string, std::string> Errors = srv.getError();
	std::string fullpath = srv.getRoot() + Errors[statusCode];
	std::ifstream file (fullpath.c_str());
	std::string line;
	std::string res;
	if (!file.is_open() || fullpath == srv.getRoot())
	{
		switch (code)
		{
			case 400:
				return buildHttpResponse("400 Bad Request", "text/html",
				buildErrorPage("400 Bad Request", "Invalid syntaxe"), loc);
			case 403:
				return buildHttpResponse("403 Forbidden", "text/html",
				buildErrorPage("403 Forbidden", "Forbidden access"), loc);
			case 404:
				return buildHttpResponse("404 Not Found", "text/html",
				buildErrorPage("404 Not Found", "Ressource not found"), loc);
			case 405:
				return buildHttpResponse("405 Method Not Allowed", "text/html",
				buildErrorPage("405 Method Not Allowed", "Can not use this functionality, see the config file"), loc);
			case 413:
				return buildHttpResponse("413 Payload Too Large", "text/html",
				buildErrorPage("413 Payload Too Large", "Request body too large"), loc);
			case 500:
				return buildHttpResponse("500 Internal Server Error", "text/html",
				buildErrorPage("500 Internal Server Error", "Error while treating request"), loc);
			case 501:
				return buildHttpResponse("501 Not Implemented", "text/html",
				buildErrorPage("501 Not Implemented", "Do not support this request type"), loc);
			case 502:
				return buildHttpResponse("502 Bad Gateway", "text/html",
				buildErrorPage("502 Bad Gateway", "Invalid response"), loc);
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
			return buildHttpResponse("400 Bad request", "text/html", res, loc);
		case 403:
			return buildHttpResponse("403 Forbidden", "text/html", res, loc);
		case 404:
			return buildHttpResponse("404 Not Found", "text/html", res, loc);
		case 405:
			return buildHttpResponse("405 Method Not Allowed", "text/html", res, loc);
		case 413:
			return buildHttpResponse("413 Payload Too Large", "text/html", res, loc);
		case 500:
			return buildHttpResponse("500 Internal Server Error", "text/html", res, loc);
		case 501:
			return buildHttpResponse("501 Not Implemented", "text/html", res, loc);
		case 502:
			return buildHttpResponse("502 Bad Gateway", "text/html", res, loc);
	}
	return buildHttpResponse("500 Internal Server Error", "text/html", res, loc);
}

static std::string	isDir(std::string &fullPath, const Server &srv, const Location loc)
{
	bool autoindex = false;
	for (size_t i = 0; i < srv.getLocations().size(); i++)
	{
		if (fullPath.find(srv.getLocations()[i].getPath()) != std::string::npos)
			autoindex = srv.getLocations()[i].getAutoIndex();
	}
	if (autoindex == false)
		return errorPage("Error 403", srv, loc);
	std::string fulldir;
	DIR *dir = opendir(fullPath.c_str());
	if (!dir)
		return errorPage("Error 403", srv, loc);
	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL)
	{
		std::string name = entry->d_name;
		fulldir += name + "\n";
	}
	closedir(dir);
	return buildHttpResponse("200 OK", "text/html", fulldir, loc);
}

std::string Poll::handleClient(const Server &srv, std::string buffer, int fd)
{
	//Parsing request
	struct stat s;
	HttpRequest req;
	Location loc;
	if (parseHttpMessage(buffer, req) != PARSE_OK)
		return errorPage("Error 400", srv, loc);
	std::string uri = req.getUri();
	if (uri == "/" || uri.empty())
		uri = "/index.html";
	std::string fullPath = srv.getRoot() + uri;
	if (req.getContentLength() > srv.getClientBodyBufferSize() && req.getMethod() == "POST")
		return errorPage("Error 413", srv, loc);
	if (req.getMethod() != "GET" && req.getMethod() != "POST" && req.getMethod() != "DELETE")
		return errorPage("Error 501", srv, loc);

	//get current location
	for (size_t i = 0; i < srv.getLocations().size(); i++)
	{
		if (fullPath.find(srv.getLocations()[i].getPath()) != std::string::npos)
		{
			loc = srv.getLocations()[i];
			loc.setBody();
			if (loc.getBody() == "Error 404" && loc.getReturnValue() != "0")
				return errorPage(loc.getBody(), srv, loc);
		}
	}
	if (srv.isAllowed(loc.getPath(), req.getMethod()) == false)
		return errorPage("Error 405", srv, loc);

	//get directory content if path = directory
	if (stat(fullPath.c_str(), &s) == 0)
	{
		if (s.st_mode & S_IFDIR)
		{
			std::string indexPath = fullPath;
			if (indexPath.empty() || indexPath[indexPath.size() - 1] != '/')
				indexPath += '/';
			indexPath += "index.html";
			if (access(indexPath.c_str(), F_OK) != -1)
			{
				std::string body = readFileContent(indexPath, buffer, srv, req);
				if (body.empty())
					return errorPage("Error 500", srv, loc);
				return buildHttpResponse("200 OK", getMimeType(indexPath), body, loc);
			}
			return isDir(fullPath, srv, loc);
		}
	}

	//run cgi
	if (fullPath.find(".py") != std::string::npos && access(fullPath.c_str(), F_OK) != -1 && loc.getExt() == ".py")
	{
		CGI temp;
		CGI cgi(temp.ScriptFileName(buffer), req, srv);
		int cgifd = cgi.execCGI(req.getBody(), *this);
		_cgifd[cgifd] = fd;
		if (fd == -1)
			return errorPage("Error 502", srv, loc);
		return "cgi executing";
	}

	//building response
	std::string body = readFileContent(fullPath, buffer, srv, req);
	if (body.empty() && req.getMethod() == "GET")
		return errorPage("Error 404", srv, loc);

	if (fullPath.find("/uploaded/") != std::string::npos && body.find("Error") == std::string::npos && req.getMethod() == "DELETE")
		return (buildHttpResponse("200 OK", "text/html", body, loc));

	if (fullPath.find("/uploaded/") != std::string::npos && body.find("Error") == std::string::npos)
		return (buildHttpResponse("201 Created", "text/html", body, loc));
	else if (fullPath.find("/uploaded/") != std::string::npos)
		return errorPage(body, srv, loc);

	if (fullPath.find("/delete/") != std::string::npos && body.find("Error") == std::string::npos)
		return (buildHttpResponse("200 OK", "text/html", body, loc));
	else if (fullPath.find("/delete/") != std::string::npos)
		return errorPage(body, srv, loc);

	return buildHttpResponse("200 OK", getMimeType(fullPath), body, loc);
}
