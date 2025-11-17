#ifndef CGI_HPP
# define CGI_HPP
# include "header.hpp"
# include "HttpParser.hpp"

class CGI
{
    private:
        std::map<std::string, std::string> _env;
    public:
        CGI();
        CGI(std::string CGIPath, HttpRequest &req);
        ~CGI();
        CGI(CGI& copy);
        CGI&    operator=(CGI& copy);
        std::string    execCGI(std::string cgi);
        char**    MapToChar();
        void    initEnv(std::string CGIPath, HttpRequest& req);
        std::string    ScriptFileName(std::string request);
};

#endif
