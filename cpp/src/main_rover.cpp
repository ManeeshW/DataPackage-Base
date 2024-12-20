#include "fdcl/system.hpp"

#include <csignal>
#include <thread>
#include <vector>

void quit_handler(int sig);


int main(void) {
    std::cout << "ROVER: starting program \n" << std::endl;

    fdcl::rover.init();

    signal(SIGINT, quit_handler);

    std::string config_file_name = "../rover.cfg";
    fdcl::rover.open_config_file(config_file_name);
    
    std::vector<std::thread> threads;
    threads.push_back(std::thread(fdcl::thread_rover_imu));
    threads.push_back(std::thread(fdcl::thread_gps));
    threads.push_back(std::thread(fdcl::thread_ane1));
    threads.push_back(std::thread(fdcl::thread_ane2));
    // threads.push_back(std::thread(fdcl::thread_key));
    threads.push_back(std::thread(fdcl::thread_wifi));

    for (auto &t : threads) {
        t.join();
    }

    fdcl::rover.close_config_file();

    // End of the system program.
    std::cout << "\nROVER: program closed!" << std::endl;
    return 0;
}


void quit_handler(int sig) {
    std::cout << "ROVER: received program kill command" << std::endl;

	try {
        fdcl::rover.close_config_file();
		fdcl::rover.turn_off();
	} catch (int error) {
        std::cout << "ROVER: error " << error << std::endl;
        exit(0);
    }
}