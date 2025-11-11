#include "../include/GlobalConfig.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cstdlib>

// --- trim début/fin ---
static void trim(std::string &s)
{
    s.erase(0, s.find_first_not_of(" \t\r\n"));
    s.erase(s.find_last_not_of(" \t\r\n") + 1);
}

// --- remove trailing ';' ---
static void removeSemicolon(std::string &s)
{
    trim(s);
    if (!s.empty() && s[s.size() - 1] == ';')
        s.erase(s.size() - 1);
}

bool parseConfigFile(const std::string &filename, Config &config)
{
    std::ifstream f(filename.c_str());
    if (!f)
        return false;

    std::string line;
    Server current_server;
    Location current_location;
    bool in_server = false;
    bool in_location = false;

    while (std::getline(f, line))
    {
        trim(line);
        if (line.empty() || line[0] == '#')
            continue;

        if (line.find("server {") == 0)
        {
            in_server = true;
            current_server = Server();
            continue;
        }
        else if (line.find("}") == 0)
        {
            if (in_location)
            {
                current_server.locations.push_back(current_location);
                in_location = false;
            }
            else if (in_server)
            {
                config.servers.push_back(current_server);
                in_server = false;
            }
            continue;
        }

        if (in_server)
        {
            if (line.find("listen") == 0)
            {
                std::string val = line.substr(6);
                trim(val);
                removeSemicolon(val);
                current_server.port = std::atoi(val.c_str());
            }
            else if (line.find("server_name") == 0)
            {
                std::string val = line.substr(11);
                trim(val);
                removeSemicolon(val);
                current_server.server_name = val;
            }
            else if (line.find("root") == 0)
            {
                std::string val = line.substr(4);
                trim(val);
                removeSemicolon(val);
                current_server.root = val;
            }
            else if (line.find("location") == 0)
            {
                in_location = true;
                current_location = Location();
                std::string val = line.substr(8);
                trim(val);
                removeSemicolon(val);
                current_location.path = val;
            }
            else if (in_location)
            {
                if (line.find("index") == 0)
                {
                    std::string val = line.substr(5);
                    trim(val);
                    removeSemicolon(val);
                    current_location.index_file = val;
                }
                else if (line.find("upload") == 0)
                {
                    std::string val = line.substr(6);
                    trim(val);
                    removeSemicolon(val);
                    current_location.allow_upload = (val == "true");
                }
            }
        }
    }

    return true;
}

