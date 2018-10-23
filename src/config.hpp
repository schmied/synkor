#pragma once

#ifndef SYNKOR_CONIG
#define SYNKOR_CONIG

#include <boost/filesystem.hpp>

#include "../contrib/json11/json11.hpp"

namespace fs = boost::filesystem;

class config {

private:
	fs::path _dir_base;
	std::string _nodename;
	int _listen_port;

	static json11::Json parse(const fs::path, std::string&);

	static const fs::path check_path_canonical(const fs::path, std::string&);
	static const fs::path check_file_readable(const fs::path, std::string&);
	static const fs::path check_dir_writable(const fs::path, const bool, std::string&);

public:
    config(const std::string, const std::string, std::string&);

	static const fs::path check_dir_base(const fs::path, std::string&);
	static const fs::path check_dir_base(const std::string, std::string&);
	static const fs::path check_dir_self(const fs::path, const std::string, std::string&);
	static const fs::path check_dir_self(const std::string, const std::string, std::string&);
	static const fs::path check_file_config(const fs::path, std::string&);
	static const fs::path check_file_config(const std::string, std::string&);
	static const fs::path check_file_key_private(const fs::path, std::string&);
	static const fs::path check_file_key_private(const std::string, std::string&);
	static const fs::path check_file_key_public(const fs::path, const std::string, std::string&);
	static const fs::path check_file_key_public(const std::string, const std::string, std::string&);

	const int get_listen_port() const;
};

#endif
