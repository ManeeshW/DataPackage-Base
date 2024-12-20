#include "fdcl/gui.hpp"
#include "fdcl/system.hpp"

#include <csignal>
#include <thread>
#include <vector>

void quit_handler(int sig);


int main(void) {
    std::cout << "BASE: starting program \n" << std::endl;

    fdcl::base.init();

    signal(SIGINT, quit_handler);

    std::string config_file_name = "../base.cfg";
    fdcl::base.open_config_file(config_file_name);
    
    std::vector<std::thread> threads;
    threads.push_back(std::thread(fdcl::thread_base_imu));
    threads.push_back(std::thread(fdcl::thread_gps));
    // threads.push_back(std::thread(fdcl::thread_key));
    threads.push_back(std::thread(fdcl::thread_logging));
    // threads.push_back(std::thread(fdcl::thread_gui));
    threads.push_back(std::thread(fdcl::thread_wifi));

    fdcl::Gui gui;
    gui.run();

    for (auto &t : threads) {
        t.join();
    }

    fdcl::base.close_config_file();

    // End of the system program.
    std::cout << "\nBASE: program closed!" << std::endl;
    return 0;
}


void quit_handler(int sig) {
    std::cout << "BASE: received program kill command" << std::endl;

	try {
        fdcl::base.close_config_file();
		fdcl::base.turn_off();
	} catch (int error) {
        std::cout << "BASE: error " << error << std::endl;
        exit(0);
    }
}