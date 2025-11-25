#include "../include/Server.hpp"

Server::Server() : _port(80), _upload_store(false), _clientBodyBufferSize(100)
{}

Server::~Server()
{}

Server::Server(const Server& copy)
{
	*this = copy;
}

Server&	Server::operator=(const Server& copy)
{
	this->_locations = copy._locations;
	this->_port = copy._port;
	this->_root = copy._root;
	this->_server_name = copy._server_name;
	this->_clientBodyBufferSize = copy._clientBodyBufferSize;
	return *this;
}

int Server::getPort() const
{ return _port; }

const std::string	&Server::getServerName() const
{ return _server_name; }

void Server::setClientBodyBufferSize(int size)
{
    _clientBodyBufferSize = size;
}

int Server::getClientBodyBufferSize() const
{
    return _clientBodyBufferSize;
}

const std::string	&Server::getRoot() const
{ return _root; }

const std::vector<Location>	&Server::getLocations() const
{ return _locations; }

void	Server::setPort(int port)
{ _port = port; }

void	Server::setServerName(const std::string &name)
{ _server_name = name; }

void	Server::setRoot(const std::string &root)
{ _root = root; }

void	Server::addLocation(const Location &loc)
{ _locations.push_back(loc); }

void	Server::print() const
{
	std::cout	<< "Server: " << _server_name
				<< " on port " << _port
				<< " root=" << _root << std::endl;
	for (size_t i = 0; i < _locations.size(); ++i)
		_locations[i].print();
}
