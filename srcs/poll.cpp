#include "include/header.hpp"

int	main()
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		std::cout << "Failed to create socket. errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}

	const int enable = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int)) < 0)
		std::cerr << "setsockopt(SO_REUSEPORT) failed" << std::endl;

	sockaddr_in sockaddr;
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = INADDR_ANY;
	sockaddr.sin_port = htons(8080);

	if (bind(sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0)
	{
		std::cerr << "Failed to bind to port 9999. errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}

	if (listen(sockfd, 10) < 0)
	{
		std::cerr << "Failed to listen on socket. errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}


	std::vector<pollfd> pollrequest;
	pollfd firstpoll;
	firstpoll.fd = sockfd;
	firstpoll.events = POLLIN;
	pollrequest.push_back(firstpoll);

	while (true)
	{
		int nready = poll(pollrequest.data(), pollrequest.size(), -1);

		for (size_t i = 0; i < pollrequest.size(); ++i)
		{
			if (pollrequest[i].revents & POLLIN)
			{
				if (pollrequest[i].fd == sockfd)
				{
						sockaddr_in client;
						socklen_t addrlen = sizeof(client);
						int connection = accept(sockfd, (struct sockaddr*)&client, (socklen_t*)&addrlen);
						if (connection < 0)
						{
							std::cerr << "Failed to grab connection. errno: " << errno << std::endl;
							exit(EXIT_FAILURE);
						}
						else
						{
							pollfd newfd;
							newfd.fd = connection;
							newfd.events = POLLIN;
							newfd.revents = 0;
							pollrequest.push_back(newfd);
						}
				}
				else
				{
					char buffer[101];
					ssize_t bytesRead = recv(pollrequest[i].fd, buffer, 100, 0);
					if (bytesRead <= 0)
					{
							close(pollrequest[i].fd);
							pollrequest.erase(pollrequest.begin() + i);
							i--;
					}
					else
					{
						buffer[bytesRead] = '\0';
						std::cout << "message = " << buffer;
						std::string response = "End";
						send(pollrequest[i].fd, response.c_str(), response.size(), 0);
					}
				}
			}
		}
	}
	return 0;
}