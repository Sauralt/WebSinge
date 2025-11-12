#include "../include/header.hpp"
#include "../include/poll.hpp"

int	main(int argc, char** argv)
{
	if (argc > 2)
	{
		std::cerr << "Too many files.\n";
		return 1;
	}
	argv = NULL;
	polling	pollsocket;
	pollsocket.pollrequest();
}
