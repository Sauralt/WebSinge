#ifndef LOCATION_HPP
#define LOCATION_HPP

#include "header.hpp"

class Location
{
	private:
		std::string	_path;
		std::string	_index_file;
		bool		_allow_upload;
		std::vector<std::string> _allowMethods;
		bool _allowGet;
    	bool _allowPost;
    	bool _allowDelete;
		
	public:
		Location();
		~Location();
		Location(const Location& copy);
    	void setAllowMethods(const std::vector<std::string> &methods);
    	const std::vector<std::string> &getAllowMethods() const;
    	bool isMethodAllowed(const std::string &m) const;
		bool allowGet() const { return _allowGet; }
    	bool allowPost() const { return _allowPost; }
    	bool allowDelete() const { return _allowDelete; }
		Location&	operator=(const Location& copy);
		const std::string &getPath() const;
		const std::string &getIndexFile() const;
		bool getAllowUpload() const;
		void setPath(const std::string &path);
		void setIndexFile(const std::string &file);
		void setAllowUpload(bool allow);
		void print() const;
};

#endif

