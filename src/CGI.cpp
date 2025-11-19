#include "../include/CGI.hpp"
#include "../include/Server.hpp"

CGI::CGI()
{
}

CGI::CGI(std::string CGIPath, HttpRequest &req, const Server &srv)
{
	this->initEnv(CGIPath, req, srv);
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

void	CGI::initEnv(std::string CGIPath, HttpRequest& req, const Server &srv)
{
	std::string str;
	std::string str2;
	std::stringstream s;
	std::stringstream s2;
	char cwd[1024];
	getcwd(cwd, sizeof(cwd));
	std::string abs = std::string(cwd) + "/site" + CGIPath;
	s << req.getContentLength();
	str = s.str();
	this->_env["CONTENT_LENGTH"] = str;
	s2 << srv.getPort();
	str2 = s2.str();
	this->_env["SERVER_PORT"] = str2;
	this->_env["REQUEST_METHOD"] = req.getMethod();
	this->_env["REQUEST_URI"] = req.getUri();
	this->_env["QUERY_STRING"] = req.getQueryString();
	this->_env["SERVER_PROTOCOL"] = req.getHttpVersion();
	this->_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	this->_env["CONTENT_TYPE"] = req.getHeaderValue("Content-Type");
	this->_env["SERVER_PROTOCOL"] = "HTTP/1.1";
	this->_env["SCRIPT_NAME"] = CGIPath;
	this->_env["SCRIPT_FILENAME"] = abs;
	this->_env["SERVER_SOFTWARE"] = srv.getServerName() + "/1.0";
	this->_env["SERVER_NAME"] = srv.getServerName();
	this->_env["PATH"] = "/usr/bin:/bin:/usr/sbin:/sbin";
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

std::string	CGI::execCGI(std::string request, const Server &srv)
{
	pid_t	pid;
	char**	env = this->MapToChar();
	std::string	cgi;
	if (!env)
		return "500 Internal server error\r\n\r\n";
	int		Stdin = dup(STDIN_FILENO);
	int		Stdout = dup(STDOUT_FILENO);
	FILE	*infile = tmpfile();
	FILE	*outfile = tmpfile();
	long	fdIn = fileno(infile);
	long	fdOut = fileno(outfile);
	write(fdIn, request.c_str(), request.size());
	lseek(fdIn, 0, SEEK_SET);
	pid = fork();
	if (pid < 0)
		return NULL;
	if (pid == 0)
	{
		dup2(fdIn, STDIN_FILENO);
		dup2(fdOut, STDOUT_FILENO);
		char * const * nil = NULL;
		execve(this->_env["SCRIPT_FILENAME"].c_str(), nil, env);
		write(STDOUT_FILENO, "500 Internal server error\r\n\r\n", 25);
	}
	else
	{
		int bufSize = srv.getClientBodyBufferSize();
		char* buffer = new char[bufSize];
		waitpid(-1, NULL, 0);
		lseek(fdOut, 0, SEEK_SET);
		int ret = 1;
		while (ret > 0)
		{
			ret = read(fdOut, buffer, bufSize - 1);
			buffer[ret] = '\0';
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
