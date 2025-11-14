#ifndef POLL_HPP
# define POLL_HPP
# include "header.hpp"
# include "Server.hpp"
# include "ServerConfig.hpp"

extern int	gSignalStatus;

class polling
{
	private:
		std::vector<pollfd>	_pollrequest;
		int		socketfd(const Server& srv);
		void	add_socket(int sockfd);
		int		send_socket(int i, const Server& srv);
	public:
		polling();
		~polling();
		polling(polling& copy);
		polling&	operator=(polling& copy);
		void		pollrequest(const Server& srv);
};

#endif
