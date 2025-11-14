#ifndef CGI_HPP
# define CGI_HPP
# include "header.hpp"

class Contact {
public:
    std::map<std::string, std::string> parse(const std::string &query);
    std::string urlDecode(const std::string &str);
    void saveToFile(const std::map<std::string, std::string> &data, const std::string &path);
    std::string generateHtml(const std::map<std::string, std::string> &data);
};


#endif
