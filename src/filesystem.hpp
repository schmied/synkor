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

#ifndef SYNKOR_FILESYSTEM
#define SYNKOR_FILESYSTEM

#include <filesystem>

#include "global.hpp"

#include "../contrib/json11/json11.hpp"

namespace synkor {
	
class filesystem {

private:
	static const stdfs::path check_path_canonical(const stdfs::path, std::string&);

public:
	static const stdfs::path check_file_readable(const stdfs::path, std::string&);
	static const stdfs::path check_dir_writable(const stdfs::path, const bool, std::string&);

	static void load_string(const stdfs::path&, std::string&);
	static json11::Json load_json(const stdfs::path&, std::string&);
	static void save_string(const stdfs::path&, const std::string&);
	static void save_json(const stdfs::path&, const json11::Json);
};

}

#endif