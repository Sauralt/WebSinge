#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP
# include "HttpParser.hpp"
# include "Server.hpp"
# include "CGI.hpp"

std::string	handleClient(const Server &srv, std::string buffer, std::vector<pollfd>& _pollfd);

#endif
