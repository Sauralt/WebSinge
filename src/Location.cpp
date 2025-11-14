#include "../include/Location.hpp"

Location::Location() : _allow_upload(false)
{}

Location::~Location()
{}

Location::Location(const Location& copy)
{
	*this = copy;
}

Location &Location::operator=(const Location& copy)
{
	this->_allow_upload = copy._allow_upload;
	this->_index_file = copy._index_file;
	this->_path = copy._path;
	return *this;
}

const std::string &Location::getPath() const
{ return _path; }

const std::string &Location::getIndexFile() const
{ return _index_file; }

bool Location::getAllowUpload() const
{ return _allow_upload; }

void Location::setPath(const std::string &path)
{ _path = path; }

void Location::setIndexFile(const std::string &file)
{ _index_file = file; }

void Location::setAllowUpload(bool allow)
{ _allow_upload = allow; }

void Location::print() const
{
	std::cout << "  Location: path=" << _path
				<< ", index=" << _index_file
				<< ", upload=" << (_allow_upload ? "true" : "false") << std::endl;
}


