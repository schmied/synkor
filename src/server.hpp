#pragma once

#ifndef SYNKOR_SERVER
#define SYNKOR_SERVER

#include <boost/asio.hpp>

#include "config.hpp"

namespace asio = boost::asio;

class server {

private:
	config *_config;
	asio::ip::tcp::socket *_socket;

//	static void thread_net(server*);
//	static void thread_decrypt(server*);
//	static void thread_decompress(server*);

public:
	server(config*, asio::ip::tcp::socket*);

	asio::ip::tcp::socket *socket() const;
//	const asio::ip::tcp::socket socket();

	static void start(config*);

};

#endif
