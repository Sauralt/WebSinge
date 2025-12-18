#include "../include/poll.hpp"

Poll::Poll()
{}

Poll::~Poll()
{ 
    for (size_t i = 0; i < _listeningsock.size(); i++)
        close(_listeningsock[i]);
    for (size_t i = 0; i < _pollrequest.size(); i++)
        close(_pollrequest[i].fd);
}


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

std::vector<pollfd>&	Poll::getPollRequest()
{ return _pollrequest; }

void	Poll::addPID(int& fd, pid_t& pid)
{ _pids[fd] = pid; }

void	Poll::addPollRequest(int& fd)
{
	pollfd newfd;
	newfd.fd = fd;
	newfd.events = POLLIN;
	newfd.revents = 0;
	_pollrequest.push_back(newfd);
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
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
		std::cerr << "setsockopt(SO_REUSEADDR) failed" << std::endl;

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
		return ;
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

int Poll::send_socket(int i, const Server& srv)
{
	int fd = _pollrequest[i].fd;
	int bufSize = srv.getClientBodyBufferSize();
	char* buf = new char[bufSize];

	//if it's cgi outfd
	if (_pids.count(fd))
	{
		while (true)
		{
			ssize_t n = read(fd, buf, bufSize);
			if (n > 0)
				_buffer[fd].append(buf, n);
			if (n <= 0)
				break;
		}
		delete [] buf;
		int status;
		waitpid(_pids[fd], &status, 0);
		std::string cgiOut = _buffer[fd];
		if (!_cgifd.count(fd))
			return i - 1;
		int client_fd = _cgifd.at(fd);
		std::string response = buildHttpResponse("200 OK", "text/html", cgiOut, Location());
		std::cout << "sending response for socket " << this->_pollrequest[i].fd
		<< " in server connected to port " << srv.getPort() << ".\n";
		send(client_fd, response.c_str(), response.size(), 0);
		_buffer.erase(fd);
		_pids.erase(fd);
		close(fd);
		_pollrequest.erase(_pollrequest.begin() + i);
		_cgifd.erase(fd);
		return i - 1;
	}

	//if it's a socket
	ssize_t n = recv(fd, buf, bufSize, 0);
	if (n <= 0)
	{
		close(fd);
		_pollrequest.erase(_pollrequest.begin() + i);
		_buffer.erase(fd);
		_clientsrv.erase(fd);
		delete [] buf;
		return i - 1;
	}
	_buffer[fd].append(buf, n);
	delete [] buf;
	if (!requestIsComplete(_buffer[fd]))
		return i;
	std::string response = handleClient(srv, _buffer[fd], fd);
	if (response == "cgi executing")
	{
		std::cout << "cgi executing\n";
		return i;
	}
	std::cout << "sending response for socket " << this->_pollrequest[i].fd
	<< " in server connected to port " << srv.getPort() << ".\n";
	send(fd, response.c_str(), response.size(), 0);
	close(fd);
	_buffer.erase(fd);
	_clientsrv.erase(fd);
	_pollrequest.erase(_pollrequest.begin() + i);
	return i - 1;
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
		fcntl(sockfd, F_SETFL, O_NONBLOCK);
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
				else if (_pids.count(_pollrequest[i].fd))
					i = send_socket(i, *_clientsrv[_cgifd[_pollrequest[i].fd]]);
				else
					i = send_socket(i, *_clientsrv[this->_pollrequest[i].fd]);
			}
		}
	}
	for (size_t i = 0; i < this->_pollrequest.size(); i++)
		close(this->_pollrequest[i].fd);
}
