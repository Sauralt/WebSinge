#include "../include/Server.hpp"

Server::Server() : _port(80), _clientBodyBufferSize(100), _root("./site") 
{
	this->_errorPages["Error 400"] = "/error/400.html";
	this->_errorPages["Error 403"] = "/error/403.html";
	this->_errorPages["Error 404"] = "/error/404.html";
	this->_errorPages["Error 405"] = "/error/405.html";
	this->_errorPages["Error 500"] = "/error/500.html";
	this->_errorPages["Error 501"] = "/error/501.html";
	this->_errorPages["Error 502"] = "/error/502.html";
	this->_errorPages["Error 505"] = "/error/505.html";
}

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
	this->_errorPages = copy._errorPages;
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

void Server::setHost(const std::string &h)
{ _host = h; }

void	Server::setServerName(const std::string &name)
{ _server_name = name; }

void	Server::setRoot(const std::string &root)
{ _root = root; }


void	Server::addLocation(const Location &loc)
{ _locations.push_back(loc); }

void Server::setErrorPage(int code, const std::string &path)
{
	std::ostringstream ss;
	ss << "Error " << code;
	_errorPages[ss.str()] = path;
	
}

std::map<std::string, std::string>	Server::getError() const
{ return this->_errorPages; }

const std::string& Server::getHost() const
{ return _host; }

bool	Server::isAllowed(std::string name, std::string method) const
{
	if (name == "temp")
		return true;
	for (size_t i = 0; i < this->_locations.size(); i++)
	{
		if (this->_locations[i].getPath() == name)
		{
			if (this->_locations[i].isMethodAllowed(method) == true)
				return true;
		}
	}
	return false;
}
