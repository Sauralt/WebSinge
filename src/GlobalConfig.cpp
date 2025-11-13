#include "../include/GlobalConfig.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <algorithm>
#include <cctype>

static void trim(std::string &s)
{
    size_t start = 0;
    while (start < s.size() && std::isspace((unsigned char)s[start])) ++start;
    size_t end = s.size();
    while (end > start && std::isspace((unsigned char)s[end - 1])) --end;
    s = s.substr(start, end - start);
}

static std::string lower(const std::string &s)
{
    std::string r = s;
    for (size_t i = 0; i < r.size(); ++i) r[i] = std::tolower((unsigned char)r[i]);
    return r;
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
    bool in_server = false;
    bool in_location = false;

    std::string line;
    size_t lineno = 0;
    while (std::getline(file, line))
    {
        ++lineno;
        trim(line);
        if (line.empty() || line[0] == '#') continue;
        if (line == "server {" || line == "server{")
        {
            current_server = Server();
            in_server = true;
            in_location = false;
            continue;
        }
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
        if (line.find("location") == 0)
        {
            size_t pos_loc = line.find("location");
            size_t pos_brace = line.find('{', pos_loc + 8);
            std::string path;
            if (pos_brace != std::string::npos)
                path = line.substr(pos_loc + 8, pos_brace - (pos_loc + 8));
            else
                path = line.substr(pos_loc + 8);
            trim(path);
            if (!path.empty() && path[path.size()-1] == ';') path.erase(path.size()-1, 1);
            trim(path);
            current_location = Location();
            current_location.setPath(path);
            in_location = true;
            continue;
        }
        size_t pos = line.find(' ');
        if (pos == std::string::npos) continue;
        std::string key = line.substr(0, pos);
        std::string val = line.substr(pos + 1);
        trim(key);
        trim(val);
        if (!val.empty() && val[val.size()-1] == ';')
            val.erase(val.size()-1, 1);
        trim(val);
        std::string lkey = lower(key);
        if (in_location)
        {
            if (lkey == "index")
            {
                current_location.setIndexFile(val);
            }
            else if (lkey == "upload")
            {
                trim(val);
                current_location.setAllowUpload(lower(val) == "true");
            }
        }
        else if (in_server)
        {
            if (lkey == "port" || lkey == "listen")
            {
                trim(val);
                std::string portstr = val;
                size_t colon = portstr.rfind(':');
                if (colon != std::string::npos)
                {
                    std::string after = portstr.substr(colon + 1);
                    trim(after);
                    portstr = after;
                }
                trim(portstr);
                int port = std::atoi(portstr.c_str());
                if (port <= 0) port = 80;
                current_server.setPort(port);
                std::cout << "[CFG] Parsed port (line " << lineno << "): " << port << std::endl;
            }
            else if (lkey == "server_name")
            {
                current_server.setServerName(val);
                std::cout << "[CFG] Parsed server_name: " << val << std::endl;
            }
            else if (lkey == "root")
            {
                current_server.setRoot(val);
                std::cout << "[CFG] Parsed root: " << val << std::endl;
            }
        }
    }
    file.close();
    return true;
}
