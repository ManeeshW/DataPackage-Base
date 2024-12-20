/** \file Wifi.hpp
*  \brief FDCL WIFI class (documentation incomplete)
*/

#ifndef FDCL_WIFI_HPP
#define FDCL_WIFI_HPP

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <strings.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>

#include "fdcl/param.hpp"
#include "fdcl/serial.hpp"


// do not request if MSG_NOSIGNAL is not defined
// it is usually defined for linux, but not on mac
#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0x0
#endif

// do not request if MSG_DONTWAIT is not defined
// it is usually defined for linux, but not on mac
#ifndef MSG_DONTWAIT
#define MSG_DONTWAIT 0x40
#endif

#define MAX_BUFFER_RECV_SIZE 8192


namespace fdcl {

/** \brief Establish Wi-Fi communication between two the base and the rover.
 *
 * Establish a server-client communication line between the base and the rover
 * to send and receive data on both ends.
 */
class Wifi {
public:
    Wifi(void);
    ~Wifi(void);

    /** \fn void load_config(void)
     * Loads the class parameters from the config file.
     */
    void load_config(fdcl::param &config);


    /** \fn void load_config(void)
     * Loads the class parameters from the config file.
     */
    int open_server(void);
    int open_client(void);

    int send(fdcl::serial& buf_send);
    int send_non_blocking(fdcl::serial& buf_send);
    int recv(fdcl::serial& buf_recv, size_t len);

private:
    int sockfd;
    int port;
    std::string server_ip_addr;
    unsigned char buf_recv[MAX_BUFFER_RECV_SIZE];

    int open_server(int port);
    int open_client(std::string server_ip_addr, int port);

    int recv(size_t len);
    int recv(void *buf, size_t len);

    int send(const void *buf, size_t len);
    int send_non_blocking(const void *buf, size_t len);
};

}  // end of namespace fdcl

#endif
