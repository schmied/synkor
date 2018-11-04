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
#include <fstream>
#include <sstream>

#include "../contrib/asio/asio.hpp"
#include "../contrib/json11/json11.hpp"
#include "../contrib/spdlog/spdlog.h"
#include "../contrib/spdlog/sinks/stdout_sinks.h"

#include "filesystem.hpp"
#include "global.hpp"
#include "log.hpp"

/*
 * ******************************************************** private
 */

static const auto logger = spdlog::stdout_logger_st(stdfs::path(__FILE__).stem().string());

/*
 * ******************************************************** public
 */

const stdfs::path synkor::filesystem::canonical(const stdfs::path &path) {
	try {
		return stdfs::canonical(path).lexically_normal();
	} catch (...) {
		THROW_EXCEPTION_NESTED("path: " + path.string());
	}
}

bool synkor::filesystem::is_file_readable(const stdfs::path &path) {
	try {
		if (!stdfs::is_regular_file(path))
			return false;
		if ((stdfs::status(path).permissions() & stdfs::perms::owner_read) == stdfs::perms::none)
			return false;
		return true;
	} catch (...) {
		THROW_EXCEPTION_NESTED("path: " + path.string());
	}
}

bool synkor::filesystem::is_dir_writable(const stdfs::path &path, const bool create) {
	try {
		if (!stdfs::exists(path)) {
			if (!create)
				return false;
			if (!stdfs::create_directories(path))
				THROW_EXCEPTION("cannot create directory: " + path.string());
			logger->info("create directory {}", path.string());
		}
		if (!stdfs::is_directory(path))
			THROW_EXCEPTION("no directory: " + path.string());
		if ((stdfs::status(path).permissions() & (stdfs::perms::owner_read | stdfs::perms::owner_write)) == stdfs::perms::none)
			return false;
		return true;
	} catch (...) {
		THROW_EXCEPTION_NESTED("path: " + path.string());
	}
}

std::string synkor::filesystem::load_string(const stdfs::path &path) {
	try {
		std::string s;
		if (!is_file_readable(path))
			THROW_EXCEPTION("no file: " + path.string());
		std::ifstream ifs {path, std::ios_base::binary | std::ios_base::in};
		asio::streambuf sb;
		const size_t buf_len = 1024;
		while (ifs) {
			auto in = sb.prepare(buf_len);
			ifs.read((char*)in.data(), buf_len);
			sb.commit((size_t) ifs.gcount());
			const auto out = sb.data();
			s.append((const char*) out.data());
			sb.consume(out.size());
		}
		if (ifs.is_open())
			ifs.close();
		return s;
	} catch (...) {
		THROW_EXCEPTION_NESTED("path: " + path.string());
	}
}

json11::Json synkor::filesystem::load_json(const stdfs::path& path) {
	try {
		const auto s = load_string(path);
		std::string error;
		const auto json = json11::Json::parse(s, error);
		if (!error.empty())
			THROW_EXCEPTION("parse error: " + error);
		return json;
	} catch (...) {
		THROW_EXCEPTION_NESTED("path: " + path.string());
	}
}

void synkor::filesystem::save_string(const stdfs::path &path, const std::string& s) {
	try {
		std::ofstream ofs {path, std::ios_base::binary | std::ios_base::out};
		ofs.write(s.c_str(), (std::streamsize) s.size());
		ofs.close();
	} catch (...) {
		THROW_EXCEPTION_NESTED("path: " + path.string());
	}
}

void synkor::filesystem::save_json(const stdfs::path &path, const json11::Json json) {
	save_string(path, json.dump());
}
