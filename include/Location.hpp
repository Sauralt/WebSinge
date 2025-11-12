#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <string>
#include <iostream>

class Location {
private:
    std::string _path;
    std::string _index_file;
    bool _allow_upload;

public:
    Location() : _allow_upload(false) {}

    const std::string &getPath() const { return _path; }
    const std::string &getIndexFile() const { return _index_file; }
    bool getAllowUpload() const { return _allow_upload; }

    void setPath(const std::string &path) { _path = path; }
    void setIndexFile(const std::string &file) { _index_file = file; }
    void setAllowUpload(bool allow) { _allow_upload = allow; }

    void print() const {
        std::cout << "  Location: path=" << _path
                  << ", index=" << _index_file
                  << ", upload=" << (_allow_upload ? "true" : "false") << std::endl;
    }
};

#endif
