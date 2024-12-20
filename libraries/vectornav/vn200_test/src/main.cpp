#include <iomanip>
#include <iostream>
#include <limits>
#include <bits/stdc++.h> 

// Include this header file to get access to VectorNav sensors.
#include "vn/sensors.h"

// We need this file for our sleep function.
#include "vn/thread.h"

using namespace std;
using namespace vn::math;
using namespace vn::sensors;
using namespace vn::protocol::uart;
using namespace vn::xplat;


// Method declarations for future use.
void asciiOrBinaryAsyncMessageReceived(void* userData, Packet& p, size_t index);


int main(int argc, char *argv[])
{
	// This example walks through using the VectorNav C++ Library to connect to
	// and interact with a VectorNav sensor.

	// First determine which COM port your sensor is attached to and update the
	// constant below. Also, if you have changed your sensor from the factory
	// default baudrate of 115200, you will need to update the baudrate
	// constant below as well.
	// const string port = "COM1";
	const string port = "/dev/ttyTHS2";
	// const string port = "/dev/ttyUSB0";
	// const string port = "/dev/tty.usbserial-A904RS9S";
	// const string portm = "/dev/ttyS0";
	const uint32_t baud_rate = 230400;

	VnSensor vs;
	cout << "Connecting to IMU at " << port << " .." << endl;
	vs.connect(port, baud_rate);

	string mn = vs.readModelNumber();
	cout << "Model Number: " << mn << endl;

	// Get some orientation data from the sensor.
	vec3f ypr = vs.readYawPitchRoll();
	cout << "Current YPR: " << ypr << endl;

	cout << "Setting up binary output register .." << endl;
	vs.writeAsyncDataOutputType(VNOFF); // disable ASCII output

	// Unregister our callback method.
	// vs.unregisterAsyncPacketReceivedHandler();

	BinaryOutputRegister bor(
		ASYNCMODE_PORT2,
		80, // 10 Hz
		COMMONGROUP_TIMESTARTUP | COMMONGROUP_YAWPITCHROLL | \
		COMMONGROUP_ANGULARRATE,
		TIMEGROUP_NONE,
		IMUGROUP_NONE,
		GPSGROUP_NUMSATS,
		ATTITUDEGROUP_LINEARACCELBODY,
		INSGROUP_INSSTATUS | INSGROUP_POSLLA | INSGROUP_VELNED | \
		INSGROUP_POSU | INSGROUP_VELU);
	vs.writeBinaryOutput1(bor);

	vs.registerAsyncPacketReceivedHandler(NULL,
		asciiOrBinaryAsyncMessageReceived);
	
	cout << "Starting sleep..." << endl;
	Thread::sleepSec(30);
	
	vs.unregisterAsyncPacketReceivedHandler();
	vs.disconnect();

	return 0;
}


void asciiOrBinaryAsyncMessageReceived(void* userData, Packet& p, size_t index)
{
	if (p.type() == Packet::TYPE_ASCII && p.determineAsciiAsyncType() == VNYPR)
	{
		vec3f ypr;
		p.parseVNYPR(&ypr);
		cout << "ASCII Async YPR: " << ypr << endl;
		return;
	}

	if (p.type() == Packet::TYPE_BINARY)
	{
		// First make sure we have a binary packet type we expect since there
		// are many types of binary output types that can be configured.
		if (!p.isCompatible(
			COMMONGROUP_TIMESTARTUP | COMMONGROUP_YAWPITCHROLL | \
			COMMONGROUP_ANGULARRATE,
			TIMEGROUP_NONE,
			IMUGROUP_NONE,
			GPSGROUP_NUMSATS,
			ATTITUDEGROUP_LINEARACCELBODY,
			INSGROUP_INSSTATUS | INSGROUP_POSLLA | INSGROUP_VELNED | \
			INSGROUP_POSU | INSGROUP_VELU))
			// Not the type of binary packet we are expecting.
			{
				std::cout << "Wrong packet, dropping the data .."  << std::endl;
				return;
			}

		// Ok, we have our expected binary output packet. Since there are many
		// ways to configure the binary data output, the burden is on the user
		// to correctly parse the binary packet. However, we can make use of
		// the parsing convenience methods provided by the Packet structure.
		// When using these convenience methods, you have to extract them in
		// the order they are organized in the binary packet per the User Manual.
		uint64_t timeStartup = p.extractUint64();
		vec3f ypr = p.extractVec3f();
		vec3f ang_rate = p.extractVec3f();
		vec3f accel = p.extractVec3f();
		uint8_t num_sats = p.extractUint8();
		uint16_t status = p.extractUint16();
		std::bitset<16> ins_status(status);

		// for (int i = 0; i < 16; i++) std::cout << ins_status[i] << ",";
		// std::cout << std::endl;

		// INS Status
		// 0 - Not tracking, initializing
		// 1 - Aligning
		// 2 - Tracking
		// 3 - Loss of GPS for more than 45 seconds
		std::bitset<2> mode_bits;
		mode_bits[0] = ins_status[0];
		mode_bits[1] = ins_status[1];
		unsigned int mode = mode_bits.to_ulong();


		// GPS Fix
		// 0 - No fix
		// 1 - Only UTC
		// 2 - 2D fix
		// 3 - 3D fix
		// NOTE: Reading GPS fix from the INS status only provides whether a 
		// GPS fix is available or not.
		// To check whether the fix is 2D/3D, needs to read GPS registers, 
		// which is not implemented here.
		// Current implementation:
		// 0 - No fix
		// 1 - Fix available
		unsigned int gps_fix = ins_status[2];

		cout << timeStartup << "," << ins_status;
		cout << "," << mode << "," << gps_fix;  // << sensor_error;
		cout << "," << ypr[0] << "," << ypr[1] << "," << ypr[2] << ",";

		if (gps_fix)
		{
			vec3d pos_lla = p.extractVec3d();
			vec3f vel_ned = p.extractVec3f();
			float pos_u = p.extractFloat();
			float vel_u = p.extractFloat();

			cout << "," 
				<< std::setprecision(
					std::numeric_limits<long double>::digits10 + 1) 
				<< "," << pos_lla[0] << "," << pos_lla[1] << "," << pos_lla[2]
				<< "," << vel_ned[0] << "," << vel_ned[1] << "," << vel_ned[2]
				<< "," << pos_u << "," << vel_u;
		}

		cout << endl;
	}
}
