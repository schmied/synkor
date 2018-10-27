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
	log_main->info("\n\n Usage: synkor [<directory> [<nodename>]]\n\n To initialize the current directory type: ./synkor . my_nodename\n");
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
	log_main->info("Initialize libsodium.");

	const std::string arg_dir_base = argc == 1 ? "." : argv[1];
	const std::string arg_nodename = argc < 3 ? "" : argv[2];
	std::string error;
	synkor::config config(arg_dir_base, arg_nodename, error);
	if (!error.empty()) {
		log_main->error(error);
		usage();
		return 1;
	}
	synkor::server::start(&config);
}
