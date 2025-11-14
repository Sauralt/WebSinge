#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP
# include "HttpParser.hpp"
# include "Server.hpp"

std::string	handleClient(const Server &srv, std::string buffer);

#endif
