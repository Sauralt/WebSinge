#include "../include/HttpParser.hpp"
#include "../include/GlobalConfig.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/stat.h>

extern std::string routeRequest(const HttpRequest &req);

static int createServerSocket(int port)
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		return -1;

	int opt = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	struct sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);

	if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		return -1;
	if (listen(sockfd, 10) < 0)
		return -1;

	return sockfd;
}

static bool fileExists(const std::string &path)
{
	struct stat st;
	return (stat(path.c_str(), &st) == 0 && !S_ISDIR(st.st_mode));
}

// --- Lecture d’un fichier en string ---
static std::string readFile(const std::string &path)
{
	std::ifstream f(path.c_str(), std::ios::in | std::ios::binary);
	if (!f)
		return "";

	std::ostringstream ss;
	ss << f.rdbuf();
	return ss.str();
}

// --- Trim début/fin d'une string ---
static void trim(std::string &s)
{
	s.erase(0, s.find_first_not_of(" \t\r\n"));
	s.erase(s.find_last_not_of(" \t\r\n") + 1);
}

// --- Handle client avec fichiers statiques ---
static void handleClient(int client_fd, const Server &srv)
{
	char buffer[4096];
	std::string request;
	ssize_t bytes;

	while ((bytes = recv(client_fd, buffer, sizeof(buffer), 0)) > 0)
	{
		request.append(buffer, bytes);

		HttpRequest req;
		if (parseHttpMessage(request, req) != PARSE_OK)
			continue;

		if (parseHttpBody(request, req) != PARSE_OK)
			continue;

		// --- Gestion fichiers statiques ---
		std::string file_path;
		bool found = false;

		for (size_t i = 0; i < srv.locations.size(); ++i)
		{
			const Location &loc = srv.locations[i];
			std::string loc_path = loc.path;
			trim(loc_path);

			if (req.uri == loc_path || (loc_path == "/" && req.uri == "/"))
			{
				if (req.uri == "/" && !loc.index_file.empty())
					file_path = srv.root + "/" + loc.index_file;
				else
					file_path = srv.root + req.uri;

				found = true;
				break;
			}
		}

		if (!found)
			file_path = srv.root + req.uri;

		std::string body;
		int status = 200;
		std::string contentType = "text/plain";

		if (!fileExists(file_path))
		{
			body = "404 Not Found";
			status = 404;
			contentType = "text/plain";
		}
		else
		{
			body = readFile(file_path);

			if (file_path.size() >= 5 &&
				file_path.substr(file_path.size() - 5) == ".html")
				contentType = "text/html";
			else if (file_path.size() >= 4 &&
					 file_path.substr(file_path.size() - 4) == ".css")
				contentType = "text/css";
			else if (file_path.size() >= 4 &&
					 file_path.substr(file_path.size() - 4) == ".jpg")
				contentType = "image/jpeg";
		}

		std::ostringstream oss;
		oss << "HTTP/1.1 " << status;
		if (status == 200) oss << " OK\r\n";
		else oss << " Not Found\r\n";
		oss << "Content-Length: " << body.size() << "\r\n";
		oss << "Content-Type: " << contentType << "\r\n";
		oss << "\r\n";
		oss << body;

		std::string response = oss.str();
		send(client_fd, response.c_str(), response.size(), 0);
		close(client_fd);
		return;
	}

	// Si on sort de la boucle recv → bad request
	std::string bad = "HTTP/1.1 400 Bad Request\r\nContent-Length: 11\r\nContent-Type: text/plain\r\n\r\nBad Request";
	send(client_fd, bad.c_str(), bad.size(), 0);
	close(client_fd);
}



// --- Boucle principale du serveur ---
void runServer(int port, const Config &config)
{
	int sockfd = createServerSocket(port);
	if (sockfd < 0)
	{
		std::cerr << "Erreur: impossible de démarrer le serveur sur le port " << port << std::endl;
		return;
	}

	std::cout << "Server running on port " << port << "..." << std::endl;

	// Pour l’instant, on prend le premier serveur
	const Server &srv = config.servers[0];

	while (true)
	{
		int client_fd = accept(sockfd, NULL, NULL);
		if (client_fd < 0)
			continue;
		// Gestion du client
		handleClient(client_fd, srv);
	}

	close(sockfd);
}
