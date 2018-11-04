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

#include "../contrib/sodium/sodium.h"
#include "../contrib/spdlog/spdlog.h"
#include "../contrib/spdlog/sinks/stdout_sinks.h"

#include "base.hpp"
#include "filesystem.hpp"
#include "log.hpp"

/*
 * ******************************************************** private
 */

static const auto logger = spdlog::stdout_logger_st(stdfs::path(__FILE__).stem().string());

static const stdfs::path FILE_KEY_PRIVATE {"key_private"};
static const stdfs::path FILE_KEY_PUBLIC {"key_public.txt"};

const stdfs::path synkor::base::path_dir_self(const stdfs::path &dir_base, const std::string &peername) {
	return stdfs::path {dir_base.string() + "/" + peername}.lexically_normal();
}

const stdfs::path synkor::base::path_file_key_private(const stdfs::path &dir_base) {
	return stdfs::path {dir_base.string() + "/" + FILE_KEY_PRIVATE.string()}.lexically_normal();
}

const stdfs::path synkor::base::path_file_key_public(const stdfs::path &dir_base, const std::string &peername) {
	return stdfs::path {dir_base.string() + "/" + peername + "/" + FILE_KEY_PUBLIC.string()}.lexically_normal();
}

/*
 * ******************************************************** public
 */

synkor::base::base(const stdfs::path &dir_base, const std::string &peername) {

	if (!filesystem::is_dir_writable(dir_base, false))
		THROW_EXCEPTION("no writable directory: " + dir_base.string());
	_dir_base = filesystem::canonical(dir_base).lexically_normal();
	logger->info("base directory: {}", _dir_base.string());

	if (peername.empty())
		THROW_EXCEPTION("no peer name defined: " + _dir_base.string());
	_peername = peername;
	logger->info("peer name: {}", _peername);

	const auto dir_self = path_dir_self(_dir_base, _peername);
	if (!filesystem::is_dir_writable(dir_self, true))
		THROW_EXCEPTION("no writable self directory: " + _dir_base.string());

	const auto file_key_private = path_file_key_private(_dir_base);
	const auto is_file_key_private = filesystem::is_file_readable(file_key_private);
	if (is_file_key_private)
		logger->info("private key: {}", file_key_private.string());
	else
		logger->info("no private key: {}", file_key_private.string());

	const auto file_key_public = path_file_key_public(_dir_base, _peername);
	const auto is_file_key_public = filesystem::is_file_readable(file_key_public);
	if (is_file_key_public)
		logger->info("public key: {}", file_key_public.string());
	else
		logger->info("no public key: {}", file_key_public.string());
	if ((is_file_key_private && !is_file_key_public) || (!is_file_key_private && is_file_key_public))
		THROW_EXCEPTION("no valid integrity in base directory: " + _dir_base.string());

	if (!is_file_key_private && !is_file_key_public) {
		unsigned char key_private[crypto_box_SECRETKEYBYTES];
		unsigned char key_public[crypto_box_PUBLICKEYBYTES];
		crypto_box_keypair(key_public, key_private);
		logger->info("generate key pair");
		const int str_key_hex_len = 1000;
		char str_key_hex[str_key_hex_len];
		sodium_bin2hex(str_key_hex, str_key_hex_len, key_private, crypto_box_SECRETKEYBYTES);
		filesystem::save_string(file_key_private, str_key_hex);
		logger->info("write private key file: {}", file_key_private.string());
		sodium_bin2hex(str_key_hex, str_key_hex_len, key_public, crypto_box_PUBLICKEYBYTES);
		filesystem::save_string(file_key_public, str_key_hex);
		logger->info("write public key file: {}", file_key_public.string());
	}
}

const stdfs::path synkor::base::dir_base() const {
	return _dir_base;
}

const std::string synkor::base::peername() const {
	return _peername;
}
