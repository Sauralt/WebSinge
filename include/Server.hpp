#ifndef SERVER_HPP
# define SERVER_HPP

# include "header.hpp"
# include "Location.hpp"

class Server
{
	private:
		int _port;
		std::string _server_name;
		std::string _root;
		std::vector<Location> _locations;
	public:
		Server();
		~Server();
		Server(const Server& copy);
		Server&	operator=(const Server& copy);
		int getPort() const;
		const std::string &getServerName() const;
		const std::string &getRoot() const;
		const std::vector<Location> &getLocations() const;
		void setPort(int port);
		void setServerName(const std::string &name);
		void setRoot(const std::string &root);
		void addLocation(const Location &loc);
		void print() const;
};

void runServer(const Server &srv);

#endif
