#ifndef POLL_HPP
# define POLL_HPP
# include "header.hpp"
# include "Server.hpp"
# include "ServerConfig.hpp"

extern int	gSignalStatus;

class Poll
{
	private:
		std::vector<pollfd>			_pollrequest;
		std::vector<int>			_listeningsock;
		std::map<int, const Server*> _listensrv;
		std::map<int, const Server*>	_clientsrv;
		int		socketfd(const Server& srv);
		void	add_socket(int sockfd);
		int		send_socket(int i, const Server& srv);
	public:
		Poll();
		~Poll();
		Poll(Poll& copy);
		Poll&	operator=(Poll& copy);
		void		pollrequest(std::vector<Server>& servers);
		bool		listeningSock(int fd);
};

#endif
