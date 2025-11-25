#ifndef SERVER_HPP
# define SERVER_HPP

# include "header.hpp"
# include "Location.hpp"

class Server
{
	private:
		int _port;
		bool _upload_store;
		std::string _server_name;
		std::string _root;
    	std::string _host;
		std::vector<Location> _locations;
		int _clientBodyBufferSize;
	public:
		Server();
		~Server();
		Server(const Server& copy);
		Server&	operator=(const Server& copy);
		int getPort() const;
		void setClientBodyBufferSize(int size);
    	int  getClientBodyBufferSize() const ;
		const std::string &getServerName() const;
		const std::string &getRoot() const;
		const std::vector<Location> &getLocations() const;
		void setUploadStore(bool value) { _upload_store = value; }
   		bool getUploadStore() const { return _upload_store; }
		void setPort(int port);
    	void setHost(const std::string &h) { _host = h; }
		void setServerName(const std::string &name);
		void setRoot(const std::string &root);
		void addLocation(const Location &loc);
		void print() const;
		const std::string& getHost() const { return _host; }
};

#endif
