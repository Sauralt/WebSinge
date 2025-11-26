#ifndef LOCATION_HPP
#define LOCATION_HPP

#include "header.hpp"

class Location
{
	private:
		std::string	_path;
		std::vector<std::string> _index_file;
		bool		_allow_upload;
		std::vector<std::string> _allowMethods;
		bool _allowGet;
    	bool _allowPost;
    	bool _allowDelete;
		bool _autoindex;
		
	public:
		Location();
		~Location();
		Location(const Location& copy);
		void setAllowMethods(const std::vector<std::string> &methods);
		const std::vector<std::string> &getAllowMethods() const;
		bool isMethodAllowed(const std::string &m) const;
		bool allowGet() const;
		bool allowPost() const;
		bool allowDelete() const;
		Location&	operator=(const Location& copy);
		const std::string &getPath() const;
		void setIndexFiles(const std::vector<std::string> &indexes);
		 const std::vector<std::string>& getIndexFiles() const;
		void setAutoIndex(bool _autoindex);
    	bool getAutoIndex() const;
		bool getAllowUpload() const;
		void setPath(const std::string &path);
		void setAllowUpload(bool allow);
		void print() const;
};

#endif

