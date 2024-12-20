#ifndef FDCL_SENSORS_IMU_HPP
#define FDCL_SENSORS_IMU_HPP


#include "fdcl/clock.hpp"
#include "fdcl/param.hpp"
#include "fdcl/types/vectors.hpp"

// external headers
#include "Eigen/Dense"
#include "vn/ezasyncdata.h" // access EzAsyncData class.
#include "vn/thread.h"      // vetornav sleep function.


namespace fdcl {
namespace sensors {

class Imu {
public:
    Imu(void);
    ~Imu(void);

    /** \fn void load_config(void)
     * Loads the configurations for the IMU from the config file.\n\n
     * Configurations:\n
     * on: on/off IMU\n
     * port: name of the serial port used by the IMU\n
     * baud_rate: baud rate used by the IMU
     */
    void load_config(fdcl::param &config_file);

    /** \fn void close(void)
     *  Close the serial port opened for communicating with the IMU.
     */
    void close(void);


    /** \fn static void parse_vn100_packet(void* userData,
     * vn::protocol::uart::Packet& p, size_t index)
     * Defines operations needs to performed when new message from the IMU is
     * arived.
     */
    static void parse_vn100_packet(
        void *userData, vn::protocol::uart::Packet &p, size_t index);


    /** \fn void open(void)
     *	Open the serial port for the IMU for communication
     */
    void open(void);


    /** \fn void open(std::string  port, int baud_rate);
     * Set the registers of the IMU for reading data
     * @param port      name of the port
     * @param baud_rate baud rate of the port
     */
    void open(std::string port, int baud_rate);


    void loop(void);
    bool enabled(void);

    void update_dt_sync(const double t);
    double get_time(void);

private:
    vn::sensors::VnSensor vs;

    std::string port; /**< Name of the port of the IMU. Use the config
        * file to set values of this parameter.
        */

    int port_number; /**< Name of the IMU port. If RS232 cable is used,
        * this port should be set to port 1. If the IMU connected to the
        * computer through the PCB or FTDI cable, this must be set to 2.
        */

    int baud_rate; /**< Baud rate of the port for the IMU */


    bool on = false;
    double mag_declination = 0.0;
    bool is_open = false;

    // This inline static variable declaration requires C++17
    inline static double dt_sync = 0.0;
    inline static fdcl::Clock clock;

    bool config_vn100_message(const int freq);
    static void check_binary_message_vn100(vn::protocol::uart::Packet &p);
};
} // end of namepsace sensors
} // end of namepsace fdcl

#endif