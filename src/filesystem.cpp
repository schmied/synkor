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

#include "global.hpp"

#include "filesystem.hpp"

const auto log_filesystem = spdlog::stdout_logger_st("filesystem");

const stdfs::path synkor::filesystem::check_path_canonical(const stdfs::path &path, std::string &error) {
	error.clear();
	stdfs::path path_norm = path.lexically_normal();
	std::error_code ec;
	stdfs::path pathc = stdfs::canonical(path_norm, ec);
	if (ec.value()) {
		error.assign("canonical() error code " + ec.value());
		return path_norm;
	}
	return pathc.lexically_normal();
}

const stdfs::path synkor::filesystem::check_file_readable(const stdfs::path &path, std::string &error) {
	error.clear();
	stdfs::path path_norm = path.lexically_normal();
	if (!stdfs::is_regular_file(path_norm)) {
		error.assign("No file " + path_norm.string());
		return path_norm;
	}
	if ((stdfs::status(path_norm).permissions() & stdfs::perms::owner_read) == stdfs::perms::none) {
		error.assign("Not a readable file: " + path_norm.string());
		return path_norm;
	}
	return check_path_canonical(path_norm, error);
}

const stdfs::path synkor::filesystem::check_dir_writable(const stdfs::path &path, const bool create, std::string &error) {
	error.clear();
	stdfs::path path_norm = path.lexically_normal();
	if (!stdfs::exists(path_norm)) {
		if (!create) {
			error.assign("No directory " + path_norm.string());
			return path_norm;
		}
		if (!stdfs::create_directories(path_norm)) {
			error.assign("Cannot create directory " + path_norm.string());
			return path_norm;
		}
		log_filesystem->info("Create directory {}", path_norm.string());
	}
	if (!stdfs::is_directory(path_norm)) {
		error.assign("No directory " + path_norm.string());
		return path_norm;
	}
	if ((stdfs::status(path_norm).permissions() & (stdfs::perms::owner_read | stdfs::perms::owner_write)) == stdfs::perms::none) {
		error.assign("Not a readable/writable directory: " + path_norm.string());
		return path_norm;
	}
	return check_path_canonical(path_norm, error);
}

std::string synkor::filesystem::load_string(const stdfs::path &path, std::string &error) {
	error.clear();
	std::string s;
	check_file_readable(path, error);
	if (!error.empty()) {
		log_filesystem->error(error);
		return s;
	}
	std::ifstream ifs { path, std::ios_base::binary | std::ios_base::in };
	asio::streambuf sb;
	const size_t buf_len = 1024;
	while (ifs) {
		asio::mutable_buffer in = sb.prepare(buf_len);
		ifs.read((char*)in.data(), buf_len);
		sb.commit(ifs.gcount());
		asio::const_buffer out = sb.data();
		s.append((char*) out.data());
		sb.consume(out.size());
	}
	if (ifs.is_open())
		ifs.close();
	return s;
}

json11::Json synkor::filesystem::load_json(const stdfs::path& path, std::string& error) {
	error.clear();
	std::string s = load_string(path, error);
	if (!error.empty()) {
		log_filesystem->error(error);
		return json11::Json();
	}
	json11::Json json = json11::Json::parse(s, error);
	if (!error.empty())
		error.assign("Cannot parse json from " + path.string() + ": " + error);
	return json;
}

void synkor::filesystem::save_string(const stdfs::path &path, const std::string& s) {
	std::ofstream ofs{ path, std::ios_base::binary | std::ios_base::out };
	ofs.write(s.c_str(), s.size());
	ofs.close();
}

void synkor::filesystem::save_json(const stdfs::path &path, const json11::Json json) {
	save_string(path, json.dump());
}
