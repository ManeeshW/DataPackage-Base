#include "fdcl/sensors/anemometer.hpp"
#include "fdcl/system.hpp"


fdcl::sensors::Anemo::Anemo(int anemo_num_) : anemo_num(anemo_num_) {
    ;  // do nothing
}


fdcl::sensors::Anemo::~Anemo(void) {
    ;  // do nothing
}


void fdcl::sensors::Anemo::load_config(fdcl::param &config_file) {
    config_file.read("ANEMO.ane" + std::to_string(anemo_num) + "_on", on);
    config_file.read("ANEMO.ane" + std::to_string(anemo_num) + "_port_name", \
        port_name);
    config_file.read("ANEMO.ane"  + std::to_string(anemo_num) + "_baud_rate", \
        baud_rate);
    config_file.read("ANEMO.mini_mode", mini_mode);
}


void fdcl::sensors::Anemo::open(void) {
    int result = 0;

    // parse the args
    serial_port_name = port_name.c_str();;
    unsigned int baud = baud_rate;
    std::cout << "ANEMO : " << anemo_num << " " << serial_port_name
              << std::endl;

    std::cout << "ANE: Attempting to open " << serial_port_name
              << " with baud rate " << baud << "..."
              << std::endl;

    // check for serial port
    if (!serial_port_name) {
        std::cerr << "ANE: Check the serial port path!" << std::endl;
        exit(EXIT_FAILURE);
    }

    // check if anemo can be detected
    result = sp_get_port_by_name(serial_port_name, &port);
    if (result != SP_OK) {
        std::cerr << "ANE: Cannot find provided serial port!" << std::endl;
        exit(EXIT_FAILURE);
    }

    // open anemo for readings
    result = sp_open(port, SP_MODE_READ);
    if (result != SP_OK) {
        std::cerr << "ANE: Cannot open " << serial_port_name << " for reading!"
                  << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "ANE: Port is open" << std::endl;

    setup_port(baud);
}


void fdcl::sensors::Anemo::setup_port(int baud) {
    /* set the serial port options for the anemo */
    std::cout << "ANE: Attempting to configure the serial port..." << std::endl;

    int result;

    // set baud rate
    result = sp_set_baudrate(port, baud);
    if (result != SP_OK)
    {
        std::cerr << "ANE: Cannot set port baud rate!" << std::endl;
        exit(EXIT_FAILURE);
    }

    // set flow control
    result = sp_set_flowcontrol(port, SP_FLOWCONTROL_NONE);
    if (result != SP_OK)
    {
        std::cerr << "ANE: Cannot set flow control!" << std::endl;
        exit(EXIT_FAILURE);
    }

    // set bit size
    result = sp_set_bits(port, 7);
    if (result != SP_OK)
    {
        std::cerr << "ANE: Cannot set data bits!" << std::endl;
        exit(EXIT_FAILURE);
    }

    // set parity
    result = sp_set_parity(port, SP_PARITY_EVEN);
    if (result != SP_OK)
    {
        std::cerr << "ANE: Cannot set parity!" << std::endl;
        exit(EXIT_FAILURE);
    }

    // set stop bits
    result = sp_set_stopbits(port, 1);
    if (result != SP_OK)
    {
        std::cerr << "ANE: Cannot set stop bits!" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "ANE: Configuring serial port completed." << std::endl;
}


void fdcl::sensors::Anemo::close(void) {
    int result = sp_close(port);
    if (result != SP_OK)
    {
        std::cerr << "ANE: Cannot close " << serial_port_name
                  << " properly!" << std::endl;
    }
    else
    {
        std::cout << "ANE: Serial at " << serial_port_name
                  << " port closed." << std::endl;
    }

    // clean exit the serial ports
    sp_free_port(port);
}


void fdcl::sensors::Anemo::init_loop(void) {
    char clear_buff[1];
    int byte_num = 0;

    std::cout << "ANE: Clearing buffer.." << std::endl;
    while(true) {
        byte_num = sp_blocking_read(port, clear_buff, 1, 0);
        if (clear_buff[0] == '\n') break;
    }
}


void fdcl::sensors::Anemo::loop(void) {

    int buff_len;
    if (mini_mode) {
        buff_len = 28;
    } else {
        buff_len = 22;
    }

    char byte_buff[buff_len];
    char byte_in[1];
    int pos = 0;
    int byte_num = 0;

    // read anemometer data until newline character
    while (true) {
        int bytes_waiting = sp_input_waiting(port);
        if (bytes_waiting > 0) {
            byte_num = sp_blocking_read_next(port, byte_in, 1, 0);
            byte_buff[pos] = byte_in[0];

            // std::cout << byte_in[0];

            if (byte_buff[pos] == '\n') {
                break;
            }

            pos++;
            
            if (pos >= buff_len) {
                break;
            }
        }
    }

    vector4 ane_data;
    if (pos >= buff_len) {
        // split anemometer string to U, V, W and Temp data
        if (mini_mode) {
            parse_mini_anemo(ane_data, byte_buff);
        } else {
            int ane_UVWT[4] = {0, 0, 0, 0};
            parse_anemo(ane_UVWT, byte_buff);
            ane_data << ane_UVWT[0], ane_UVWT[1], ane_UVWT[2], ane_UVWT[3];
        }

        // std::cout << ane_data.transpose() << "\n";

        #ifdef FDCL_ROVER
        if (anemo_num == 1) {
            rover.update_ane1(ane_data);
        } else if (anemo_num == 2) {
            rover.update_ane2(ane_data);
        }
        #elif FDCL_BASE
        base.update_ane1(ane_data);
        #endif

    }
}


void fdcl::sensors::Anemo::parse_mini_anemo(vector4 &ane_UVWT, \
    char byte_buff[]) {

    for (int i = 0; i < 4; i++) {
        char data[6];
        
        for (int j = 0; j < 7; j++) {
            data[j] = byte_buff[i*7 + j];
        }

        try {
            ane_UVWT(i) = atof(data);
        } catch (int e) {}
    }
}


void fdcl::sensors::Anemo::parse_anemo(int (&ane_UVWT)[4], char byte_buff[])
{
    for (int i = 0; i < 4; i++)
    {
        char data[4];
        for (int j = 0; j < 5; j++) data[j] = byte_buff[i*5 + j];

        try
        {
            ane_UVWT[i] = atoi(data);
        }
        catch (int e) {}
    }
}
