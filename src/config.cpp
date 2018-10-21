
#include <boost/log/trivial.hpp>
#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>

#include <sodium.h>

#include "../contrib/json11/json11.hpp"

#include "config.hpp"

namespace fs = boost::filesystem;

const char *FILE_CONFIG = "synkor.conf";
const char *FILE_KEY_PRIVATE = "key_private";
const char *FILE_KEY_PUBLIC = "key_public";

const char *KEY_NAME = "name";
const char *KEY_LISTEN_PORT = "listen_port";

config::config(char *directory, std::string &error) {

	const fs::path dir_base_nonc(directory);
	if (!fs::is_directory(dir_base_nonc)) {
		error.assign("No directory: ");
		error.append(dir_base_nonc.string());
		return;
	}
	if ((fs::status(dir_base_nonc).permissions() & (fs::perms::owner_read | fs::perms::owner_write)) == 0) {
		error.assign("Not a readable/writable directory: ");
		error.append(dir_base_nonc.string());
		return;
	}

	boost::system::error_code ec;

	dir_base = fs::canonical(dir_base_nonc, ec);
	if (ec.value() != boost::system::errc::success) {
		error.assign("canonical() error code " + ec.value());
		return;
	}
	BOOST_LOG_TRIVIAL(info) << "Base directory: " << dir_base.string();

	const fs::path file_nonc(dir_base.string() + "/" + FILE_CONFIG);
	if (!fs::is_regular_file(file_nonc)) {
		error.assign("No config file: ");
		error.append(file_nonc.string());
		return;
	}
	file = fs::canonical(file_nonc, ec);
	if (ec.value() != boost::system::errc::success) {
		error.assign("canonical() error code " + ec.value());
		return;
	}
	BOOST_LOG_TRIVIAL(info) << "Config file: " << file.string();

	std::string config_string;
	fs::load_string_file(file, config_string);
	std::string config_error;
	json11::Json config_json = json11::Json::parse(config_string, config_error);
	if (!config_error.empty()) {
		error.assign("Error in config file: ");
		error.append(config_error);
		return;
	}
	name = config_json[KEY_NAME].string_value();

	const fs::path dir_self_nonc(dir_base.string() + "/" + name);
	if (!fs::exists(dir_self_nonc)) {
		if (!fs::create_directory(dir_self_nonc)) {
			error.assign("Cannot create directory: ");
			error.append(dir_self_nonc.string());
			return;
		}
		BOOST_LOG_TRIVIAL(info) << "Directory created: " << dir_self_nonc.string();
	}
	dir_self = fs::canonical(dir_self_nonc, ec);
	if (ec.value() != boost::system::errc::success) {
		error.assign("canonical() error code " + ec.value());
		error.append(" on directory " + dir_self.string());
		return;
	}
	BOOST_LOG_TRIVIAL(info) << "Self Directory: " << dir_self.string();

	const fs::path file_key_private(dir_base.string() + "/" + FILE_KEY_PRIVATE);
	if (!fs::exists(file_key_private)) {
		BOOST_LOG_TRIVIAL(info) << "No private key found, generate key pair.";
		unsigned char key_public[crypto_box_PUBLICKEYBYTES];
		unsigned char key_private[crypto_box_SECRETKEYBYTES];
		crypto_box_keypair(key_public, key_private);
		std::ofstream ofs_private;
		ofs_private.open(file_key_private.c_str(), std::ios::out | std::ios::binary);
		ofs_private.write((const char*) key_private, crypto_box_SECRETKEYBYTES);
		ofs_private.close();
		fs::path file_key_public(dir_self.string() + "/" + FILE_KEY_PUBLIC);
		if (fs::exists(file_key_public)) {
			if (!fs::remove(file_key_public)) {
				error.assign("Cannot delete public key " + file_key_public.string());
				return;
			}
		}
		std::ofstream ofs_public;
		ofs_public.open(file_key_public.c_str(), std::ios::out | std::ios::binary);
		ofs_public.write((const char*) key_public, crypto_box_PUBLICKEYBYTES);
		ofs_public.close();
	}

	listen_port = config_json[KEY_LISTEN_PORT].int_value();

	BOOST_LOG_TRIVIAL(info) << KEY_NAME << ": " << name;
	BOOST_LOG_TRIVIAL(info) << KEY_LISTEN_PORT << ": " << listen_port;
}
