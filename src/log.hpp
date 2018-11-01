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

#ifndef SYNKOR_LOG
#define SYNKOR_LOG

#include <exception>

#include "../contrib/spdlog/spdlog.h"

#define THROW_EXCEPTION(WHAT) throw std::runtime_error {__func__ + std::string("() ") + WHAT};
#define THROW_EXCEPTION_NESTED(WHAT) std::throw_with_nested(std::runtime_error {__func__ + std::string("() ") + WHAT});

namespace synkor {

class log {

private:

public:
	static void exception(const std::shared_ptr<spdlog::logger>, const std::exception&, size_t level = 0);
	static void usage();

};

}

#endif
