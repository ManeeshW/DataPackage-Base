#ifndef FDCL_ANEMO_HPP
#define FDCL_ANEMO_HPP

// user headers
#include "fdcl/param.hpp"
#include "fdcl/types/vectors.hpp"

// external libraries
#include "Eigen/Dense"
#include <libserialport.h>

// system headers
#include <iostream>
#include <unistd.h>


namespace fdcl
{
namespace sensors
{

/** \brief Read from Anemometers
*
*  This class reads from anemometers used for the ONR - Ship Air-Wake project.
*  Anemometers must be connected to the computer through an RS232-USB cable.
*  This class reads the string from the anemometers (-9999-9999-9999-9999)
*  format and parses them to numerical values.
*/
class Anemo
{
public:
    Anemo(int anemo_num_);
    ~Anemo(void);


    const char *serial_port_name = NULL;  /**< Name of the serial port for the
                                           * anemometer. This must be set
                                           * through the port.
                                           */

    struct sp_port *port; /**< Port structure for the serial port. Uses
                           * libserialport as the serial port.
                           */

    bool on = 0; /**< Boolean flag to make anemometer on/off. The anemometer
                  * thread will only continue is this is set to True.
                  */

    std::string port_name; /**< Name of the port of the anemometers */
    int baud_rate = 9600; /**< Baud rate of the port for the anemometers */


    /** \fn void load_config(void)
    * 	Loads the configurations for the anemometer from the config file.\n\n
    *  Configurations:\n
    * 	port_name: name of the serial port used by the anemometer\n
    *	baud_rate: baud rate used by the anemometer
    */
    void load_config(fdcl::param &config_file);


    /** \fn void open(void)
    *	Open the serial port for the anemometer for communication
    */
    void open(void);


    /** \fn void init_loop(void)
    *  Clear the input serial buffer at the beginning. This is to be called
    *  before calling the main loop.
    */
    void init_loop(void);


    /** \fn void loop(void)
    *	Read the string from the anemometer and parse it to integer values. This
    *	function includes the callback function for the fdcl_ekf class so that
    *	the SYS data is updated at the end of each loop.
    */
    void loop(void);


    /** \fn void close(void)
    *  Close the serial port opened for communicating with the anemometer.
    */
    void close(void);


private:
    int anemo_num = 1;  /** variable to define which anemometer an instantiation
                         * of the class referes to
                         */

    bool mini_mode = 0; /** whether to use mini anemometers or not */


    /** \fn void setup(int baud)
    * Setups the serial port for the anemometer with the recommended
    * configurations. This is called through the void open() function.
    */
    void setup_port(int baud);

    /** \fn void parse_mini_anemo(int (&ane_UVWT)[4], char byte_buff[])
     * Parses the mini anemometer readings
     * @param ane_UVWT  variable which the parses data is saved
     * @param byt_buff  buffer of bytes read from the sensor
     */
    void parse_mini_anemo(vector4 &ane_UVWT, char byte_buff[]);

    /** \fn void parse_anemo(int (&ane_UVWT)[4], char byte_buff[])
     * Parses the big anemometer readings
     * @param ane_UVWT  variable which the parses data is saved
     * @param byt_buff  buffer of bytes read from the sensor
     */
    void parse_anemo(int (&ane_UVWT)[4], char byte_buff[]);

};  // end of anemo class

}  // end of sensors namespace
}  // end of fdcl namespace

#endif
