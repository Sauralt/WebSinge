#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <map>
#include <vector>

struct Location {
    std::string path;
    std::string index_file;
    bool allow_upload;

    Location() : allow_upload(false) {}
};

struct Server {
    int port;
    std::string server_name;
    std::string root;
    std::vector<Location> locations;

    Server() : port(80) {}
};

struct Config {
    std::vector<Server> servers;
};

bool parseConfigFile(const std::string &filename, Config &config);

#endif
