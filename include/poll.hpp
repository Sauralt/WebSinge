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
		void					addPollRequest(long& newfd);
		std::string	handleClient(const Server &srv, std::string buffer);
};

#endif
