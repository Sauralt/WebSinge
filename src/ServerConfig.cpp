#include "../include/header.hpp"
#include "../include/ServerConfig.hpp"

static void trim(std::string &s)
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(),
			std::not1(std::ptr_fun<int, int>(std::isspace))));
	s.erase(std::find_if(s.rbegin(), s.rend(),
			std::not1(std::ptr_fun<int, int>(std::isspace))).base(),
			s.end());
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
			std::cout << "Matched location: " << loc_path << std::endl;
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
	else if (file_path.find(".png") != std::string::npos)
		content_type = "image/png";
	else if (file_path.find(".jpg") != std::string::npos || file_path.find(".jpeg") != std::string::npos)
		content_type = "image/jpeg";
	std::string response = buildHttpResponse("200 OK", content_type, body);
	return response;
}

// void runServer(const Server &srv)
// {
// 	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
// 	if (server_fd < 0)
// 	{
// 		std::cerr << "Error: failed to create socket" << std::endl;
// 		return;
// 	}

// 	int opt = 1;
// 	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
// 	struct sockaddr_in address;
// 	std::memset(&address, 0, sizeof(address));
// 	address.sin_family = AF_INET;
// 	address.sin_addr.s_addr = INADDR_ANY;
// 	address.sin_port = htons(srv.getPort());
// 	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
// 	{
// 		std::cerr << "Error: failed to bind on port " << srv.getPort() << std::endl;
// 		close(server_fd);
// 		return;
// 	}
// 	if (listen(server_fd, 10) < 0)
// 	{
// 		std::cerr << "Error: listen() failed" << std::endl;
// 		close(server_fd);
// 		return;
// 	}
// 	std::cout	<< "Server listening on port " << srv.getPort()
// 				<< " (root: " << srv.getRoot() << ")" << std::endl;
// 	while (true)
// 	{
// 		struct sockaddr_in client_addr;
// 		socklen_t addrlen = sizeof(client_addr);
// 		int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addrlen);
// 		if (client_fd < 0)
// 			continue;

// 		handleClient(client_fd, srv);
// 		close(client_fd);
// 	}
// 	close(server_fd);
// }

