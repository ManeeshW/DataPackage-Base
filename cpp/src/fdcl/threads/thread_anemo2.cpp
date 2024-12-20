#include "fdcl/system.hpp"
#include "fdcl/sensors/anemometer.hpp"


void fdcl::thread_ane2(void)
{
    std::cout << "ANE2: thread initialized .." << std::endl;

    fdcl::sensors::Anemo ane2(2);

    // Read IMU configuration parameters.
    {
        std::lock_guard<std::mutex> guard(rover.config_mutex);
        ane2.load_config(rover.config);
    }

    // Check if the anemometer is enabled in the config file, exit otherwise.
    if (!ane2.on) {
        std::cout
            << "ANE2: disabled in the config file"
            << "\n\texiting thread\n"
            << std::endl;
        return;
    }

    ane2.open();
    ane2.init_loop();

    while (rover.on) {
        ane2.loop();
        usleep(40000);  // 25 Hz
    }

    ane2.close();

    std::cout << "ANE2: thread closed!" << std::endl;
}
