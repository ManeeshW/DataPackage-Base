#include "fdcl/clock.hpp"
#include "fdcl/save.hpp"
#include "fdcl/system.hpp"

#include <unistd.h>


namespace fdcl {

void write_header(fdcl::save &log);
void write_data(fdcl::save &log, double t, const std::string &t_os);

std::string get_string_3x1(std::string var_name);
std::string get_string_4x1(std::string var_name);
}


void fdcl::thread_logging(void) {
    std::cout << "LOG: thread started" << std::endl;

    fdcl::Clock clock("log thread clock");
    
    std::string log_file_name = "log_" + clock.get_date_time() + ".txt";

    std::cout << "LOG: opening file " << log_file_name << std::endl;
    fdcl::save log;
    log.open(log_file_name);
    bool file_open = true;

    // Write the header row.
    write_header(log);

    while (base.on) {
        write_data(log, clock.get_seconds(), clock.get_system_time());
        usleep(50e3);  // Limit max frequency to 20 Hz
    }

    log.close();

    std::cout << "LOG: thread closed" << std::endl;
}


void fdcl::write_header(fdcl::save &log)
{
    log.write("t");
    log.write("t_system");

    log.write(get_string_3x1("base_ypr"));
    log.write(get_string_3x1("base_a"));
    log.write(get_string_3x1("base_W"));

    log.write(get_string_3x1("base_llh"));
    log.write("base_utc");
    log.write("base_status");
    log.write("base_sats");

    log.write(get_string_4x1("base_ane"));


    log.write(get_string_3x1("ypr"));
    log.write(get_string_3x1("a"));
    log.write(get_string_3x1("W"));

    log.write(get_string_3x1("rtk_x"));
    log.write(get_string_3x1("rtk_v"));
    log.write(get_string_3x1("llh"));
    log.write("utc");
    log.write("status");
    log.write("sats");

    log.write(get_string_4x1("ane1"));
    log.write(get_string_4x1("ane2"));

    log.endl();
}


void fdcl::write_data(fdcl::save &log, double t, const std::string &t_os)
{
    // NOTE: before changing order, adding or removing data, make sure that
    // headers in write_base_header ara analogous to the below data

    vector3 base_ypr, base_a, base_W;

    piksi_t base_piksi;
    vector3 base_x, base_v, base_llh;
    int base_num_sats;
    int base_status;
    float base_utc;

    vector4 base_ane;

    vector3 ypr, a, W;

    piksi_t piksi;
    vector3 x, v, llh;
    int num_sats;
    int status;
    float utc;

    vector4 ane1;
    vector4 ane2;

    base.get_imu(base_ypr, base_a, base_W);

    base.get_gps(base_piksi);
    base_x << base_piksi.n, base_piksi.e, base_piksi.d;
    base_v << base_piksi.v_n, base_piksi.v_e, base_piksi.v_d;
    base_llh << base_piksi.lat, base_piksi.lon, base_piksi.h;
    num_sats = base_piksi.sats;
    status = base_piksi.status;
    utc = base_piksi.utc;

    base.get_ane1(base_ane);

    rover.get_imu(ypr, a, W);

    rover.get_gps(piksi);
    x << piksi.n, piksi.e, piksi.d;
    v << piksi.v_n, piksi.v_e, piksi.v_d;
    llh << piksi.lat, piksi.lon, piksi.h;
    num_sats = piksi.sats;
    status = piksi.status;
    utc = piksi.utc;

    rover.get_ane1(ane1);
    rover.get_ane1(ane2);


    log.write(t);
    log.write(t_os);

    log.write(base_ypr);
    log.write(base_a);
    log.write(base_W);

    log.write(base_llh);
    log.write(base_utc);
    log.write(base_status);
    log.write(base_num_sats);

    log.write(base_ane);

    log.write(ypr);
    log.write(a);
    log.write(W);

    log.write(x);
    log.write(v);
    log.write(llh);
    log.write(utc);
    log.write(status);
    log.write(num_sats);

    log.write(ane1);
    log.write(ane2);

    log.endl();
}


std::string fdcl::get_string_3x1(std::string var_name) {
    return var_name + "_0, " + var_name + "_1, " + var_name + "_2";
}


std::string fdcl::get_string_4x1(std::string var_name) {
    return var_name + "_0, " + var_name + "_1, " + var_name + "_2," \
        + var_name + "_3";
}