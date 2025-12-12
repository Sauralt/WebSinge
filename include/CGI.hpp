#ifndef CGI_HPP
# define CGI_HPP
# include "header.hpp"
# include "HttpParser.hpp"
# include "Server.hpp"
# include "poll.hpp"

class CGI
{
	private:
		std::map<std::string, std::string> _env;
	public:
		CGI();
		CGI(std::string CGIPath, HttpRequest &req, const Server &srv);
		~CGI();
		CGI(CGI& copy);
		CGI&	operator=(CGI& copy);
		std::string	execCGI(std::string cgi, const Server &srv, Poll& _poll);
		char**	MapToChar();
		void	initEnv(std::string CGIPath, HttpRequest& req, const Server &srv);
		std::string	ScriptFileName(std::string request);
};

#endif
