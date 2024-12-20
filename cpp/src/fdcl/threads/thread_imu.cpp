#include "fdcl/system.hpp"

#include <pigpio.h>
#include "bno055/RPi_BNO055.h"
#include "bno055/RPi_Sensor.h"
#include "bno055/utilitiy/imumaths.h"

double dt_imu_sync = 0.0;
#define BNO055_SAMPLERATE_DELAY_MS (80)



void fdcl::thread_imu(void) {
    std::cout << "IMU: thread started" << std::endl;

    Adafruit_BNO055 bno = Adafruit_BNO055();

    bool imu_on = false;
    fdcl::Clock clock("log imu clock");

    // Read IMU configuration parameters.
    {
        std::lock_guard<std::mutex> guard(rover.config_mutex);
        rover.config.read("IMU.on", imu_on);
    }

    // Check if the IMU is enabled in the config file, exit otherwise.
    if (!imu_on) {
        std::cout 
            << "IMU: disabled in the config file"
            << "\n\texiting thread\n" 
            << std::endl;
        return;
    }

    // Sync time with the main clock
    {
        clock.reset();

        std::lock_guard<std::mutex> guard(rover.clock_mutex);
        dt_imu_sync = rover.clock.get_seconds() - clock.get_seconds();

        if (dt_imu_sync < 0) {
            std::cerr << "IMU: time sync is negative\n\texiting thread"
                << std::endl;
            return;
        }

        std::cout << "IMU: time sync is " << dt_imu_sync << " s" << std::endl;
    }

    if (gpioInitialise() < 0) {
        std::cout << "IMU: initialisation error of the GPIO" 
            << "\n\tclosing thread" 
            << std::endl;
        return;
    }

    bno._HandleBNO = i2cOpen(bno._i2cChannel, BNO055_ADDRESS_A, 0);

    if (!bno.begin()) {
        std::cout << "IMU: no BNO055 detected" 
            << "\n\tcheck wiring or I2C ADDR\n\tclosing thread" 
            << std::endl;
        return;
    }

    vector3 ypr_i = vector3::Zero();
    vector3 a_i = vector3::Zero();
    vector3 W_i = vector3::Zero();

    while (rover.on) {
        imu::Vector<3> ypr = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
        imu::Vector<3> a = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
        imu::Vector<3> W = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);

        ypr_i(0) = ypr.x();
        ypr_i(1) = ypr.y();
        ypr_i(2) = ypr.z();

        a_i(0) = a.x();
        a_i(1) = a.y();
        a_i(2) = a.z();

        W_i(0) = W.x();
        W_i(1) = W.y();
        W_i(2) = W.z();

        rover.update_imu(ypr_i, a_i, W_i);

        gpioSleep(PI_TIME_RELATIVE, 0, 1000 * BNO055_SAMPLERATE_DELAY_MS);
    }

    std::cout << "IMU: thread closed" << std::endl;
}
