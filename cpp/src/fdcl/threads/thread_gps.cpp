#include "fdcl/system.hpp"
#include "fdcl/sensors/swiftnav.hpp"


double dt_gps_sync = 0.0;


void fdcl::thread_gps(void) {
    std::cout << "GPS: thread started" << std::endl;
    
    fdcl::sensors::Swiftnav gps;

    // Read GPS configuration parameters.
    {
        #if defined FDCL_ROVER
        std::lock_guard<std::mutex> guard(rover.config_mutex);
        gps.load_config(rover.config);
        #elif defined FDCL_BASE
        std::lock_guard<std::mutex> guard(base.config_mutex);
        gps.load_config(base.config);
        #endif
    }

    // Check if the GPS is enabled in the config file, exit otherwise.
    if (!gps.enabled()) {
        std::cout 
            << "GPS: disabled in the config file"
            << "\n\texiting thread\n" 
            << std::endl;
        return;
    }

    gps.open();
    gps.init_loop();

    #if defined FDCL_ROVER
    while (rover.on) {
        gps.loop();
    }
    #elif defined FDCL_BASE
    while (base.on) {
        gps.loop();
    }
    #endif

    gps.close();

    std::cout << "GPS: thread closed" << std::endl;
}
