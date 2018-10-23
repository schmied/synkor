
#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>

#include <sodium.h>

#include "../contrib/spdlog/spdlog.h"
#include "../contrib/spdlog/sinks/stdout_sinks.h"
#include "../contrib/json11/json11.hpp"

#include "config.hpp"

namespace fs = boost::filesystem;

const auto log_config = spdlog::stdout_logger_st("config");

const fs::path FILE_CONFIG("synkor.conf");
const fs::path FILE_KEY_PRIVATE("key_private");
const fs::path FILE_KEY_PUBLIC("key_public");

const std::string JSON_KEY_NODENAME("nodename");
const std::string JSON_KEY_LISTEN_PORT("listen_port");

const fs::path config::check_path_canonical(const fs::path path, std::string &error) {
	error.clear();
	boost::system::error_code ec;
	fs::path pathc = fs::canonical(path, ec);
	if (ec.value() != boost::system::errc::success) {
		error.assign("canonical() error code " + ec.value());
		return path;
	}
	return pathc;
}

const fs::path config::check_file_readable(const fs::path path, std::string &error) {
	error.clear();
	if (!fs::is_regular_file(path)) {
		error.assign("No file " + path.string());
		return path;
	}
	if ((fs::status(path).permissions() & fs::perms::owner_read) == 0) {
		error.assign("Not a readable file: " + path.string());
		return path;
	}
	return check_path_canonical(path, error);
}

const fs::path config::check_dir_writable(const fs::path path, const bool create, std::string &error) {
	error.clear();
	if (!fs::exists(path)) {
		if (!create) {
			error.assign("No directory " + path.string());
			return path;
		}
		if (!fs::create_directories(path)) {
			error.assign("Cannot create directory " + path.string());
			return path;
		}
		log_config->info("Create directory {}", path.string());
	}
	if (!fs::is_directory(path)) {
		error.assign("No directory " + path.string());
		return path;
	}
	if ((fs::status(path).permissions() & (fs::perms::owner_read | fs::perms::owner_write)) == 0) {
		error.assign("Not a readable/writable directory: " + path.string());
		return path;
	}
	return check_path_canonical(path, error);
}

const fs::path config::check_file_config(const fs::path dir_base, std::string &error) {
	return check_file_readable(fs::path(dir_base.string() + "/" + FILE_CONFIG.string()), error);
}

const fs::path config::check_file_config(const std::string dir_base, std::string &error) {
	return check_file_config(fs::path(dir_base), error);
}

const fs::path config::check_file_key_private(const fs::path dir_base, std::string &error) {
	return check_file_readable(fs::path(dir_base.string() + "/" + FILE_KEY_PRIVATE.string()), error);
}

const fs::path config::check_file_key_private(const std::string dir_base, std::string &error) {
	return check_file_key_private(fs::path(dir_base), error);
}

const fs::path config::check_file_key_public(const fs::path path, const std::string nodename, std::string &error) {
	return check_file_readable(fs::path(path.string() + "/" + nodename + "/" + FILE_KEY_PUBLIC.string()), error);
}

const fs::path config::check_file_key_public(const std::string dir_base, const std::string nodename, std::string &error) {
	return check_file_key_public(fs::path(dir_base), nodename, error);
}

const fs::path config::check_dir_base(const fs::path dir_base, std::string &error) {
	return check_dir_writable(dir_base, false, error);
}

const fs::path config::check_dir_base(const std::string dir_base, std::string &error) {
	return check_dir_base(fs::path(dir_base), error);
}

const fs::path config::check_dir_self(const fs::path dir_base, const std::string nodename, std::string &error) {
	return check_dir_writable(fs::path(dir_base.string() + "/" + nodename), true, error);
}

const fs::path config::check_dir_self(const std::string dir_base, const std::string nodename, std::string &error) {
	return check_dir_self(fs::path(dir_base), nodename, error);
}

json11::Json config::parse(const fs::path dir_base, std::string &error) {
	error.clear();
	fs::path file_config = check_file_config(dir_base, error);
	if (!error.empty())
		return json11::Json();
	std::string config;
	fs::load_string_file(file_config, config);
	return json11::Json::parse(config, error);
}

config::config(const std::string dir_base, const std::string nodename, std::string &error) {

	_dir_base = check_dir_base(dir_base, error);
	bool is_dir_base = fs::is_directory(_dir_base);
	if (!is_dir_base && nodename.empty()) {
		error.assign("No directory " + _dir_base.string());
		return;
	}
	log_config->info("Base directory: {}", _dir_base.string());

	const fs::path file_config = check_file_config(_dir_base, error);
	bool is_file_config = fs::is_regular_file(file_config);
	if (!is_file_config && nodename.empty()) {
		error.assign("No config file " + file_config.string());
		return;
	}
	if (is_file_config) {
		if (!nodename.empty()) {
			error.assign("Cannot initialize node '" + nodename + "', config file already exists in base directory " + _dir_base.string());
			return;
		}
		log_config->info("Config file: {}", file_config.string());
		auto config_json = parse(_dir_base, error);
		if (!error.empty())
			return;
		_nodename = config_json[JSON_KEY_NODENAME].string_value();
		if (_nodename.empty()) {
			error.assign("No json key '" + JSON_KEY_NODENAME + "' defined in config file.");
			return;
		}
	} else {
		_nodename = nodename;
	}
	log_config->info("Nodename: {}", _nodename);

	const fs::path file_key_private = check_file_key_private(_dir_base, error);
	bool is_file_key_private = fs::is_regular_file(file_key_private);
	if ((is_file_config && !is_file_key_private) || (!is_file_config && is_file_key_private)) {
		error.assign("No valid integrity in base directory.");
		return;
	}
	if (is_file_key_private)
		log_config->info("Private key: {}", file_key_private.string());

	const fs::path file_key_public = config::check_file_key_public(dir_base, _nodename, error);
	bool is_file_key_public = fs::is_regular_file(file_key_public);
	if ((is_file_config && !is_file_key_public) || (!is_file_config && is_file_key_public)) {
		error.assign("No valid integrity in base directory.");
		return;
	}
	if (is_file_key_public)
		log_config->info("Public key: {}", file_key_public.string());

	const fs::path dir_self = check_dir_self(_dir_base, _nodename, error);
	if (!error.empty())
		return;

	if (!is_file_config) {

		check_dir_writable(dir_base, true, error);
		if (!error.empty()) 
			return;

		unsigned char key_public[crypto_box_PUBLICKEYBYTES];
		unsigned char key_private[crypto_box_SECRETKEYBYTES];
		crypto_box_keypair(key_public, key_private);

		std::ofstream ofs_config;
		ofs_config.open(file_config.c_str(), std::ios::out | std::ios::binary);
		ofs_config << std::string("{ \"" + JSON_KEY_NODENAME + "\": \"" + _nodename + "\" }");
		ofs_config.close();
		log_config->info("Generate config {}", file_config.string());

		std::ofstream ofs_key_private;
		ofs_key_private.open(file_key_private.c_str(), std::ios::out | std::ios::binary);
		ofs_key_private.write((const char*) key_private, crypto_box_SECRETKEYBYTES);
		ofs_key_private.close();
		log_config->info("Generate private key {}", file_key_private.string());

		std::ofstream ofs_key_public;
		ofs_key_public.open(file_key_public.c_str(), std::ios::out | std::ios::binary);
		ofs_key_public.write((const char*) key_public, crypto_box_PUBLICKEYBYTES);
		ofs_key_public.close();
		log_config->info("Generate public key {}", file_key_public.string());
	}

	auto config_json = parse(_dir_base, error);
	_listen_port = config_json[JSON_KEY_LISTEN_PORT].int_value();
	log_config->info("Listen port: {}", _listen_port);
}

const int config::get_listen_port() const {
	return _listen_port;
}
