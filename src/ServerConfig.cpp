#include "../include/GlobalConfig.hpp"
#include "../include/HttpParser.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <sys/socket.h>

static std::string intToString(size_t n)
{
    std::ostringstream oss;
    oss << n;
    return oss.str();
}

void handleClient(int client_fd, const Server &server)
{
    char buffer[4096];
    int bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes <= 0)
    {
        close(client_fd);
        return;
    }
    buffer[bytes] = '\0';
    std::string request(buffer);
    std::istringstream req_stream(request);
    std::string method, uri, version;
    req_stream >> method >> uri >> version;
    std::cout << "Requête reçue : " << method << " " << uri << std::endl;
    if (uri == "/")
        uri = "/index.html";
    std::string file_path = server.getRoot() + uri;
    std::ifstream file(file_path.c_str(), std::ios::binary);
    if (!file)
    {
        std::string notFound = "<h1>404 Not Found</h1>";
        std::string response =
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: " + intToString(notFound.size()) + "\r\n\r\n" +
            notFound;
        send(client_fd, response.c_str(), response.size(), 0);
        close(client_fd);
        return;
    }
    std::string contentType = "text/plain";
    if (file_path.find(".html") != std::string::npos)
        contentType = "text/html";
    else if (file_path.find(".css") != std::string::npos)
        contentType = "text/css";
    else if (file_path.find(".png") != std::string::npos)
        contentType = "image/png";
    std::stringstream buffer_stream;
    buffer_stream << file.rdbuf();
    std::string body = buffer_stream.str();

    std::string response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: " + contentType + "\r\n"
        "Content-Length: " + intToString(body.size()) + "\r\n\r\n" +
        body;
    send(client_fd, response.c_str(), response.size(), 0);
    close(client_fd);
}

void runServer(const Server &server)
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "Erreur: impossible de créer le socket\n";
        return;
    }

    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(server.getPort());
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (sockaddr *)&addr, sizeof(addr)) < 0)
    {
        std::cerr << "Erreur: impossible de binder le port " << server.getPort() << "\n";
        close(sockfd);
        return;
    }

    listen(sockfd, 10);
    std::cout << "Serveur démarré sur le port " << server.getPort() << std::endl;

    while (true)
    {
        int client_fd = accept(sockfd, NULL, NULL);
        if (client_fd < 0)
            continue;
        handleClient(client_fd, server);
    }

    close(sockfd);
}
