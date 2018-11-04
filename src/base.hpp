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

#ifndef SYNKOR_BASE
#define SYNKOR_BASE

#include <filesystem>

#include "global.hpp"

namespace synkor {

class base {

private:
	stdfs::path _dir_base;
	std::string _peername;

	static const stdfs::path path_dir_self(const stdfs::path&, const std::string&);
	static const stdfs::path path_file_key_private(const stdfs::path&);
	static const stdfs::path path_file_key_public(const stdfs::path&, const std::string&);

public:
	base(const stdfs::path&, const std::string&);

	const stdfs::path dir_base() const;
	const std::string peername() const;

	static const std::string valid_peername(const std::string&);
};

}

#endif
