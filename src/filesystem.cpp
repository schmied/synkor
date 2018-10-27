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

#include "../contrib/json11/json11.hpp"
#include "../contrib/spdlog/spdlog.h"
#include "../contrib/spdlog/sinks/stdout_sinks.h"

#include "global.hpp"

#include "filesystem.hpp"

const auto log_filesystem = spdlog::stdout_logger_st("filesystem");

const stdfs::path synkor::filesystem::check_path_canonical(const stdfs::path path, std::string &error) {
	error.clear();
	std::error_code ec;
	stdfs::path pathc = stdfs::canonical(path, ec);
	if (ec.value()) {
		error.assign("canonical() error code " + ec.value());
		return path;
	}
	return pathc;
}

const stdfs::path synkor::filesystem::check_file_readable(const stdfs::path path, std::string &error) {
	error.clear();
	if (!stdfs::is_regular_file(path)) {
		error.assign("No file " + path.string());
		return path;
	}
	if ((stdfs::status(path).permissions() & stdfs::perms::owner_read) == stdfs::perms::none) {
		error.assign("Not a readable file: " + path.string());
		return path;
	}
	return check_path_canonical(path, error);
}

const stdfs::path synkor::filesystem::check_dir_writable(const stdfs::path path, const bool create, std::string &error) {
	error.clear();
	if (!stdfs::exists(path)) {
		if (!create) {
			error.assign("No directory " + path.string());
			return path;
		}
		if (!stdfs::create_directories(path)) {
			error.assign("Cannot create directory " + path.string());
			return path;
		}
		log_filesystem->info("Create directory {}", path.string());
	}
	if (!stdfs::is_directory(path)) {
		error.assign("No directory " + path.string());
		return path;
	}
	if ((stdfs::status(path).permissions() & (stdfs::perms::owner_read | stdfs::perms::owner_write)) == stdfs::perms::none) {
		error.assign("Not a readable/writable directory: " + path.string());
		return path;
	}
	return check_path_canonical(path, error);
}

void synkor::filesystem::load_string(const stdfs::path& path, std::string& s) {
	std::ifstream file;
//	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	file.open(path, std::ios_base::binary);
	std::size_t size = static_cast<std::size_t>(file_size(path));
	s.resize(size, '\0');
	file.read(&s[0], size);
	file.close();
}

json11::Json synkor::filesystem::load_json(const stdfs::path& path, std::string& error) {
	std::string s;
	log_filesystem->info("x1");
	load_string(path, s);
	log_filesystem->info("x2");
	return json11::Json::parse(s, error);
}

void synkor::filesystem::save_string(const stdfs::path &p, const std::string& s) {
	std::ofstream file;
	file.exceptions(std::ofstream::failbit | std::ofstream::badbit);
	file.open(p, std::ios_base::binary);
	file.write(s.c_str(), s.size());
	file.close();
}

void synkor::filesystem::save_json(const stdfs::path &p, const json11::Json json, const std::string& s) {
}
