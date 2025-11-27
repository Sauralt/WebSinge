#ifndef HEADER_HPP
# define HEADER_HPP
# include <sys/socket.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <dirent.h>
# include <netinet/in.h>
# include <cstdlib>
# include <iostream>
# include <unistd.h>
# include <poll.h>
# include <vector>
# include <csignal>
# include <fstream>
# include <sstream>
# include <string>
# include <cstring>
# include <cstdlib>
# include <algorithm>
# include <cctype>
# include <fcntl.h>
# include <map>

std::string HtmlToString(const char* filename);

#endif
