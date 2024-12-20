#include "fdcl/clock.hpp"
#include "fdcl/system.hpp"
#include "fdcl/wifi.hpp"

#include <unistd.h>


// D2F = double as float
// Sending doubles as float reduces the size of the payload by half, and 
// sufficient enough for most of the cases. If a higher accuracy is needed, you
// can simply send doubles as doubles, but remember to adjust the respective
// expected byte size.
#if defined FDCL_ROVER
#define BASE_TO_ROVER data_buffer.unpack
#define ROVER_TO_BASE data_buffer.pack
#define BASE_TO_ROVER_D2F data_buffer.unpack_as_double
#define ROVER_TO_BASE_D2F data_buffer.pack_as_float
#elif defined FDCL_BASE
#define BASE_TO_ROVER data_buffer.pack
#define ROVER_TO_BASE data_buffer.unpack
#define BASE_TO_ROVER_D2F data_buffer.pack_as_float
#define ROVER_TO_BASE_D2F data_buffer.unpack_as_double
#endif


namespace fdcl {
void rover_wifi_thread(void);
void base_wifi_thread(void);
void base_to_rover(fdcl::serial &data_buffer);
void rover_to_base(fdcl::serial &data_buffer);
}


void fdcl::thread_wifi(void) {
    #if defined FDCL_ROVER
    rover_wifi_thread();
    #elif defined FDCL_BASE
    base_wifi_thread();
    #endif
}


#if defined FDCL_ROVER
void fdcl::rover_wifi_thread(void) {
    std::cout << "WIFI: thread starting .." << std::endl;

    fdcl::Clock clock("wifi thread clock");
    fdcl::Wifi wifi;

    // Read configuration parameters.
    {
        std::lock_guard<std::mutex> guard(rover.config_mutex);
        wifi.load_config(rover.config);
    }


    usleep(1e6); // Wait for 1 second


    // Buffers to save received data and the data to be sent.
    fdcl::serial buf_send, buf_recv;  
    int bytes_sent, bytes_received;

    // This value must be same as the number of bytes sent by the base. If not,
    // the wifi thread might hang indefinitely.
    const int expected_bytes = 1;

	wifi.open_server();

    while(rover.on)
    {
        // First, wait for the base data.
        bytes_received = wifi.recv(buf_recv, expected_bytes);
        if (bytes_received == expected_bytes) {
            base_to_rover(buf_recv);
        }

        // If system is off, do not wait for the inputs from the rover.
        if (!rover.on) {
            break;
        }

        usleep(1000);

        // Now, send data to base.
        rover_to_base(buf_send);
        bytes_sent = wifi.send(buf_send);

        usleep(10000);  // 100 Hz
    }

    std::cout << "WIFI: thread closed!" << std::endl;
}
#endif


#if defined FDCL_BASE
void fdcl::base_wifi_thread(void) {
    std::cout << "WIFI: thread starting .." << std::endl;

    fdcl::Clock clock("wifi thread clock");
    fdcl::Wifi wifi;

    // Read configuration parameters.
    {
        std::lock_guard<std::mutex> guard(base.config_mutex);
        wifi.load_config(base.config);
    }


    usleep(1e6); // Wait for 1 second


    // Buffers to save received data and the data to be sent.
    fdcl::serial buf_send, buf_recv;  
    int bytes_sent, bytes_received;

    // This value must be same as the number of bytes sent by the rover. If not,
    // the wifi thread might hang indefinitely.
    const int expected_bytes = 120;

    wifi.open_client();

    while(base.on) {

        // First sent data to rover.
        base_to_rover(buf_send);
        bytes_sent = wifi.send(buf_send);

        // If system is off, do not wait for the inputs from the rover.
        if (!base.on) {
            break;
        }

        usleep(1000);

        // Receive data from the from the rover.
        bytes_received = wifi.recv(buf_recv, expected_bytes);
        if (bytes_received == expected_bytes) {
            rover_to_base(buf_recv);
        }

        usleep(10000);  // 100 Hz
    }


    // Send the system states to rover to make sure that the base gets the
    // system off signal. This is required to not to make rover wifi thread
    // wait indefinitely.
    std::cout << "WIFI: sending system off .." << std::endl;
    for (int i = 0; i < 50; i++)
    {
        base_to_rover(buf_send);
        bytes_sent = wifi.send_non_blocking(buf_send);
        usleep(10000); // wait 10 milliseconds
    }

    std::cout << "WIFI: thread closed!" << std::endl;
}
#endif


void fdcl::base_to_rover(fdcl::serial &data_buffer)
{
    bool system_on;

    #if defined FDCL_BASE
    data_buffer.clear();
    #endif

    // Atomic variables need to be converted.
    system_on = base.on;
    
    // If the following lines are changed, the expected bytes at rover must be
    // updated manually.
    // NOTE: bool = 1, int = 2, float = 4, double = 8.

    BASE_TO_ROVER(system_on);


    #if defined FDCL_ROVER
    // Atomic variables need to be converted.
    rover.on = system_on;

    data_buffer.clear();
    #endif

    // std::cout << "Send buffer size: \n" << data_buffer.size() << std::endl;
}


void fdcl::rover_to_base(fdcl::serial &data_buffer) {

    vector3 ypr_i, a_i, W_i;

    piksi_t piksi;
    vector3 x_g, v_g, llh;
    int num_sats;
    int status = 0;
    float utc;

    vector4 ane1, ane2;

    int freq_imu = 0;
    int freq_gps = 0;
    int freq_ane1 = 0;
    int freq_ane2 = 0;

    #if defined FDCL_ROVER
    data_buffer.clear();
    rover.get_imu(ypr_i, a_i, W_i);
    
    rover.get_gps(piksi);
    x_g << piksi.n, piksi.e, piksi.d;
    v_g << piksi.v_n, piksi.v_e, piksi.v_d;
    llh << piksi.lat, piksi.lon, piksi.h;
    num_sats = piksi.sats;
    status = piksi.status;
    utc = piksi.utc;

    rover.get_ane1(ane1);
    rover.get_ane2(ane2);

    freq_imu = rover.freq_imu;
    freq_gps = rover.freq_gps;
    freq_ane1 = rover.freq_ane1;
    freq_ane2 = rover.freq_ane2;

    #endif

    // If the following lines are changed, the expected bytes at rover must be
    // updated manually.
    // NOTE: bool = 1, int = 2, float = 4, double = 8.

    ROVER_TO_BASE_D2F(ypr_i);
    ROVER_TO_BASE_D2F(a_i);
    ROVER_TO_BASE_D2F(W_i);

    ROVER_TO_BASE_D2F(x_g);
    ROVER_TO_BASE_D2F(v_g);
    ROVER_TO_BASE_D2F(llh);
    ROVER_TO_BASE(status);
    ROVER_TO_BASE(num_sats);
    ROVER_TO_BASE(utc);

    ROVER_TO_BASE_D2F(ane1);
    ROVER_TO_BASE_D2F(ane2);

    ROVER_TO_BASE(freq_imu);
    ROVER_TO_BASE(freq_gps);
    ROVER_TO_BASE(freq_ane1);
    ROVER_TO_BASE(freq_ane2);


    #if defined FDCL_BASE

    // Assign received values from the rover to the rover object in the base.
    rover.update_imu(ypr_i, a_i, W_i);

    piksi.n = x_g(0);
    piksi.e = x_g(1);
    piksi.d = x_g(2);

    piksi.v_n = v_g(0);
    piksi.v_e = v_g(1);
    piksi.v_d = v_g(2);

    piksi.lat = llh(0);
    piksi.lon = llh(1);
    piksi.h = llh(2);

    piksi.sats = num_sats;
    piksi.status = status;
    piksi.utc = utc;

    rover.update_gps(piksi);

    rover.update_ane1(ane1);
    rover.update_ane2(ane2);

    // Frequency unpack must be done at the end.
    rover.freq_imu = freq_imu;
    rover.freq_gps = freq_gps;
    rover.freq_ane1 = freq_ane1;
    rover.freq_ane2 = freq_ane2;

    data_buffer.clear();
    #endif

    // std::cout << "Send buffer size: \n" << data_buffer.size() << std::endl;
}
