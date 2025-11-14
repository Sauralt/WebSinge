#include "CGI.hpp"

void	exec_CGI(std::string cgi)
{
	pid_t	pid;
	int		fd[2];
	if (pipe(fd) < 0)
		return ;
	pid = fork();
	if (pid < 0)
		return ;
	if (pid == 0)
	{
		dup2(1, fd[0]);
		execve(cgi)
	}
}