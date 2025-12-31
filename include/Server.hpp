#ifndef SERVER_HPP
# define SERVER_HPP

# include "header.hpp"
# include "Location.hpp"

class Server
{
	private:
		int									_port;
		int									_clientBodyBufferSize;
		std::string							_server_name;
		std::string							_root;
		std::string							_host;
		std::vector<Location>				_locations;
		std::map<std::string, std::string>	_errorPages;
	public:
		Server();
		~Server();
		Server(const Server& copy);
		Server&	operator=(const Server& copy);
		int		getPort() const;
		void	setClientBodyBufferSize(int size);
		size_t	getClientBodyBufferSize() const ;
		const std::string &getServerName() const;
		const std::string &getRoot() const;
		const std::vector<Location> &getLocations() const;
		void setPort(int port);
		void setHost(const std::string &h);
		void setServerName(const std::string &name);
		void setRoot(const std::string &root);
		void addLocation(const Location &loc);
		void setErrorPage(int code, const std::string &path);
		const std::string& getHost() const;
		bool isAllowed(std::string name, std::string method) const;
		std::map<std::string, std::string> getError() const;
};

#endif
