
#include <iostream>

#include <boost/asio.hpp>
//#include <boost/log/trivial.hpp>

#include <sodium.h>

#include "../contrib/spdlog/spdlog.h"
#include "../contrib/spdlog/sinks/stdout_sinks.h"

#include "config.hpp"

const int max_length = 1024;

namespace asio = boost::asio;

const auto console = spdlog::stdout_logger_st("main");

void session(asio::ip::tcp::socket socket) {
	for (;;)
	{
		char data[max_length];

		boost::system::error_code error;
		size_t length = socket.read_some(boost::asio::buffer(data), error);
		if (error == boost::asio::error::eof)
			break; // Connection closed cleanly by peer.
		else if (error)
			throw boost::system::system_error(error); // Some other error.

		console->info("data");
//		boost::asio::write(sock, boost::asio::buffer(data, length));
	}
	console->info("session closed");
}

int main(int argc, char **argv)
{
	if (argc != 2) {
//		BOOST_LOG_TRIVIAL(error) << "Usage: synkor <directory>";
		console->info("Usage: synkor <directory>");
		return 1;
	}

	const int error_sodium = sodium_init();
	if (error_sodium != 0) {
//		BOOST_LOG_TRIVIAL(error) << "Error: sodium_init()";
		console->info("sodium_init() error: {}", error_sodium);
		return 1;
	}
//	BOOST_LOG_TRIVIAL(info) << "libsodium initialized.";

	std::string error_config;
	config config(argv[1], error_config);
	if (!error_config.empty()) {
//		BOOST_LOG_TRIVIAL(error) << error;
		console->info(error_config);
		return 1;
	}

	boost::asio::io_context asio;

	if (config.get_listen_port() > 0) {
		asio::ip::tcp::acceptor acceptor(asio, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), config.get_listen_port()));
		for (;;) {
			console->info("iter");
			asio::ip::tcp::socket socket(asio);
			std::thread(session, acceptor.accept()).detach();
		}
	}
}
