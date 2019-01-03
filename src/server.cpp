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

#include "../contrib/asio/asio.hpp"
#include "../contrib/spdlog/spdlog.h"
#include "../contrib/spdlog/sinks/stdout_sinks.h"

#include "server.hpp"

/*
 * ******************************************************** private
 */

static const auto logger = spdlog::stdout_logger_st(stdfs::path(__FILE__).stem().string());

static const int buf_net_len = 1024;
static const int buf_net_to_decrypt_len = 4 * buf_net_len;
static const int buf_decrypt_to_decompress_len = 1024;
static const int buf_decompress_len = 1024;

static char buf_net_to_decrypt[buf_net_to_decrypt_len];
static char buf_decrypt_to_decompress[buf_decrypt_to_decompress_len];

/*
void server(asio::ip::tcp::socket socket) {

	//	int buf_idx_net_start = 0;
	int buf_idx_net_to_decrypt_start = 0;
	char buf_net[buf_net_len];
	char buf_net_to_decrypt[buf_net_to_decrypt_len];
	char buf_decrypt_to_decompress[buf_decrypt_to_decompress_len];
	char buf_decompress[buf_decompress_len];

	for (;;) {
		boost::system::error_code error;
		size_t length = socket.read_some(boost::asio::buffer(buf_net), error);
		if (error == boost::asio::error::eof)
			break; // Connection closed cleanly by peer.
		else if (error)
			throw boost::system::system_error(error); // Some other error.

		log_server->info("buf_net: \n", buf_net);
//		char *start = buf_idx_net_to_decrypt_start;

		std::memcpy(buf_net_to_decrypt, buf_net, length);
		//		boost::asio::write(sock, boost::asio::buffer(data, length));
	}
	log_server->info("session closed");
}
*/

void thread_net(asio::ip::tcp::socket socket) {
	char buf_net[buf_net_len];
//	asio::ip::tcp::socket *socket = server->socket();
	for (;;) {
		std::error_code error;
		const auto length = socket.read_some(asio::buffer(buf_net), error);
		if (error == asio::error::eof)
			break; // Connection closed cleanly by peer.
		else if (error)
			throw std::system_error(error); // Some other error.

		logger->info("buf_net: \n", buf_net);
		//		char *start = buf_idx_net_to_decrypt_start;

		std::memcpy(buf_net_to_decrypt, buf_net, length);
		//		boost::asio::write(sock, boost::asio::buffer(data, length));
	}
	logger->info("session closed");
}

/*
void server::thread_decrypt(server *server) {

}

void server::thread_decompress(server *server) {

}
*/

/*
 * ******************************************************** public
 */

synkor::server::server(config *config, asio::ip::tcp::socket *socket) {
	_config = config;
	_socket = socket;
}

asio::ip::tcp::socket *synkor::server::socket() const {
	return _socket;
}

void synkor::server::start(config *config) {

	if (config == nullptr || config->listen_port() <= 0)
		return;

	asio::io_context asio;
//	for (;;) {
		asio::ip::tcp::acceptor acceptor {asio, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), config->listen_port())};
		logger->info("Server listen on port {}", config->listen_port());
		for (;;) {
			const asio::ip::tcp::socket socket {asio};
			std::thread(thread_net, acceptor.accept()).detach();
		}
//	}
}
