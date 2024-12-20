#include "fdcl/system.hpp"
#include "fdcl/sensors/anemometer.hpp"


void fdcl::thread_ane1(void)
{
    std::cout << "ANE1: thread initialized .." << std::endl;

    fdcl::sensors::Anemo ane1(1);

    // Read IMU configuration parameters.
    {
        std::lock_guard<std::mutex> guard(rover.config_mutex);
        ane1.load_config(rover.config);
    }

    // Check if the anemometer is enabled in the config file, exit otherwise.
    if (!ane1.on) {
        std::cout
            << "ANE1: disabled in the config file"
            << "\n\texiting thread\n"
            << std::endl;
        return;
    }

    ane1.open();
    ane1.init_loop();

    while (rover.on) {
        ane1.loop();
        usleep(40000);  // 25 Hz
    }

    ane1.close();

    std::cout << "ANE1: thread closed!" << std::endl;
}
