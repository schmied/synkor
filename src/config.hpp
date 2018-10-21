#pragma once

#include <boost/filesystem.hpp>

class config {

private:
	boost::filesystem::path file;
	boost::filesystem::path dir_base;
	boost::filesystem::path dir_self;
	std::string name;
	int listen_port;

public:
	config(char*, std::string&);

	const int get_listen_port() const;
};
