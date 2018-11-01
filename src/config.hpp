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

#pragma once

#ifndef SYNKOR_CONIG
#define SYNKOR_CONIG

#include <filesystem>

#include "global.hpp"

namespace synkor {

class config {

private:
	stdfs::path _dir_base;
	std::string _peername;
	int _listen_port;

	static const stdfs::path path_dir_self(const stdfs::path&, const std::string&);
	static const stdfs::path path_file_config(const stdfs::path&);
	static const stdfs::path path_file_key_private(const stdfs::path&);
	static const stdfs::path path_file_key_public(const stdfs::path&, const std::string&);

public:
	config(const stdfs::path&, const std::string&);

	int get_listen_port() const;

};

}

#endif
