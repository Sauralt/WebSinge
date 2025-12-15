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
	this->_env["SCRIPT_FILENAME"] = abs;
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
	this->_env["SCRIPT_NAME"] = CGIPath;
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
		envp[i] = std::strcpy(envp[i], (const char*)entry.c_str());
		++i;
	}
	envp[i] = NULL;
	return envp;
}

std::string	CGI::ScriptFileName(std::string request)
{
	std::string	res;
	int			metLen;

	if (request.find("GET") != std::string::npos)
		metLen = 4;
	else
		metLen = 5;
	for (std::string::iterator it = request.begin() + metLen; (*it) != 'y' && (*it - 1) != 'p' && (*it - 2) != '.'; it++)
		res.push_back((*it));
	res.push_back('y');
	return res;
}

int	CGI::execCGI(std::string request, Poll& _poll)
{
	pid_t	pid;
	char**	env = this->MapToChar();
	if (!env)
		return -1;
	int		inFd[2];
	int		outFd[2];
	if (pipe(inFd) < 0 || pipe(outFd) < 0)
		return -1;
	pid = fork();
	if (pid < 0)
		return -1;
	if (pid == 0)
	{
		dup2(inFd[0], STDIN_FILENO);
		dup2(outFd[1], STDOUT_FILENO);
		for (size_t i = 0; i < _poll.getPollRequest().size(); i++)
			close(_poll.getPollRequest()[i].fd);
		close(inFd[0]);
		close(outFd[1]);
		close(inFd[1]);
		close(outFd[0]);
		char *argv[2];
		argv[0] = (char*)this->_env["SCRIPT_FILENAME"].c_str();
		argv[1] = NULL;
		execve(argv[0], argv, env);
		write(STDOUT_FILENO, "500 Internal server error\r\n\r\n", 29);
		exit(1);
	}
	write(inFd[1], request.c_str(), request.size());
	close(inFd[1]);
	fcntl(outFd[0], F_SETFL, O_NONBLOCK);
	_poll.addPollRequest(outFd[0]);
	_poll.addPID(outFd[0], pid);
	close(inFd[0]);
	close(outFd[1]);
	for (size_t i = 0; env[i]; i++)
		delete [] env[i];
	delete [] env;
	return (outFd[0]);
}
