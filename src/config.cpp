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

#include "../contrib/json11/json11.hpp"
#include "../contrib/spdlog/spdlog.h"
#include "../contrib/spdlog/sinks/stdout_sinks.h"

#include "base.hpp"
#include "config.hpp"
#include "global.hpp"
#include "log.hpp"
#include "filesystem.hpp"

/*
 * ******************************************************** private
 */

static const auto logger = spdlog::stdout_logger_st(stdfs::path(__FILE__).stem().string());

static const stdfs::path FILE_CONFIG {"synkor.conf"};

static const std::string JSON_KEY_PEERNAME {"peername"};
static const std::string JSON_KEY_LISTEN_PORT {"listen_port"};

const stdfs::path synkor::config::_path_file_config(const stdfs::path &dir_base) {
    return stdfs::path {dir_base.string() + "/" + FILE_CONFIG.string()}.lexically_normal();
}

const std::string synkor::config::_peername(const stdfs::path &dir_base, const std::string &peername) {
    const auto file_config = _path_file_config(dir_base);
    const auto is_file_config = filesystem::is_file_readable(file_config);
    if (!is_file_config && peername.empty())
            THROW_EXCEPTION("no peer name defined, cannot initialize directory: " + dir_base.string());
    if (is_file_config && !peername.empty())
            THROW_EXCEPTION("directory already initialized: " + dir_base.string());
    if (is_file_config) {
        const auto config_json = filesystem::load_json(file_config);
        const auto peername_json = config_json[JSON_KEY_PEERNAME].string_value();
        if (peername_json.empty())
            THROW_EXCEPTION("no peer name defined in config file: " + file_config.string());
        if (peername_json != base::valid_peername(peername_json))
            THROW_EXCEPTION("peer name defined in config file not valid.");
        return base::valid_peername(peername_json);
    }
    const auto valid_peername = base::valid_peername(peername);
    filesystem::save_string(file_config, "{ \"" + JSON_KEY_PEERNAME + "\": \"" + valid_peername + "\" }");
    logger->info("create config file: {}", file_config.string());
    return valid_peername;
}

/*
 * ******************************************************** public
 */

synkor::config::config(const stdfs::path &dir_base, const std::string &peername) : _base(dir_base, _peername(dir_base, peername)) {
    const auto file_config = _path_file_config(_base.dir_base());
    const auto config_json = filesystem::load_json(file_config);
    logger->info("load config file: {}", file_config.string());
    _listen_port = config_json[JSON_KEY_LISTEN_PORT].int_value();
    logger->info("listen port: {}", _listen_port);
}

int synkor::config::listen_port() const {
    return _listen_port;
}
