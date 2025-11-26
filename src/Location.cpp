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
    this->_allowMethods = copy._allowMethods;
    this->_allowGet = copy._allowGet;
    this->_allowPost = copy._allowPost;
    this->_allowDelete = copy._allowDelete;
    this->_autoindex = copy._autoindex;
	return *this;
}

const std::string &Location::getPath() const
{ return _path; }

bool Location::getAllowUpload() const
{ return _allow_upload; }

void Location::setPath(const std::string &path)
{ _path = path; }

void Location::setAllowUpload(bool allow)
{ _allow_upload = allow; }

void Location::print() const
{
	std::cout << "Location: path=" << _path << ", upload=" << (_allow_upload ? "true" : "false") << std::endl
                << ", index=[";
                for (size_t i = 0; i < _index_file.size(); i++)
                {
                    std::cout << _index_file[i];
                    if (i + 1 < _index_file.size())
                        std::cout << ", ";
                }
                std::cout << "]" << std::endl;
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

void Location::setAutoIndex(bool value) 
{ 
    _autoindex = value; 
}

bool Location::getAutoIndex() const 
{ 
    return _autoindex; 
}

void Location::setIndexFiles(const std::vector<std::string> &indexes)
{
        _index_file = indexes;
}

const std::vector<std::string>& Location::getIndexFiles() const
{
    return _index_file;
}
