#include "../include/poll.hpp"

polling::polling()
{}

polling::~polling()
{}

polling::polling(polling& copy)
{
	this->_pollrequest = copy._pollrequest;
}

polling&	polling::operator=(polling& copy)
{
	this->_pollrequest = copy._pollrequest;
	return *this;
}

void	signal_handler(int sig)
{
	gSignalStatus = sig;
}

int	polling::socketfd(const Server& srv)
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		std::cout << "Failed to create socket." << std::endl;
		exit(EXIT_FAILURE);
	}

	const int enable = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int)) < 0)
		std::cerr << "setsockopt(SO_REUSEPORT) failed" << std::endl;

	sockaddr_in sockaddr;
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = INADDR_ANY;
	sockaddr.sin_port = htons(srv.getPort());

	if (bind(sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0)
	{
		std::cerr << "Failed to bind to port :" << srv.getPort() << std::endl;
		exit(EXIT_FAILURE);
	}

	if (listen(sockfd, 10) < 0)
	{
		std::cerr << "Failed to listen on socket." << std::endl;
		exit(EXIT_FAILURE);
	}
	return sockfd;
}

void	polling::add_socket(int sockfd)
{
	sockaddr_in client;
	socklen_t addrlen = sizeof(client);
	int connection = accept(sockfd, (struct sockaddr*)&client, (socklen_t*)&addrlen);
	if (connection < 0)
	{
		std::cerr << "Failed to grab connection." << std::endl;
		exit(EXIT_FAILURE);
	}
	else
	{
		fcntl(connection, F_SETFL, O_NONBLOCK);
		pollfd newfd;
		newfd.fd = connection;
		newfd.events = POLLIN;
		newfd.revents = 0;
		this->_pollrequest.push_back(newfd);
	}
}

int	polling::send_socket(int i, const Server& srv)
{
	char buffer[101];
	ssize_t bytesRead = recv(this->_pollrequest[i].fd, buffer, 100, 0);
	if (bytesRead <= 0)
	{
			close(this->_pollrequest[i].fd);
			this->_pollrequest.erase(this->_pollrequest.begin() + i);
			i--;
	}
	else
	{
		std::cout << buffer << std::endl;
		buffer[bytesRead] = '\0';
		std::string response = handleClient(srv, buffer);
		send(this->_pollrequest[i].fd, response.c_str(), response.size(), 0);
	}
	return i;
}

void	polling::pollrequest(const Server& srv)
{
	gSignalStatus = 0;
	std::signal(SIGINT, signal_handler);
	int	sockfd = socketfd(srv);
	std::vector<pollfd> pollrequest;
	pollfd firstpoll;
	firstpoll.fd = sockfd;
	firstpoll.events = POLLIN;
	this->_pollrequest.push_back(firstpoll);

	while (gSignalStatus == 0)
	{
		poll(this->_pollrequest.data(), this->_pollrequest.size(), -1);

		for (size_t i = 0; i < this->_pollrequest.size(); ++i)
		{
			if (this->_pollrequest[i].revents & POLLIN)
			{
				if (this->_pollrequest[i].fd == sockfd)
					add_socket(sockfd);
				else
					i = send_socket(i, srv);
			}
		}
	}
}
