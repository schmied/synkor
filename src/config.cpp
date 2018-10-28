/*
 * Copyright 2018 Michael Schmiedgen
 *
 * Permission to use, copy, modify, and/or distribute this software for any purpose
 * with or without fee is hereby granted, provided that the above copyright notice
 * and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS.IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
 * THIS SOFTWARE.
 */

#include <filesystem>

#include "../contrib/json11/json11.hpp"
#include "../contrib/sodium/sodium.h"
#include "../contrib/spdlog/spdlog.h"
#include "../contrib/spdlog/sinks/stdout_sinks.h"

#include "global.hpp"

#include "config.hpp"
#include "filesystem.hpp"

const auto log_config = spdlog::stdout_logger_st("config");

const stdfs::path FILE_CONFIG("synkor.conf");
const stdfs::path FILE_KEY_PRIVATE("key_private.txt");
const stdfs::path FILE_KEY_PUBLIC("key_public.txt");

const std::string JSON_KEY_NODENAME("nodename");
const std::string JSON_KEY_LISTEN_PORT("listen_port");

const stdfs::path synkor::config::check_file_config(const stdfs::path dir_base, std::string &error) {
	return filesystem::check_file_readable(stdfs::path(dir_base.string() + "/" + FILE_CONFIG.string()), error);
}

const stdfs::path synkor::config::check_file_config(const std::string dir_base, std::string &error) {
	return check_file_config(stdfs::path(dir_base), error);
}

const stdfs::path synkor::config::check_file_key_private(const stdfs::path dir_base, std::string &error) {
	return filesystem::check_file_readable(stdfs::path(dir_base.string() + "/" + FILE_KEY_PRIVATE.string()), error);
}

const stdfs::path synkor::config::check_file_key_private(const std::string dir_base, std::string &error) {
	return check_file_key_private(stdfs::path(dir_base), error);
}

const stdfs::path synkor::config::check_file_key_public(const stdfs::path path, const std::string nodename, std::string &error) {
	return filesystem::check_file_readable(stdfs::path(path.string() + "/" + nodename + "/" + FILE_KEY_PUBLIC.string()), error);
}

const stdfs::path synkor::config::check_file_key_public(const std::string dir_base, const std::string nodename, std::string &error) {
	return check_file_key_public(stdfs::path(dir_base), nodename, error);
}

const stdfs::path synkor::config::check_dir_base(const stdfs::path dir_base, std::string &error) {
	return filesystem::check_dir_writable(dir_base, false, error);
}

const stdfs::path synkor::config::check_dir_base(const std::string dir_base, std::string &error) {
	return check_dir_base(stdfs::path(dir_base), error);
}

const stdfs::path synkor::config::check_dir_self(const stdfs::path dir_base, const std::string nodename, std::string &error) {
	return filesystem::check_dir_writable(stdfs::path(dir_base.string() + "/" + nodename), true, error);
}

const stdfs::path synkor::config::check_dir_self(const std::string dir_base, const std::string nodename, std::string &error) {
	return check_dir_self(stdfs::path(dir_base), nodename, error);
}

json11::Json synkor::config::load(const stdfs::path dir_base, std::string &error) {
	stdfs::path file_config = check_file_config(dir_base, error);
	if (!error.empty())
		return json11::Json();
	return filesystem::load_json(file_config, error);
}

synkor::config::config(const std::string dir_base, const std::string nodename, std::string &error) {

	_dir_base = check_dir_base(dir_base, error);
	bool is_dir_base = stdfs::is_directory(_dir_base);
	if (!is_dir_base && nodename.empty()) {
		error.assign("No directory " + _dir_base.string());
		return;
	}
	log_config->info("Base directory: {}", _dir_base.string());

	const stdfs::path file_config = check_file_config(_dir_base, error);
	bool is_file_config = stdfs::is_regular_file(file_config);
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
		auto config_json = load(_dir_base, error);
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

	const stdfs::path file_key_private = check_file_key_private(_dir_base, error);
	bool is_file_key_private = stdfs::is_regular_file(file_key_private);
	if ((is_file_config && !is_file_key_private) || (!is_file_config && is_file_key_private)) {
		error.assign("No valid integrity in base directory.");
		return;
	}
	if (is_file_key_private)
		log_config->info("Private key: {}", file_key_private.string());

	const stdfs::path file_key_public = config::check_file_key_public(_dir_base, _nodename, error);
	bool is_file_key_public = stdfs::is_regular_file(file_key_public);
	if ((is_file_config && !is_file_key_public) || (!is_file_config && is_file_key_public)) {
		error.assign("No valid integrity in base directory.");
		return;
	}
	if (is_file_key_public)
		log_config->info("Public key: {}", file_key_public.string());

	const stdfs::path dir_self = check_dir_self(_dir_base, _nodename, error);
	if (!error.empty())
		return;

	if (!is_file_config) {

		filesystem::check_dir_writable(_dir_base, true, error);
		if (!error.empty()) 
			return;

//		json11::Json json_config = json11::Json::object { JSON_KEY_NODENAME, _nodename}; //"{ \"" + JSON_KEY_NODENAME + "\": \"" + _nodename + "\" }";
//		filesystem::save_string(file_config, json_config.dump());
		filesystem::save_string(file_config, "{ \"" + JSON_KEY_NODENAME + "\": \"" + _nodename + "\" }");
		log_config->info("Write config {}", file_config.string());

		unsigned char key_private[crypto_box_SECRETKEYBYTES];
		unsigned char key_public[crypto_box_PUBLICKEYBYTES];
		crypto_box_keypair(key_public, key_private);
		log_config->info("Generate key pair.");

		const int str_key_hex_len = 1000;
		char str_key_hex[str_key_hex_len];
		sodium_bin2hex(str_key_hex, str_key_hex_len, key_private, crypto_box_SECRETKEYBYTES);
		filesystem::save_string(file_key_private, str_key_hex);
		log_config->info("Write private key {}", file_key_private.string());
		sodium_bin2hex(str_key_hex, str_key_hex_len, key_public, crypto_box_PUBLICKEYBYTES);
		filesystem::save_string(file_key_public, str_key_hex);
		log_config->info("Write public key {}", file_key_public.string());
	}

	auto config_json = load(_dir_base, error);
	if (!error.empty())
		return;
	log_config->info("Load config {}", file_config.string());
	_listen_port = config_json[JSON_KEY_LISTEN_PORT].int_value();
	log_config->info("Listen port: {}", _listen_port);
}

const int synkor::config::get_listen_port() const {
	return _listen_port;
}
