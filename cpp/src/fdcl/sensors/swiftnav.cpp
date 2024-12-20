#include "fdcl/sensors/swiftnav.hpp"
#include "fdcl/system.hpp"


static sbp_msg_callbacks_node_t heartbeat_node_0;
static sbp_msg_callbacks_node_t gps_time_node;
static sbp_msg_callbacks_node_t pos_llh_node;
static sbp_msg_callbacks_node_t pos_ecef_node;
static sbp_msg_callbacks_node_t base_pos_llh_node;
static sbp_msg_callbacks_node_t vel_ned_node;
static sbp_msg_callbacks_node_t baseline_node;
static sbp_msg_callbacks_node_t heartbeat_node;
static sbp_msg_callbacks_node_t obs_node;


static struct sp_port *piksi_port;
struct fdcl::piksi_t piksi;

bool fdcl::sensors::Swiftnav::flag_start = 0;
int fdcl::sensors::Swiftnav::loop_count = 0;
// bool fdcl::sensors::Swiftnav::rtk_solution = false;


fdcl::sensors::Swiftnav::Swiftnav(void) {
    ; // do nothing
}


fdcl::sensors::Swiftnav::~Swiftnav(void) {
    ; // do nothing
}


void fdcl::sensors::Swiftnav::load_config(fdcl::param &config_file) {
    config_file.read("GPS.on", on);
    config_file.read("GPS.port", port);
    config_file.read("GPS.baud_rate", baud_rate);
}


bool fdcl::sensors::Swiftnav::enabled(void) {
    return on;
}


void fdcl::sensors::Swiftnav::init_loop(void) {
    sbp_state_init(&s0);
    sbp_register_callback(&s0, SBP_MSG_HEARTBEAT, &heartbeat_callback_0, NULL, \
        &heartbeat_node_0);

    // create the primary sbp_state and register all the required callbacks
    sbp_state_init(&s);
    sbp_register_callback(&s, SBP_MSG_UTC_TIME, &gps_time_callback, NULL, \
        &gps_time_node); // 252
    sbp_register_callback(&s, SBP_MSG_POS_LLH, &pos_llh_callback, NULL, \
        &pos_llh_node); // 522
    sbp_register_callback(&s, SBP_MSG_VEL_NED, &vel_ned_callback, NULL, \
        &vel_ned_node); // 526
    sbp_register_callback(&s, SBP_MSG_BASELINE_NED, &baseline_callback, NULL, \
        &baseline_node); // 524
    sbp_register_callback(&s, SBP_MSG_HEARTBEAT, &heartbeat_callback, \
        NULL, &heartbeat_node); // 65535

    std::cout << "\nGPS: Waiting for the first heartbeat..." << std::endl;
    while (!flag_start) {
        sbp_process(&s0, &piksi_port_read);
    }
    std::cout << "GPS: Starting the main loop..." << std::endl;
}


void fdcl::sensors::Swiftnav::loop(void) {
    
    ret = sbp_process(&s, &piksi_port_read);
    if (ret < 0) {
        std::cout << "GPS: sbp_process error: " << (int)ret << std::endl;
    }

    usleep(1000); // sleep for 1 ms
}


void fdcl::sensors::Swiftnav::heartbeat_callback_0(u16 sender_id, u8 len, \
    u8 msg[], void *context) {

    (void) sender_id, (void)len, (void)msg, (void)context;
    std::cout << "GPS: first heartbeat detected" << std::endl;

    flag_start = 1;
}


void fdcl::sensors::Swiftnav::heartbeat_callback(u16 sender_id, u8 len, \
    u8 msg[], void *context) {
    (void)sender_id, (void)len, (void)msg, (void)context;
    // fprintf(stdout, "%s\n", __FUNCTION__);
}


void fdcl::sensors::Swiftnav::baseline_callback(u16 sender_id, u8 len, \
    u8 msg[], void *context) {
    (void)sender_id, (void)len, (void)msg, (void)context;
    // fprintf(stdout, "%s\n", __FUNCTION__);

    msg_baseline_ned_t baseline = *(msg_baseline_ned_t *)msg;
    piksi.n = (float) baseline.n / 1.0e3;
    piksi.e = (float) baseline.e / 1.0e3;
    piksi.d = (float) baseline.d / 1.0e3;

    // If the baseline.flags is zero, no RTK solution exists. Set the solution
    // flag to false and exit immediately.
    if ((int)baseline.flags == 0) {
        piksi.rtk_solution = false;
        return;
    }

    // If baseline.flags is non-zero, update the solution flag and variances.
    loop_count = 0;
    piksi.rtk_solution = true;

    piksi.status = (int)baseline.flags;
    piksi.S_rtk_x_h = (float)baseline.h_accuracy / 1.0e3;
    piksi.S_rtk_x_v = (float)baseline.v_accuracy / 1.0e3;
}


void fdcl::sensors::Swiftnav::pos_llh_callback(u16 sender_id, u8 len, \
    u8 msg[], void *context) {
    (void)sender_id, (void)len, (void)msg, (void)context;
    // fprintf(stdout, "%s\n", __FUNCTION__);

    msg_pos_llh_t pos_llh = *(msg_pos_llh_t *)msg;

    piksi.lat = pos_llh.lat;
    piksi.lon = pos_llh.lon;
    piksi.h = pos_llh.height;
    piksi.S_llh_h = (float) pos_llh.h_accuracy / 1.0e3;
    piksi.S_llh_v = (float) pos_llh.v_accuracy / 1.0e3;

    loop_count++;
    if (loop_count > 3) {
        piksi.rtk_solution = false;
    }

    // If an RTK solution does not exist, use LLH message to get the status of
    // the GPS receiver.
    if (!piksi.rtk_solution) {
        piksi.status = (int)pos_llh.flags;
    }
    
    piksi.sats = (int)pos_llh.n_sats;

    #if defined FDCL_ROVER
    rover.update_gps(piksi);
    #elif defined FDCL_BASE
    base.update_gps(piksi);
    #endif
}


void fdcl::sensors::Swiftnav::vel_ned_callback(u16 sender_id, u8 len, \
    u8 msg[], void *context) {
    (void)sender_id, (void)len, (void)msg, (void)context;
    // fprintf(stdout, "%s\n", __FUNCTION__);

    msg_vel_ned_t vel_ned = *(msg_vel_ned_t *)msg;

    piksi.v_n = (float) vel_ned.n / 1.0e3;
    piksi.v_e = (float) vel_ned.e / 1.0e3;
    piksi.v_d = (float) vel_ned.d / 1.0e3;

    piksi.S_rtk_v_h = (float) vel_ned.h_accuracy / 1.0e3;
    piksi.S_rtk_v_v = (float) vel_ned.v_accuracy / 1.0e3;
}


void fdcl::sensors::Swiftnav::gps_time_callback(u16 sender_id, u8 len, \
    u8 msg[], void *context) {
    (void)sender_id, (void)len, (void)msg, (void)context;
    // fprintf(stdout, "%s\n", __FUNCTION__);

    msg_utc_time_t gps_time = *(msg_utc_time_t *)msg;

    piksi.hr = gps_time.hours;
    piksi.min = gps_time.minutes;
    piksi.sec = gps_time.seconds;
    piksi.ms = gps_time.ns / 1.0e7;
    piksi.utc = 10000.0 * piksi.hr + 100.0 * piksi.min + piksi.sec \
        + piksi.ms / 100.0;
}


// u32 fdcl::sensors::Swiftnav::piksi_port_read(u8 *buff, u32 n, void *context) {
//     (void)context;
//     u32 result;

//     result = sp_blocking_read(piksi_port, buff, n, 0);

//     return result;
// }

s32 fdcl::sensors::Swiftnav::piksi_port_read(u8 *buff, u32 n, void *context) {
    (void)context;
    s32 result; // Change to s32

    result = sp_blocking_read(piksi_port, buff, n, 0);

    return static_cast<s32>(result); // Cast to s32 to ensure compatibility
}


void fdcl::sensors::Swiftnav::open(void) {
    int result = 0;

    // parse the args
    serial_port_name = port.c_str();
    ;
    unsigned int baud = baud_rate;

    std::cout << "GPS: Attempting to open " << serial_port_name
              << " with baud rate " << baud << " .."
              << std::endl;

    // check for serial port
    if (!serial_port_name) {
        std::cerr << "GPS: Check the serial port path of the Piksi!"
            << std::endl;
        exit(EXIT_FAILURE);
    }

    // check if Piksi can be detected
    result = sp_get_port_by_name(serial_port_name, &piksi_port);
    if (result != SP_OK) {
        std::cerr << "GPS: Cannot find provided serial port!" << std::endl;
        exit(EXIT_FAILURE);
    }

    // open Piksi for readings
    result = sp_open(piksi_port, SP_MODE_READ);
    if (result != SP_OK) {
        std::cerr << "GPS: Cannot open " << serial_port_name << " for reading!"
            << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "GPS: Port is open" << std::endl;

    setup_port(baud);
}


void fdcl::sensors::Swiftnav::setup_port(int baud) {
    /* set the serial port options for the Piksi */
    std::cout << "GPS: Attempting to configure the serial port..." << std::endl;

    int result;

    // set baud rate
    std::cout << "GPS: Configuring the baud rate..." << std::endl;
    result = sp_set_baudrate(piksi_port, baud);
    if (result != SP_OK) {
        std::cerr << "GPS: Cannot set port baud rate!" << std::endl;
        exit(EXIT_FAILURE);
    }

    // set flow control
    std::cout << "GPS: Configuring the flow control..." << std::endl;
    result = sp_set_flowcontrol(piksi_port, SP_FLOWCONTROL_NONE);
    if (result != SP_OK) {
        std::cerr << "GPS: Cannot set flow control!" << std::endl;
        exit(EXIT_FAILURE);
    }

    // set bit size
    std::cout << "GPS: Configuring the number of data bits..." << std::endl;
    result = sp_set_bits(piksi_port, 8);
    if (result != SP_OK) {
        std::cerr << "GPS: Cannot set data bits!" << std::endl;
        exit(EXIT_FAILURE);
    }

    // set parity
    std::cout << "GPS: Configuring the parity..." << std::endl;
    result = sp_set_parity(piksi_port, SP_PARITY_NONE);
    if (result != SP_OK) {
        std::cerr << "GPS: Cannot set parity!" << std::endl;
        exit(EXIT_FAILURE);
    }

    // set stop bits
    std::cout << "GPS: Configuring the number of stop bits..." << std::endl;
    result = sp_set_stopbits(piksi_port, 1);
    if (result != SP_OK) {
        std::cerr << "GPS: Cannot set stop bits!" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "GPS: Configuring serial port completed." << std::endl;
}


void fdcl::sensors::Swiftnav::close(void) {
    int result = sp_close(piksi_port);
    if (result != SP_OK) {
        std::cerr << "GPS: Cannot close " << serial_port_name << " properly!"
                  << std::endl;
    } else {
        std::cout << "GPS: Serial at " << serial_port_name << " port closed."
                  << std::endl;
    }

    // clean exit the serial ports
    sp_free_port(piksi_port);
    // free(serial_port_name);
}
