
#include <iostream>

#include <boost/filesystem.hpp>

#include <sodium.h>

using namespace std;

int main()
{

    std::cout << "Hello World!!!!\n";

    boost::filesystem::path p("C:/");

    if (sodium_init() < 0) {
        return 1;
    }

    std::cout << "Hello World2!!!!\n";

    std::cout << "Hello World!\n" << boost::filesystem::basename(p);
}
