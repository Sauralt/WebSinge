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

std::string readFileContent(const std::string &path)
{
    std::ifstream file(path.c_str());
    if (!file.is_open())
        return "";
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
	if (path.rfind(".png") != std::string::npos)
		return "image/png";
	if (path.rfind(".jpg") != std::string::npos || path.rfind(".jpeg") != std::string::npos)
		return "image/jpeg";
	return "application/octet-stream";
}

static std::string buildErrorPage(const std::string &statusCode, const std::string &message)
{
	std::ostringstream ss;
	ss << "<h1>" << statusCode << "</h1>\n";
	ss << "<p>" << message << "</p>\n";
	ss << "<a href=\"/index.html\">Retour à l'accueil</a>\n";
	return wrapHtmlPage(statusCode, ss.str());
}

std::string handleClient(const Server &srv, std::string buffer)
{
    HttpRequest req;
    if (parseHttpMessage(buffer, req) != PARSE_OK)
        return buildHttpResponse("400 Bad Request", "text/html",
        buildErrorPage("400 Bad Request", "La requête est invalide."));

    std::string uri = req.getUri();
    std::string fullPath = srv.getRoot() + uri;

    if (fullPath.find(".py") != std::string::npos)
    {
        CGI temp;
        CGI cgi(temp.ScriptFileName(buffer), req, srv);
        std::string content = cgi.execCGI(buffer, srv);
        if (content.empty())
            return buildHttpResponse("502 Bad Gateway", "text/html",
            buildErrorPage("502 Bad Gateway", "Erreur lors de l'exécution du CGI."));
        return buildHttpResponse("200 OK", "text/html", content);
    }
    std::string body = readFileContent(fullPath);
    if (body.empty())
        return buildHttpResponse("404 Not Found", "text/html", buildErrorPage("404 Not Found", "La ressource demandée est introuvable."));
    return buildHttpResponse("200 OK", getMimeType(fullPath), body);
}

