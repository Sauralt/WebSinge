#include "../include/GlobalConfig.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <algorithm>
#include <cctype>
#include <functional>

static void trim(std::string &s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                                    std::not1(std::ptr_fun<int, int>(std::isspace))));
    s.erase(std::find_if(s.rbegin(), s.rend(),
                         std::not1(std::ptr_fun<int, int>(std::isspace))).base(),
            s.end());
}

bool parseConfigFile(const std::string &filename, Config &config)
{
    std::ifstream file(filename.c_str());
    if (!file.is_open())
    {
        std::cerr << "Erreur: impossible d'ouvrir le fichier " << filename << std::endl;
        return false;
    }

    Server current_server;
    Location current_location;
    bool in_server = false, in_location = false;

    std::string line;
    while (std::getline(file, line))
    {
        trim(line);
        if (line.empty() || line[0] == '#') continue;

        // Début serveur
        if (line == "server {")
        {
            current_server = Server();
            in_server = true;
            continue;
        }

        // Fin bloc
        if (line == "}")
        {
            if (in_location)
            {
                current_server.addLocation(current_location);
                in_location = false;
            }
            else if (in_server)
            {
                config.addServer(current_server);
                in_server = false;
            }
            continue;
        }

        // Début location
        if (line.find("location") == 0)
        {
            size_t start = line.find(' ');
            size_t end = line.find('{');
            std::string loc_path = line.substr(start, end - start);
            trim(loc_path);
            current_location = Location();
            current_location.setPath(loc_path);
            in_location = true;
            continue;
        }

        // Parsing clé/valeur
        size_t pos = line.find(' ');
        if (pos == std::string::npos) continue;

        std::string key = line.substr(0, pos);
        std::string val = line.substr(pos + 1);
        trim(val);

        if (!val.empty() && val.back() == ';') val.pop_back();
        trim(val);

        if (in_location)
        {
            if (key == "index") current_location.setIndexFile(val);
            else if (key == "upload") current_location.setAllowUpload(val == "true");
        }
        else if (in_server)
        {
            if (key == "port") current_server.setPort(std::atoi(val.c_str()));
            else if (key == "server_name") current_server.setServerName(val);
            else if (key == "root") current_server.setRoot(val);
        }
    }

    file.close();
    return true;
}
