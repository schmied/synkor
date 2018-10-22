#pragma once

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

class config {

private:
    fs::path file;
    fs::path dir_base;
    fs::path dir_self;
	std::string name;
	int listen_port;

public:
    config(std::string&);
    config(fs::path, std::string&);

	const int get_listen_port() const;
};
