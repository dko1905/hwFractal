#include <exception>
#include <iostream>
#include <vector>
#include <string>
#include <memory>

#include "application.hpp"
#include "util/config.hpp"

using namespace hwfractal;

int main(int argc, char *argv[]) {	
	int status = EXIT_SUCCESS;

	try {
		auto conf = std::make_shared<config>("resources/config.conf");
		application app(conf);

		std::cout << *argv << ": Program successfull." << std::endl;
	} catch (const std::exception &exception) {
		std::cerr << *argv << ": Caught: " << exception.what() << std::endl;
		status = EXIT_FAILURE;
	} catch(...) {
		std::cerr << *argv << ": Caught unknown exception" << std::endl;
		status = EXIT_FAILURE;
	}
	
	return status;
}