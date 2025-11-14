#ifndef CONFIG_HPP
# define CONFIG_HPP

# include "header.hpp"
# include "Server.hpp"

class Config
{
	private:
		std::vector<Server> _servers;

	public:
	Config();
	~Config();
	Config(Config& copy);
	Config&	operator=(Config& copy);
	const std::vector<Server> &getServers() const;
	void addServer(const Server &srv);
};

	bool parseConfigFile(const std::string &filename, Config &config);

#endif

