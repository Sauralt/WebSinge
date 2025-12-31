#ifndef POLL_HPP
# define POLL_HPP
# include "header.hpp"
# include "Server.hpp"

extern int	gSignalStatus;

class Poll
{
	private:
		std::vector<pollfd>				_pollrequest;
		std::vector<int>				_listeningsock;
		std::map<int, const Server*>	_listensrv;
		std::map<int, const Server*>	_clientsrv;
		std::map<int, std::string>		_buffer;
		std::map<int, pid_t>			_pids;
		std::map<int, int>				_cgifd;
		int		socketfd(const Server& srv);
		void	add_socket(int sockfd);
		int		send_socket(int i, const Server& srv);
	public:
		Poll();
		~Poll();
		Poll(Poll& copy);
		Poll&					operator=(Poll& copy);
		void					pollrequest(std::vector<Server>& servers);
		bool					listeningSock(int fd);
		std::vector<pollfd>&	getPollRequest();
		void					addPollRequest(int& newfd);
		std::string	handleClient(const Server &srv, std::string buffer, int fd);
		void		addPID(int& fd, pid_t& pid);
		void		addfd(int& fd);
};

std::string buildHttpResponse(const std::string &status,
									const std::string &contentType,
									const std::string &body,
									const Location &loc);
std::string errorPage(const std::string &statusCode, const Server &srv,
							const Location &loc);

#endif
