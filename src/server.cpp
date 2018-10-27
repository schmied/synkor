
#include <boost/asio.hpp>

#include "../contrib/spdlog/spdlog.h"
#include "../contrib/spdlog/sinks/stdout_sinks.h"

#include "server.hpp"

namespace asio = boost::asio;

const auto log_server = spdlog::stdout_logger_st("server");

const int buf_net_len = 1024;
const int buf_net_to_decrypt_len = 4 * buf_net_len;
const int buf_decrypt_to_decompress_len = 1024;
const int buf_decompress_len = 1024;

char buf_net_to_decrypt[buf_net_to_decrypt_len];
char buf_decrypt_to_decompress[buf_decrypt_to_decompress_len];

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

/*
void server::thread_decrypt(server *server) {

}

void server::thread_decompress(server *server) {

}
*/

server::server(config *config, asio::ip::tcp::socket *socket) {
	_config = config;
	_socket = socket;
}

asio::ip::tcp::socket *server::socket() const {
	return _socket;
}

void server::start(config *config) {

	if (config == nullptr || config->get_listen_port() <= 0)
		return;
	
	boost::asio::io_context asio;
//	for (;;) {
		asio::ip::tcp::acceptor acceptor(asio, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), config->get_listen_port()));
		log_server->info("Server listen on port {}", config->get_listen_port());
		for (;;) {
			const asio::ip::tcp::socket socket(asio);
			std::thread(thread_net, acceptor.accept()).detach();
		}
//	}
}