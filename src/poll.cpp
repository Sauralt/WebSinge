#include "../include/poll.hpp"

Poll::Poll()
{}

Poll::~Poll()
{}

Poll::Poll(Poll& copy)
{
	this->_pollrequest = copy._pollrequest;
}

Poll&	Poll::operator=(Poll& copy)
{
	this->_pollrequest = copy._pollrequest;
	return *this;
}

void	signal_handler(int sig)
{
	gSignalStatus = sig;
}

int	Poll::socketfd(const Server& srv)
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		std::cout << "Failed to create socket." << std::endl;
		std::exit(EXIT_FAILURE);
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
		std::exit(EXIT_FAILURE);
	}

	if (listen(sockfd, 10) < 0)
	{
		std::cerr << "Failed to listen on socket." << std::endl;
		std::exit(EXIT_FAILURE);
	}
	return sockfd;
}

void	Poll::add_socket(int sockfd)
{
	sockaddr_in client;
	socklen_t addrlen = sizeof(client);
	int connection = accept(sockfd, (struct sockaddr*)&client, (socklen_t*)&addrlen);
	if (connection < 0)
	{
		std::cerr << "Failed to grab connection." << std::endl;
		std::exit(EXIT_FAILURE);
	}
	else
	{
		this->_clientsrv[connection] = this->_listensrv[sockfd];
		fcntl(connection, F_SETFL, O_NONBLOCK);
		this->_buffer[connection] = "";
		std::cout << "reading request from socket " << connection << " in server connected to port " << this->_clientsrv[connection]->getPort() << ".\n";
		pollfd newfd;
		newfd.fd = connection;
		newfd.events = POLLIN;
		newfd.revents = 0;
		this->_pollrequest.push_back(newfd);
	}
}

bool requestIsComplete(const std::string &buffer)
{
	size_t header_end = buffer.find("\r\n\r\n");
	if (header_end == std::string::npos)
		return false;
	std::string headers = buffer.substr(0, header_end);
	size_t pos = headers.find("Content-Length:");
	if (pos != std::string::npos)
	{
		pos += 16;
		while (pos < headers.size() && (headers[pos] == ' ' || headers[pos] == '\t'))
			pos++;
		int contentLength;
		std::string str = headers.c_str() + pos;
		std::istringstream(str) >> contentLength;
		size_t total_needed = header_end + 4 + contentLength;
		return buffer.size() >= total_needed;
	}
	return true;
}


int	Poll::send_socket(int i, const Server& srv)
{
	int bufSize = srv.getClientBodyBufferSize();
	char* buffer = new char[bufSize];
	ssize_t bytesRead = recv(this->_pollrequest[i].fd, buffer, bufSize - 1, 0);
	if (bytesRead <= 0)
	{
			close(this->_pollrequest[i].fd);
			this->_pollrequest.erase(this->_pollrequest.begin() + i);
			delete [] buffer;
			i--;
	}
	else
	{
		buffer[bytesRead] = '\0';
		this->_buffer[this->_pollrequest[i].fd] += buffer;
		if (requestIsComplete(this->_buffer[this->_pollrequest[i].fd]))
		{
			std::string response = handleClient(srv, this->_buffer[this->_pollrequest[i].fd], this->_pollrequest);
			std::cout << "sending response for socket " << this->_pollrequest[i].fd << " in server connected to port " << this->_clientsrv[this->_pollrequest[i].fd]->getPort() << ".\n";
			send(this->_pollrequest[i].fd, response.c_str(), response.size(), 0);
			this->_buffer.erase(this->_pollrequest[i].fd);
			this->_buffer.clear();
		}
		delete [] buffer;
	}
	return i;
}

bool	Poll::listeningSock(int fd)
{
	for (std::vector<int>::iterator it = this->_listeningsock.begin();
		it < this->_listeningsock.end(); it++)
	{
		if ((*it) == fd)
			return true;
	}
	return false;
}

void	Poll::pollrequest(std::vector<Server>& servers)
{
	gSignalStatus = 0;
	std::signal(SIGINT, signal_handler);
	for (std::vector<Server>::iterator srv = servers.begin();
		srv < servers.end(); srv++)
	{
		int	sockfd = socketfd((*srv));
		pollfd firstpoll;
		firstpoll.fd = sockfd;
		firstpoll.events = POLLIN;
		firstpoll.revents = 0;
		this->_pollrequest.push_back(firstpoll);
		this->_listeningsock.push_back(sockfd);
		this->_listensrv[sockfd] = &(*srv);
	}

	while (gSignalStatus == 0)
	{
		poll(this->_pollrequest.data(), this->_pollrequest.size(), -1);
		for (size_t i = 0; i < this->_pollrequest.size(); ++i)
		{
			if (this->_pollrequest[i].revents & POLLIN)
			{
				if (this->listeningSock(this->_pollrequest[i].fd))
					add_socket(this->_pollrequest[i].fd);
				else
					i = send_socket(i, *_clientsrv[this->_pollrequest[i].fd]);
			}
		}
	}
	for (size_t i = 0; i < this->_pollrequest.size(); i++)
		close(this->_pollrequest[i].fd);
}
