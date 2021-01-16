#include <cstdlib>
#include <exception>
#include <iostream>
#include <vector>
#include <string>

#include "application.hpp"

using namespace hwfractal;

int main(int argc, char *argv[]) {
	application *app = NULL;
	int status = EXIT_SUCCESS;

	try {
		app = new application();

		std::cout << *argv << ": Program successfull." << std::endl;
	} catch (const std::exception &exception) {
		std::cerr << *argv << ": Caught: " << exception.what() << std::endl;
		status = EXIT_FAILURE;
	} catch(...) {
		std::cerr << *argv << ": Caught unknown exception" << std::endl;
		status = EXIT_FAILURE;
	}
	delete app;
	
	return status;
}