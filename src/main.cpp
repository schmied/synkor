
#include <iostream>

#include <boost/log/trivial.hpp>

#include <sodium.h>

#include "config.hpp"

int main(int argc, char **argv)
{
	if (argc != 2) {
		BOOST_LOG_TRIVIAL(error) << "Usage: synkor <directory>";
		return 1;
	}

	if (sodium_init() != 0) {
		BOOST_LOG_TRIVIAL(error) << "Error: sodium_init()";
		return 1;
	}
	BOOST_LOG_TRIVIAL(info) << "libsodium initialized.";

	std::string error;
	config config(argv[1], error);
	if (!error.empty()) {
		BOOST_LOG_TRIVIAL(error) << error;
		return 1;
	}
}
