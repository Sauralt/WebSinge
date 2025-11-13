#ifndef LOCATION_HPP
#define LOCATION_HPP

#include "header.hpp"

class Location
{
	private:
		std::string _path;
		std::string _index_file;
		bool _allow_upload;

	public:
		Location();
		~Location();
		Location(const Location& copy);
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
