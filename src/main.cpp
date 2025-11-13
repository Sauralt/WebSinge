#include "../include/HttpParser.hpp"
#include "../include/GlobalConfig.hpp"
#include <iostream>
#include <cstdlib>

extern void runServer(const Server &srv);

int main(int argc, char **argv)
{
    Config config;

    std::string conf_file = (argc == 1 ? "config/Singe.conf" : argv[1]);
    std::cout << "Using config file: " << conf_file << std::endl;

    if (!parseConfigFile(conf_file, config))
    {
        std::cerr << "Erreur: impossible de charger le fichier de config: " << conf_file << std::endl;
        return 1;
    }
    if (config.getServers().empty())
    {
        std::cerr << "Erreur: aucun serveur défini dans la configuration." << std::endl;
        return 1;
    }
    const Server &srv = config.getServers()[0];
    int port = srv.getPort();
    std::cout << "Server: " << srv.getServerName()
              << " listening on port " << port << std::endl;

    std::cout << "Root directory: " << srv.getRoot() << std::endl;
    if (port <= 0 || port > 65535)
    {
        std::cerr << "Port invalide: " << port << std::endl;
        return 1;
    }
    std::cout << "Server starting..." << std::endl;
    runServer(srv);
    return 0;
}

