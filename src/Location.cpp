#include "../include/Location.hpp"

Location::Location() : _allow_upload(false), _allowGet(false), _allowPost(false), _allowDelete(false) {}

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

void Location::setAllowMethods(const std::vector<std::string> &methods)
{
    _allowMethods = methods;
    _allowGet = false;
    _allowPost = false;
    _allowDelete = false;

    for (size_t i = 0; i < methods.size(); i++)
    {
        if (methods[i] == "GET") _allowGet = true;
        else if (methods[i] == "POST") _allowPost = true;
        else if (methods[i] == "DELETE") _allowDelete = true;
    }
}

const std::vector<std::string> &Location::getAllowMethods() const
{
    return _allowMethods;
}

bool Location::isMethodAllowed(const std::string &m) const
{
    if (m == "GET") return _allowGet;
    if (m == "POST") return _allowPost;
    if (m == "DELETE") return _allowDelete;
    return false;
}

