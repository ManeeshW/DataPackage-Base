/** \file system.hpp
*  \brief FDCL System class
*
* All the other classes are memebers of the system class. Files main_rover and
* main_base have instances of the system class.
*/

#ifndef FDCL_SYSTEM_HPP
#define FDCL_SYSTEM_HPP

#include "fdcl/clock.hpp"
#include "fdcl/param.hpp"
#include "fdcl/types/freq.hpp"
#include "fdcl/types/piksi.hpp"
#include "fdcl/types/vectors.hpp"

#include <atomic>
#include <mutex>


namespace fdcl {

class System {

public:
    std::atomic<bool> on;

    std::atomic<int> freq_imu;
    std::atomic<int> freq_gps;
    std::atomic<int> freq_ane1;
    std::atomic<int> freq_ane2;

    std::mutex clock_mutex;
    fdcl::Clock clock;

    std::mutex config_mutex;
    fdcl::param config;

    System(void);
    ~System(void);

    void init(void);
    void turn_off(void);

    void open_config_file(const std::string file_name);
    void close_config_file(void);

    void update_imu(const vector3 &ypr_in, const vector3 &a_in, \
        const vector3 &W_in);
    void update_gps(const piksi_t &gps_in);
    void update_ane1(const vector4 &ane_in);
    void update_ane2(const vector4 &ane_in);

    void get_imu(vector3 &ypr_out, vector3 &a_out, vector3 &W_out);
    void get_gps(piksi_t &gps_out);
    void get_ane1(vector4 &ane_out);
    void get_ane2(vector4 &ane_out);

private:
    std::mutex mutex_imu;
    vector3 ypr = vector3::Zero();
    vector3 a = vector3::Zero();
    vector3 W = vector3::Zero();

    std::mutex mutex_gps;
    piksi_t gps;

    std::mutex mutex_anemo1;
    vector4 ane1 = vector4::Zero();
    
    std::mutex mutex_anemo2;
    vector4 ane2 = vector4::Zero();

    freq_t freq_t_imu;
    freq_t freq_t_gps;
    freq_t freq_t_ane1;
    freq_t freq_t_ane2;
};

extern System rover;
extern System base;

void thread_key(void);
void thread_gui(void);
void thread_wifi(void);
void thread_logging(void);

// Sensor threads
void thread_gps(void);
void thread_base_imu(void);
void thread_rover_imu(void);
void thread_ane1(void);
void thread_ane2(void);

}  // end of namespace fdcl


#endif

