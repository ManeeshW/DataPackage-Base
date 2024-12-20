#ifndef FDCL_SWIFTNAV_HPP
#define FDCL_SWIFTNAV_HPP

// user libraries
#include "fdcl/clock.hpp"
#include "fdcl/param.hpp"
#include "fdcl/types/piksi.hpp"

// external libraries
#include "Eigen/Dense"
#include <libserialport.h>
#include "libsbp/sbp.h"
#include "libsbp/system.h"
#include "libsbp/navigation.h"
#include "libsbp/observation.h"

// system headers
#include <iostream>
#include <sys/time.h>
#include <unistd.h>


namespace fdcl {

namespace sensors {


/** \brief Read SwiftNav Piksi Multi GPS sensor
*
* This class reads from the Piksi Multi GPS sensor and parses data to be used
* in fdcl::ekf class
*/
class Swiftnav {
public:
    Swiftnav(void);
    ~Swiftnav(void);

    bool enabled(void);

    /** \fn void load_config(void);
    * Loads the configurations for the GPS from the config file.\n
    */
    void load_config(fdcl::param &config_file);


    /** \fn void open(void)
    *	Open the serial port for the GPS for communication
    */
    void open(void);


    /** \fn void init_loop(void);
    * Attaches all the callback functions for the required SBP messages. Must be
    * called before the loop()
    */
    void init_loop(void);


    /** \fn void loop(void)
    * Reads data from GPS and parses them. This function includes the callback
    * function for the fdcl_ekf class so that the system data is updated at the
    * end of the each loop.
    */
    void loop(void);


    /** \fn void close(void)
    *  Close the serial port opened for communicating with the GPS.
    */
    void close(void);

    /** \fn void setup_port(int baud)
    * Setup the port for the GPS for reading from Pikis Multi
    * @param baud baud rate of the Piksi Multi
    */
    void setup_port(int baud);

private:

    int ret = 0;
    static int loop_count;  /**< Counts the number of GPS callbacks, 
        * LLH callback keeps incrementing this count, and baselibe message
        * (i.e. RTK GPS solution) keeps resetting this value to zero. If the 
        * count is greater than 3, rtk_solution flag is set to false.
        */

    sbp_state_t s;
    sbp_state_t s0;

    struct timeval stop, start;

    static bool flag_start; /**< GPS main loop will not start until this flag is
        * is set to True. By default this is set to False. This is set to True 
        * when the first heartbeat signal from the GPS is detected
        */

    const char *serial_port_name = NULL; /**< Name of the serial port for the
        * GPS sensor. This is assigned in the code
        */

    std::string port = "\\dev\\ttyUSB0"; /**< Name of the serial port for the
        * GPS sensor. This can be configured through the config file
        */

    int baud_rate = 115200; /** Baud rate of the GPS sensor. This can be
        * configured through the config file
        */

    bool on = 0; /**< Boolean flag to make GPS on/off. The GPS thread will only
        * continue is this is set to True. This can be configured through the 
        * config file
        */

    // This inline static variable declaration requires C++17
    inline static double dt_sync = 0.0;
    inline static fdcl::Clock clock;


    void setup_port(struct sp_port *piksi_port, int baud);
    static s32 piksi_port_read(u8 *buff, u32 n, void *context);

    // callback functions for each message type read from the sensor
    static void heartbeat_callback_0(
        u16 sender_id, u8 len, u8 msg[], void *context
    );
    static void heartbeat_callback(
        u16 sender_id, u8 len, u8 msg[], void *context
    );
    static void baseline_callback(
        u16 sender_id, u8 len, u8 msg[], void *context
    );
    static void pos_llh_callback(
        u16 sender_id, u8 len, u8 msg[], void *context
    );
    static void pos_ecef_callback(
        u16 sender_id, u8 len, u8 msg[], void *context
    );
    static void vel_ned_callback(
        u16 sender_id, u8 len, u8 msg[], void *context
    );
    static void gps_time_callback(
        u16 sender_id, u8 len, u8 msg[], void *context
    );
    static void base_pos_llh_callback(
        u16 sender_id, u8 len, u8 msg[], void *context
    );
    static void base_pos_ecef_callback(
        u16 sender_id, u8 len, u8 msg[], void *context
    );
    // static void obs_callback(u16 sender_id, u8 len, u8 msg[], void *context);
    // static void obs_callback(u16 sender_id, u8 len, u8 msg[], void *context);
};

}  // end of namespace sensors
}  // end of namespace fdcl

#endif
