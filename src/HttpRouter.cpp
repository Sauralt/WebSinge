#include "../include/HttpParser.hpp"

std::string handleRoot(const HttpRequest &req)
{
	(void)req;
	return "Welcome";
}

std::string handleHello(const HttpRequest &req)
{
	(void)req;
	return "Hello, world!";
}

std::string handleUpload(const HttpRequest &req)
{
	std::ostringstream oss;
	oss << "Upload received (" << req.getBody().size() << " bytes)";
	return oss.str();
}

std::string handleNotFound(const HttpRequest &req)
{
	(void)req;
	return "404 Not Found";
}

std::string routeRequest(const HttpRequest &req)
{
	if (req.getUri() == "/")
		return handleRoot(req);
	else if (req.getUri() == "/hello")
		return handleHello(req);
	else if (req.getUri() == "/upload")
		return handleUpload(req);
	return handleNotFound(req);
}
