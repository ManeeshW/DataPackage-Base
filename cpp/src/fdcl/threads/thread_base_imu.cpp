#include "fdcl/system.hpp"
#include "fdcl/sensors/vn100.hpp"

double dt_imu_sync = 0.0;


void fdcl::thread_base_imu(void) {
    std::cout << "IMU: thread started" << std::endl;
    
    fdcl::sensors::Imu imu;

    // Read IMU configuration parameters.
    {
        std::lock_guard<std::mutex> guard(base.config_mutex);
        imu.load_config(base.config);
    }

    // Check if the IMU is enabled in the config file, exit otherwise.
    if (!imu.enabled()) {
        std::cout 
            << "IMU: disabled in the config file"
            << "\n\texiting thread\n" 
            << std::endl;
        return;
    }

    // Sync time with the main clock
    {
        std::lock_guard<std::mutex> guard(base.clock_mutex);
        dt_imu_sync = base.clock.get_seconds() - imu.get_time();

        if (dt_imu_sync < 0) {
            std::cerr << "IMU: time sync is negative\n\texiting thread"
                << std::endl;
            return;
        }

        std::cout << "IMU: time sync is " << dt_imu_sync << " s" << std::endl;
    }

    imu.update_dt_sync(dt_imu_sync);
    imu.open();

    while (base.on) {
        imu.loop();
    }

    imu.close();

    std::cout << "IMU: thread closed" << std::endl;
}