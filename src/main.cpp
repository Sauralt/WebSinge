#include "../include/HttpParser.hpp"
#include "../include/GlobalConfig.hpp"
#include <iostream>
#include <cstdlib>

// Déclaration externe de la fonction serveur
// (implémentée dans src/http_server.cpp)
extern void runServer(int port, const Config &config);

int main(int argc, char **argv)
{
	(void)argc;
    (void)argv;
    std::cout << "====================================" << std::endl;
    std::cout << "        Mini WebServ (C++98)        " << std::endl;
    std::cout << "====================================" << std::endl;

    // --- Charger la configuration ---
    Config config;
    std::string conf_file = "config/Singe.conf";
    if (!parseConfigFile(conf_file, config))
    {
        std::cerr << "Erreur: impossible de charger le fichier de config: " << conf_file << std::endl;
        return 1;
    }

    if (config.servers.empty())
    {
        std::cerr << "Erreur: aucun serveur défini dans la configuration." << std::endl;
        return 1;
    }

    // Pour l’instant, on prend le premier serveur de la config
    Server &srv = config.servers[0];
    int port = srv.port;
    std::cout << "Server: " << srv.server_name << " listening on port " << port << std::endl;
    std::cout << "Root directory: " << srv.root << std::endl;

    // --- Vérification port valide ---
    if (port <= 0 || port > 65535)
    {
        std::cerr << "Port invalide: " << port << std::endl;
        return 1;
    }

    // --- Lancer le serveur ---
    std::cout << "Server starting..." << std::endl;
    runServer(port, config);

    return 0;
}

