#include "fdcl/system.hpp"

namespace fdcl {
fdcl::System rover;
fdcl::System base;
}

fdcl::System::System(void) {
    ;
}


fdcl::System::~System(void) {
    ;
}


void fdcl::System::init(void) {
    on = true;
    freq_imu = 0;
    freq_gps = 0;
    freq_ane1 = 0;
    freq_ane2 = 0;
    clock.reset();
}



void fdcl::System::turn_off(void) {
    on = false;
}


void fdcl::System::open_config_file(const std::string file_name) {
    config.open(file_name);
}


void fdcl::System::close_config_file(void) {
    config.close();
}


void fdcl::System::update_imu(const vector3 &ypr_in, const vector3 &a_in, \
        const vector3 &W_in) {
    
    freq_imu = freq_t_imu.calculate_freq(clock.get_seconds());

    std::lock_guard<std::mutex> guard(mutex_imu);
    ypr = ypr_in;
    a = a_in;
    W = W_in;
}


void fdcl::System::update_gps(const piksi_t &gps_in) {

    freq_gps = freq_t_gps.calculate_freq(clock.get_seconds());

    std::lock_guard<std::mutex> guard(mutex_gps);
    gps = gps_in;
}


void fdcl::System::update_ane1(const vector4 &ane_in) {

    freq_ane1 = freq_t_ane1.calculate_freq(clock.get_seconds());

    std::lock_guard<std::mutex> guard(mutex_anemo1);
    ane1 = ane_in;
}


void fdcl::System::update_ane2(const vector4 &ane_in) {

    freq_ane2 = freq_t_ane2.calculate_freq(clock.get_seconds());

    std::lock_guard<std::mutex> guard(mutex_anemo2);
    ane2 = ane_in;
}


void fdcl::System::get_imu(vector3 &ypr_out, vector3 &a_out, vector3 &W_out) {
    std::lock_guard<std::mutex> guard(mutex_imu);
    ypr_out = ypr;
    a_out = a;
    W_out = W;
}


void fdcl::System::get_gps(piksi_t &gps_out) {
    std::lock_guard<std::mutex> guard(mutex_imu);
    gps_out = gps;
}


void fdcl::System::get_ane1(vector4 &ane_out) {
    std::lock_guard<std::mutex> guard(mutex_anemo1);
    ane_out = ane1;
}


void fdcl::System::get_ane2(vector4 &ane_out) {
    std::lock_guard<std::mutex> guard(mutex_anemo2);
    ane_out = ane2;
}

