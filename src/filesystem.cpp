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
#include "../contrib/spdlog/sinks/basic_file_sink.h"
//#include "../contrib/spdlog/sinks/stdout_sinks.h"

#include "filesystem.hpp"
#include "global.hpp"
#include "log.hpp"

/*
 * ******************************************************** private
 */

static const auto logger = spdlog::basic_logger_mt(stdfs::path(__FILE__).stem().string(), "synkor.log");
//static const auto logger = spdlog::stdout_logger_st(stdfs::path(__FILE__).stem().string());

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
		const std::size_t buf_len = 1024;
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




#include <fstream>
#include <thread>
#include <contrib/asio/asio.hpp>
#include <mutex>
#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <chrono>


//const std::size_t bufsize = 1048576;
static const std::size_t bufsize_asio = 16777216;
static const std::size_t bufsize_fstream = 262144;
//const std::size_t bufsize = 8192;
static asio::streambuf sb {bufsize_asio};
static std::mutex mtx;

static const std::size_t data_size_len_path = 2;
static const std::size_t data_size_len_content = 4;

void synkor::filesystem::thread_read(const std::vector<stdfs::path> src_paths) {
	char buf[bufsize_fstream];
	std::ios_base::sync_with_stdio(false);
//	bool run = true;
//	const std::filesystem::path path {action_items_src.substr(8)}; // remove 'file:///'
	std::ifstream ifs;
	ifs.rdbuf()->pubsetbuf(buf, bufsize_fstream);
//	ifs.open(path, std::ios_base::binary | std::ios_base::in);

//	bool is_content_size_written = false;

	for (auto src_path : src_paths) {

//		const auto pathstring = src_path.string();
		logger->info("read path1 {:s}", src_path.string());
		if (src_path.string().find("file:///") == 0)
			src_path = src_path.string().substr(8);
		logger->info("read path2 {:s}", src_path.string());
		const auto pathstring_size = src_path.string().size();

		ifs.open(src_path, std::ios_base::binary | std::ios_base::in);

		ifs.seekg(0, std::ios_base::end);
		const auto file_size = static_cast<std::size_t>(ifs.tellg());
		ifs.seekg(0, std::ios_base::beg);



		mtx.lock();

		for (auto i = data_size_len_path; i > 0; i--) {
			auto buf_dst = sb.prepare(1);
			auto c = static_cast<unsigned char*>(buf_dst.data());
			const auto shift = (i - 1) * std::numeric_limits<unsigned char>::digits;
			const unsigned char s = (pathstring_size >> shift) & 0xff;
			logger->info("read data size {:d} ({:d} >> {:d})", s, pathstring_size, shift);
			std::memcpy(c, &s, 1);
			sb.commit(1);
		}

		auto buf_dst_path = sb.prepare(pathstring_size);
		//asio::buffer_copy(buf_dst_path.data(), pathstring.c_str(), pathstring_size);
		std::memcpy(buf_dst_path.data(), src_path.c_str(), pathstring_size);
		sb.commit(pathstring_size);

		for (auto i = data_size_len_content; i > 0; i--) {
			auto buf_dst = sb.prepare(1);
			auto c = static_cast<unsigned char*>(buf_dst.data());
			const unsigned char s = (file_size >> (i * sizeof (unsigned char))) & 0xff;
			std::memcpy(c, &s, 1);
			sb.commit(1);
		}

		mtx.unlock();



		while (ifs) {

			mtx.lock();
			logger->info("read lock");

			const auto read_size_max = sb.max_size() - sb.size();
			logger->info("read size max {:d}", read_size_max);
			auto buf_dst = sb.prepare(read_size_max);
			logger->info("read {:d}", buf_dst.size());
			ifs.read(static_cast<char*>(buf_dst.data()), static_cast<std::streamsize>(read_size_max));
			const auto read_size = static_cast<std::size_t>(ifs.gcount());
			logger->info("read read {:d}", read_size);
			sb.commit(read_size);

			mtx.unlock();
			logger->info("read unlock");

			using namespace std::chrono_literals;
			std::this_thread::sleep_for(1ms);
		}
//		run = false;

		ifs.close();
	}
	logger->info("reader done");
}

enum write_buf_state {path, content};

void synkor::filesystem::thread_write(const stdfs::path dst_path) {
	char buf[bufsize_fstream];
	std::ios_base::sync_with_stdio(false);
	bool run = true;
	int no_data_count = 0;
	std::ofstream ofs;
	ofs.rdbuf()->pubsetbuf(buf, bufsize_fstream);

	stdfs::path path;
	std::size_t written_size = 0;

	write_buf_state state {write_buf_state::path};
	std::size_t data_size = 0;
	std::size_t data_size_to_read = data_size_len_path;

	while (run) {

		mtx.lock();
		logger->info("write lock");

		if (sb.size() == 0)
			no_data_count++;

		while (sb.size() > 0) {

			const auto buf_src = sb.data();

			if (data_size_to_read > 0) {
				data_size_to_read--;
				const auto c = static_cast<const unsigned char*>(buf_src.data());
				data_size += static_cast<std::size_t>(*c) << (data_size_to_read * sizeof (unsigned char));
				sb.consume(1);
				logger->info("write data size {:d}", data_size);
				continue;
			}

			switch (state) {
			case write_buf_state::path:
				{
					if (data_size == 0)
						goto done;
					const auto c = static_cast<const char*>(buf_src.data());
					const auto size = std::min(buf_src.size(), data_size - path.string().size());
					path = path.string().append(c, size);
					sb.consume(size);
					if (path.string().size() == data_size) {
						written_size = 0;
						state = write_buf_state::content;
						data_size = 0;
						data_size_to_read = data_size_len_content;
					}
				}
				logger->info("write path {:s}", path.string());
				break;
			case write_buf_state::content:
				{
					if (written_size == 0) {
						stdfs::path p = dst_path;
						p /= path;
						ofs.open(p, std::ios_base::binary | std::ios_base::out);
					}
					const auto c = static_cast<const char*>(buf_src.data());
					const auto size = std::min(buf_src.size(), data_size - written_size);
					ofs.write(c, static_cast<std::streamsize>(size));
					sb.consume(size);
					written_size += size;
					if (written_size == data_size) {
						ofs.close();
						path.clear();
						state = write_buf_state::path;
						data_size = 0;
						data_size_to_read = data_size_len_path;
					}
				}
				break;
			}
		}

done:
		mtx.unlock();
		logger->info("write unlock");

		if (no_data_count > 10) {
			run = false;
		} else {
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(1ms);
		}
	}
	logger->info("write done");
}
