#ifndef POLL_HPP
# define POLL_HPP
# include "header.hpp"

class polling
{
	private:
		std::vector<pollfd>				_pollrequest;
		int		socketfd();
		void	add_socket(int sockfd);
		int		send_socket(int i);
	public:
		polling();
		~polling();
		polling(polling& copy);
		polling&	operator=(polling& copy);
		void		pollrequest();
};

#endif
