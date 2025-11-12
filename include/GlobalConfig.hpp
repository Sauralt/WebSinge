#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Server.hpp"
#include "Location.hpp"
#include <vector>

class Config {
    private:
        std::vector<Server> _servers;

    public:
        const std::vector<Server> &getServers() const { return _servers; }
        void addServer(const Server &srv) { _servers.push_back(srv); }
    };

    bool parseConfigFile(const std::string &filename, Config &config);

#endif

