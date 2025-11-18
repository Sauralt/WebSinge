#include "../include/GlobalConfig.hpp"
#include "../include/header.hpp"
#include <set>

Config::Config()
{}

Config::~Config()
{}

Config::Config(Config& copy)
{
	*this = copy;
}

Config& Config::operator=(Config& copy)
{
	this->_servers = copy._servers;
	return *this;
}

const std::vector<Server> &Config::getServers() const
{ return _servers; }

void Config::addServer(const Server &srv)
{ _servers.push_back(srv); }

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
        if (line.empty() || line[0] == '#')
            continue;
        static std::set<std::string> server_keys;
        if (line.find("server") == 0 && line.find("{") != std::string::npos)
        {
            in_server = true;
            in_location = false;
            current_server = Server();
            server_keys.clear(); // reset for new server block
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
            size_t pos_brace = line.find('{');
            std::string path = line.substr(8, pos_brace - 8);
            trim(path);
            if (!path.empty() && path[path.size()-1] == ';')
                path.erase(path.size()-1, 1);
            trim(path);

            current_location = Location();
            current_location.setPath(path);
            std::cout << "[CFG] New location path: '" << path << "'" << std::endl;
            in_location = true;
            continue;
        }
        size_t pos = line.find(' ');
        if (pos == std::string::npos)
            continue;
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
                current_location.setIndexFile(val);
            else if (lkey == "upload")
                current_location.setAllowUpload(lower(val) == "true");
        }
        else if (in_server)
        {
            if (lkey == "host" || lkey == "server_name" || lkey == "root" || lkey == "listen" || lkey == "port")
            {
                if (server_keys.count(lkey))
                {
                    std::cerr << "Erreur: clé '" << lkey << "' dupliquée dans le bloc server (ligne " << lineno << ")" << std::endl;
                    return false;
                }
                server_keys.insert(lkey);
            }
            if (lkey == "port" || lkey == "listen")
            {
                int port = std::atoi(val.c_str());
                if (port <= 0) port = 80;
                current_server.setPort(port);
                std::cout << "[CFG] Parsed port: " << port << std::endl;
            }
            else if (lkey == "server_name")
                current_server.setServerName(val);
            else if (lkey == "root") 
            {
                current_server.setRoot(val);
            }
            else if (lkey == "host") {
                if (val != "0.0.0.0" && val != "127.0.0.1") {
                    std::cerr << "Erreur: le host doit être '0.0.0.0' ou '127.0.0.1' (ligne " << lineno << ")" << std::endl;
                    return false;
                }
                current_server.setHost(val);
            }
        }
    }
	file.close();
	return true;
}
