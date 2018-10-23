
#include <iostream>

#include <sodium.h>

#include "../contrib/spdlog/spdlog.h"
#include "../contrib/spdlog/sinks/stdout_sinks.h"

#include "config.hpp"
#include "server.hpp"

namespace fs = boost::filesystem;

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
	config config(arg_dir_base, arg_nodename, error);
	if (!error.empty()) {
		log_main->error(error);
		usage();
		return 1;
	}
	server::start(&config);
}
