#include "fdcl/sensors/vn100.hpp"
#include "fdcl/system.hpp"


// used in configuring binary output register
using namespace vn::protocol::uart;


fdcl::sensors::Imu::Imu(void) {
    ;
}


fdcl::sensors::Imu::~Imu() {
    if (is_open) {
        close();
    }
}


bool fdcl::sensors::Imu::config_vn100_message(const int freq) {
    
    vn::protocol::uart::AsyncMode port_mode;
    if (port_number == 1) {
        port_mode = ASYNCMODE_PORT1;
    } else if (port_number == 2) {
        port_mode = ASYNCMODE_PORT2;
    } else {
        std::cerr <<  "IMU: port number is incorrectly set tp "
            << port_number
            << std::endl;
        return false;
    }

    vn::sensors::BinaryOutputRegister bor(
        port_mode,
        freq,
        COMMONGROUP_YAWPITCHROLL | COMMONGROUP_ANGULARRATE | COMMONGROUP_ACCEL,
        TIMEGROUP_NONE,
        IMUGROUP_NONE,
        GPSGROUP_NONE,
        ATTITUDEGROUP_NONE,
        INSGROUP_NONE);

    vs.writeBinaryOutput1(bor);

    vs.registerAsyncPacketReceivedHandler(NULL, parse_vn100_packet);

    return true;
}


void fdcl::sensors::Imu::parse_vn100_packet(
    void *userData, vn::protocol::uart::Packet &p, size_t index) {
    
    if (p.type() != vn::protocol::uart::Packet::TYPE_BINARY) {
        return;
    }
    
    check_binary_message_vn100(p);

    vn::math::vec3f ypr = p.extractVec3f();
    vn::math::vec3f ang_rate = p.extractVec3f();
    vn::math::vec3f accel = p.extractVec3f();

    vector3 YPR = vector3::Zero();
    vector3 W_i = vector3::Zero();
    vector3 a_i = vector3::Zero();
    matrix3 R_ni = matrix3::Zero();

    for (int i = 0; i < 3; i++) {
        YPR(i) = ypr[i] / 180.0 * M_PI;
        W_i(i) = ang_rate[i];
        a_i(i) = accel[i];
    }

    bool mag_declination = 0;
    YPR(0) = YPR(0) + mag_declination / 180.0 * M_PI;

    base.update_imu(YPR, a_i, W_i);
}


void fdcl::sensors::Imu::open(void) {
    open(port, baud_rate);
}


void fdcl::sensors::Imu::open(std::string port, int baud_rate) {
    
    // Create a VnSensor object and use it to connect to sensor.
    std::cout <<  "IMU: connecting to IMU at " << port << " .." << std::endl;
    vs.connect(port, baud_rate);
    std::string model_num = vs.readModelNumber();
    std::cout <<  "IMU: connected to " << model_num << std::endl;
    is_open = true;

    std::cout <<  "IMU: setting up binary output register .." << std::endl;
    vs.writeAsyncDataOutputType(VNOFF);  // disable ASCII output

    int freq = 4; // 200 Hz
    config_vn100_message(freq);
    
    std::cout <<  "IMU: starting sleep...\n" << std::endl;

}


void fdcl::sensors::Imu::close(void) {
    std::cout <<  "IMU: closing sensor .." << std::endl;
    vs.unregisterAsyncPacketReceivedHandler();

    int freq = 0;

    vn::protocol::uart::AsyncMode port_mode;
    if (port_number == 1) {
        port_mode = ASYNCMODE_PORT1;
    } else if (port_number == 2) {
        port_mode = ASYNCMODE_PORT2;
    } else {
        std::cerr << "IMU: port number is incorrectly set to "
                  << port_number
                  << std::endl;
    }

    vn::sensors::BinaryOutputRegister bor(
        port_mode,
        freq,
        COMMONGROUP_NONE,
        TIMEGROUP_NONE,
        IMUGROUP_NONE,
        GPSGROUP_NONE,
        ATTITUDEGROUP_NONE,
        INSGROUP_NONE);
    vs.writeBinaryOutput1(bor);
   
    std::cout <<  "IMU: disconnecting sensor .." << std::endl;
    vs.disconnect();

    is_open = false;
}


void fdcl::sensors::Imu::check_binary_message_vn100(
    vn::protocol::uart::Packet &p
) {
  
    // Make sure we have a binary packet type we expect
    if (!p.isCompatible(
            COMMONGROUP_YAWPITCHROLL | COMMONGROUP_ANGULARRATE,
            TIMEGROUP_NONE,
            IMUGROUP_NONE,
            GPSGROUP_NONE,
            ATTITUDEGROUP_LINEARACCELBODY,
            INSGROUP_NONE
            )
        ) {
        std::cout << "Wrong IMU binary packet... \n"
            << "not expecting this!"
            << std::endl;
        return;
    }
}


void fdcl::sensors::Imu::load_config(fdcl::param &config_file) {
    config_file.read("IMU.on", on);
    config_file.read("IMU.port", port);
    config_file.read("IMU.port_num", port_number);
    config_file.read("IMU.baud_rate", baud_rate);
    config_file.read("IMU.mag_declination", mag_declination);

    std::cout << "IMU: port: " << port << std::endl;
    std::cout << "IMU: port number: " << port_number << std::endl;
    std::cout << "IMU: baud_rate: " << baud_rate << std::endl;
}


void fdcl::sensors::Imu::loop(void) {
    vn::xplat::Thread::sleepMs(200);
}


bool fdcl::sensors::Imu::enabled(void) {
    return on;
}


double fdcl::sensors::Imu::get_time(void) {
    return clock.get_seconds();
}


void fdcl::sensors::Imu::update_dt_sync(const double t) {
    dt_sync = t;
}