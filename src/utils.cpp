#include "../include/header.hpp"

std::string HtmlToString(const char* filename)
{
	std::ifstream	file (filename);
	std::string		html;
	std::string		line;

	while (std::getline(file, line))
	{
		for (size_t i = 0; i < line.size(); i++)
			html.push_back(line[i]);
	}
	return html;
}
