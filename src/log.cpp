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

#include <exception>
#include <iostream>

#include "../contrib/spdlog/spdlog.h"

#include "log.hpp"

/*
 * ******************************************************** private
 */

/*
 * ******************************************************** public
 */

void synkor::log::exception(const std::shared_ptr<spdlog::logger> logger, const std::exception& e, std::size_t level) {
	logger->warn("{}", e.what());
//	logger->warn("{}{}", std::string(level, ' '), e.what());
	try {
		std::rethrow_if_nested(e);
	} catch (const std::exception& e) {
		exception(logger, e, level + 2);
	} catch (...) {
	}
}

void synkor::log::usage() {
	std::cout << std::endl << " Usage: synkor [<base directory> [<peer name>]]" << std::endl << std::endl <<
	  " To initialize the current directory type: ./synkor . my_peer_name" << std::endl << std::endl;
}
