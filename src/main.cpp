#include "../include/header.hpp"
#include "../include/GlobalConfig.hpp"
#include "../include/poll.hpp"

int gSignalStatus;

int main(int argc, char **argv)
{
	signal(SIGPIPE, SIG_IGN);
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
	std::cout << "Server starting..." << std::endl;
	Poll polling;
	polling.pollrequest(config.getServers());
	return 0;
}

