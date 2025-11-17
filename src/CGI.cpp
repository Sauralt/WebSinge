#include "../include/CGI.hpp"

CGI::CGI()
{
}

CGI::CGI(std::string CGIPath, HttpRequest &req)
{
	this->initEnv(CGIPath, req);
}

CGI::~CGI()
{}

CGI::CGI(CGI& copy)
{
	this->_env = copy._env;
}

CGI&	CGI::operator=(CGI& copy)
{
	this->_env = copy._env;
	return *this;
}

void	CGI::initEnv(std::string CGIPath, HttpRequest& req)
{
	std::string str;
	std::stringstream s;
	s << req.getContentLength();
	str = s.str();
	this->_env["CONTENT_LENGTH"] = str;
	this->_env["METHOD"] = req.getMethod();
	this->_env["URI"] = req.getUri();
	this->_env["QUERY_STRING"] = req.getQueryString();
	this->_env["HTTP_VER"] = req.getHttpVersion();
	this->_env["BODY"] = req.getBody();
	this->_env["REDIRECT"] = "200";
	this->_env["COMMON_GATEWAY_INTERFACE"] = "CGI/1.1";
	this->_env["CONTENT_TYPE"] = req.getHeaders().find("Content-Type")->second;
	this->_env["HTTP_PROTOCOLE"] = "HTTP/1.1";
	this->_env["SCRIPT"] = CGIPath;
}

char**	CGI::MapToChar()
{
	char **envp = new char*[this->_env.size() + 1];
	size_t i = 0;

	for (std::map<std::string, std::string>::iterator it = this->_env.begin();
		it != this->_env.end(); ++it)
	{
		std::string entry = it->first + "=" + it->second;
		envp[i] = new char[entry.size() + 1];
		envp[i] = strcpy(envp[i], (const char*)entry.c_str());
		++i;
	}
	envp[i] = NULL;
	return envp;
}

std::string	CGI::ScriptFileName(std::string request)
{
	std::string	res;

	for (std::string::iterator it = request.begin() + 4; (*it) != '?'; it++)
		res.push_back((*it));
	return res;
}

std::string	CGI::execCGI(std::string request)
{
	pid_t	pid;
	char**	env = this->MapToChar();
	std::string	filename = ScriptFileName(request);
	std::string	cgi;
	if (!env)
		return NULL;
	int Stdin = dup(STDIN_FILENO);
	int Stdout = dup(STDOUT_FILENO);
	FILE *infile = tmpfile();
	FILE *outfile = tmpfile();
	long	fdIn = fileno(infile);
	long	fdOut = fileno(outfile);
	write(fdIn, request.c_str(), request.size());
	pid = fork();
	if (pid < 0)
		return NULL;
	if (pid == 0)
	{
		dup2(fdIn, STDIN_FILENO);
		dup2(fdOut, STDOUT_FILENO);
		char * const * nil = NULL;
		execve(filename.c_str(), nil, env);
		write(STDOUT_FILENO, "Status: 500\r\n\r\n", 15);
	}
	else
	{
		char	buffer[100];
		waitpid(-1, NULL, 0);
		int ret = 1;
		while (ret > 0)
		{
			ret = read(fdOut, buffer, 99);
			buffer[100] = '\0';
			cgi += buffer;
		}
	}
	dup2(Stdin, STDIN_FILENO);
	dup2(Stdout, STDOUT_FILENO);
	fclose(infile);
	fclose(outfile);
	close(fdIn);
	close(fdOut);
	close(Stdin);
	close(Stdout);
	for (size_t i = 0; env[i]; i++)
		delete[] env[i];
	delete[] env;
	if (!pid)
		exit(0);
	return (cgi);
}
