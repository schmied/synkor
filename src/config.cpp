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

#include "config.hpp"
#include "global.hpp"
#include "log.hpp"
#include "filesystem.hpp"

static const auto log_config = spdlog::stdout_logger_st("config");

static const stdfs::path FILE_CONFIG("synkor.conf");
static const stdfs::path FILE_KEY_PRIVATE("key_private");
static const stdfs::path FILE_KEY_PUBLIC("key_public.txt");

static const std::string JSON_KEY_PEERNAME("peername");
static const std::string JSON_KEY_LISTEN_PORT("listen_port");

const stdfs::path synkor::config::path_dir_self(const stdfs::path &dir_base, const std::string &nodename) {
	return stdfs::path { dir_base.string() + "/" + nodename }.lexically_normal();
}

const stdfs::path synkor::config::path_file_config(const stdfs::path &dir_base) {
	return stdfs::path { dir_base.string() + "/" + FILE_CONFIG.string() }.lexically_normal();
}

const stdfs::path synkor::config::path_file_key_private(const stdfs::path &dir_base) {
	return stdfs::path { dir_base.string() + "/" + FILE_KEY_PRIVATE.string() }.lexically_normal();
}

const stdfs::path synkor::config::path_file_key_public(const stdfs::path &dir_base, const std::string &nodename) {
	return stdfs::path { dir_base.string() + "/" + nodename + "/" + FILE_KEY_PUBLIC.string() }.lexically_normal();
}

synkor::config::config(const stdfs::path &dir_base, const std::string &peername) {

	if (!filesystem::is_dir_writable(dir_base, false))
		THROW_EXCEPTION("no writable directory: " + dir_base.string());
	_dir_base = filesystem::canonical(dir_base).lexically_normal();
	log_config->info("base directory: {}", _dir_base.string());

	const stdfs::path file_config = path_file_config(_dir_base);
	bool is_file_config = filesystem::is_file_readable(file_config);
	if (!is_file_config && peername.empty())
			THROW_EXCEPTION("no peer name defined, cannot initialize directory: " + _dir_base.string());
	if (is_file_config && !peername.empty())
			THROW_EXCEPTION("directory already initialized: " + _dir_base.string());
	if (is_file_config) {
		log_config->info("config file: {}", file_config.string());
		auto config_json = filesystem::load_json(path_file_config(_dir_base));
		_peername = config_json[JSON_KEY_PEERNAME].string_value();
		if (_peername.empty())
			THROW_EXCEPTION("no peer name defined in config file: " + file_config.string());
	} else {
		_peername = peername;
	}
	log_config->info("peer name: {}", _peername);

	const stdfs::path dir_self = path_dir_self(_dir_base, _peername);
	if (!filesystem::is_dir_writable(dir_self, true))
		THROW_EXCEPTION("no writable self directory: " + _dir_base.string());

	const stdfs::path file_key_private = path_file_key_private(_dir_base);
	bool is_file_key_private = filesystem::is_file_readable(file_key_private);
	if ((is_file_config && !is_file_key_private) || (!is_file_config && is_file_key_private))
		THROW_EXCEPTION("no valid integrity in base directory: " + _dir_base.string());
	if (is_file_key_private)
		log_config->info("private key: {}", file_key_private.string());

	const stdfs::path file_key_public = path_file_key_public(_dir_base, _peername);
	bool is_file_key_public = filesystem::is_file_readable(file_key_public);
	if ((is_file_config && !is_file_key_public) || (!is_file_config && is_file_key_public))
		THROW_EXCEPTION("no valid integrity in base directory: " + _dir_base.string());
	if (is_file_key_public)
		log_config->info("public key: {}", file_key_public.string());

	if (!is_file_config) {
		filesystem::save_string(file_config, "{ \"" + JSON_KEY_PEERNAME + "\": \"" + _peername + "\" }");
		log_config->info("write config file: {}", file_config.string());

		unsigned char key_private[crypto_box_SECRETKEYBYTES];
		unsigned char key_public[crypto_box_PUBLICKEYBYTES];
		crypto_box_keypair(key_public, key_private);
		log_config->info("generate key pair");

		const int str_key_hex_len = 1000;
		char str_key_hex[str_key_hex_len];
		sodium_bin2hex(str_key_hex, str_key_hex_len, key_private, crypto_box_SECRETKEYBYTES);
		filesystem::save_string(file_key_private, str_key_hex);
		log_config->info("write private key file: {}", file_key_private.string());
		sodium_bin2hex(str_key_hex, str_key_hex_len, key_public, crypto_box_PUBLICKEYBYTES);
		filesystem::save_string(file_key_public, str_key_hex);
		log_config->info("write public key file: {}", file_key_public.string());
	}

	auto config_json = filesystem::load_json(path_file_config(_dir_base));
	log_config->info("load config file: {}", file_config.string());
	_listen_port = config_json[JSON_KEY_LISTEN_PORT].int_value();
	log_config->info("listen port: {}", _listen_port);
}

int synkor::config::get_listen_port() const {
	return _listen_port;
}
