
#include <boost/program_options.hpp>
#include <iostream>

namespace Experimental {
	template <typename T>
	T* addressOf(T& arg) {
		return &arg;
	}
}


int main(int argc, char *argv[])
{
	boost::program_options::options_description generic("Generic options");

	std::cout << "testing...\n";

	return EXIT_SUCCESS;
}