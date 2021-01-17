#include <exception>
#include <iostream>
#include <vector>
#include <string>
#include <thread>

#include "application.hpp"

using namespace hwfractal;

int main(int argc, char *argv[]) {
	
	int status = EXIT_SUCCESS;

	try {
		application app;

		app.init();

		app.loop();

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