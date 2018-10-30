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

#include <iostream>

#include "../contrib/sodium/sodium.h"
#include "../contrib/spdlog/spdlog.h"
#include "../contrib/spdlog/sinks/stdout_sinks.h"

#include "config.hpp"
#include "server.hpp"

const auto log_main = spdlog::stdout_logger_st("main");

static void usage() {
	log_main->info("\n\n Usage: synkor [<base directory> [<peer name>]]\n\n To initialize the current directory type: ./synkor . my_peer_name\n");
}

void log_exception(const std::exception& e, int level =  0) {
	log_main->info("[exception] {}{}", std::string(level, ' '), e.what());
	try {
		std::rethrow_if_nested(e);
	} catch (const std::exception& e) {
		log_exception(e, level + 2);
	} catch (...) {
	}
}

int main(int argc, char **argv) {

	if (argc > 3) {
		usage();
		return 1;
	}

	const int error_sodium = sodium_init();
	if (error_sodium != 0) {
		log_main->error("sodium_init() error: {}", error_sodium);
		return 1;
	}
	log_main->info("initialize libsodium");

	const stdfs::path arg_dir_base {argc == 1 ? "." : argv[1]};
	std::string arg_peername;
	if (argc == 3)
		arg_peername.assign(argv[2]);

	try {
		synkor::config config {arg_dir_base, arg_peername};
		synkor::server::start(&config);
	} catch (const std::exception& e) {
		log_exception(e);
		usage();
		return 1;
	}
}
