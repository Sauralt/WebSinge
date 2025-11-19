#ifndef CONFIG_HPP
# define CONFIG_HPP

# include "header.hpp"
# include "Server.hpp"

class Config
{
	private:
		std::vector<Server> _servers;
		int					_servNum;
	public:
	Config();
	~Config();
	Config(Config& copy);
	Config&	operator=(Config& copy);
	std::vector<Server> &getServers();
	int	getservNum();
	void addServer(const Server &srv);
};

bool parseConfigFile(const std::string &filename, Config &config);

#endif

