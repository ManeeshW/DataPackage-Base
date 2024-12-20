#include "fdcl/wifi.hpp"


fdcl::Wifi::Wifi(void) {
    ;
}


fdcl::Wifi::~Wifi(void) {
    ;
}


void fdcl::Wifi::load_config(fdcl::param &config) {
    config.read("WIFI.server_ip_addr", server_ip_addr);
    config.read("WIFI.port", port);
}



int fdcl::Wifi::open_server(void) {
    return open_server(port);
}



int fdcl::Wifi::open_server(int port) {
    
    this->port = port;
    struct sockaddr_in serv_addr, cli_addr;

    std::cout << "WIFI: opening server socket" << std::endl;

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        std::cerr << "WIFI: ERROR: opening socket" << std::endl;
        return -1;
    }

    // reuse a port number
    int on = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
        std::cerr << "WIFI setsockopt failed" << std::endl;
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);
    if ( ::bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        std::cerr << "WIFI: binding failed" << std::endl;
        return -1;
    }

    listen(sockfd,5);

    unsigned int clilen = sizeof(cli_addr);

    std::cout << "WIFI: accepting" << std::endl;
    int newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

    if (newsockfd < 0) {
        std::cerr << "WIFI: accepting failed" << std::endl;
        return -1;
    }

    this->sockfd = newsockfd;

    return newsockfd;
}



int fdcl::Wifi::open_client() {
    return open_client(server_ip_addr, port);
}



int fdcl::Wifi::open_client(std::string server_ip_addr, int port) {
    this->server_ip_addr = server_ip_addr;
    this->port = port;

    struct sockaddr_in serv_addr;
    struct hostent *server;

    std::cout << "WIFI: opening client socket" << std::endl;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "WIFI: ERROR initializing socket" << std::endl;
        return -1;
    }
    server = gethostbyname(server_ip_addr.c_str());
    if (server == NULL) {
        std::cout << "WIFI: ERROR getting host by name" << std::endl;
        return -1;
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;

    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, 
            server->h_length);
    serv_addr.sin_port = htons(port);

    std::cout << "WIFI: client connecting" << std::endl;
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        std::cout << "WIFI: ERROR client connecting" << std::endl;
        return -1;
    }
    std::cout << "WIFI: client connected!" << std::endl;

    return sockfd;
}


int fdcl::Wifi::send(const void *buf, size_t len) {
    
    int bytes_sent;
    bytes_sent=::send(sockfd,buf,len,MSG_NOSIGNAL);
    
    if (bytes_sent < len) {
        std::cout << "WIFI: ERROR: bytes_sent " << bytes_sent 
            << " <  bytes_requested  " << len 
            << std::endl;
    }

    return bytes_sent;
}


int fdcl::Wifi::send_non_blocking(const void *buf, size_t len) {
    int bytes_sent;
    bytes_sent=::send(sockfd, buf, len, MSG_DONTWAIT);

    if (bytes_sent < len) {
        std::cout << "WIFI: ERROR: bytes_sent " << bytes_sent
            << " <  bytes_requested  " << len
            << std::endl;
    }

    return bytes_sent;
}


int fdcl::Wifi::send(fdcl::serial& buf) {
    return send(buf.data(), buf.size());
}


int fdcl::Wifi::send_non_blocking(fdcl::serial& buf) {
    return send_non_blocking(buf.data(), buf.size());
}


int fdcl::Wifi::recv(void *buf, size_t len) {
    int bytes_recv;
    if(len == 0) {
        bytes_recv = ::recv(sockfd, buf, sizeof(buf), 0);
    } else {
        bytes_recv = ::recv(sockfd, buf, len, 0);
    }

    if (bytes_recv < 0) {
        std::cerr << "WIFI: ERROR: fdcl::Wifi.recv: bytes_recv " 
            << bytes_recv << " < 0."
            << std::endl;
    }

    return bytes_recv;
}



int fdcl::Wifi::recv(size_t len) {
    if (len > MAX_BUFFER_RECV_SIZE) {
        std::cout << "WIFI: ERROR: fdcl::Wifi.recv: the requested receive data" 
            << " size is greater than MAX_BUFFER_SIZE_RECV, increase "
            << "MAX_BUFFER_SIZE_RECV in fdcl/Wifi.hpp" 
            << std::endl;
    }

    return recv(buf_recv, len);
}



int fdcl::Wifi::recv(fdcl::serial& fdcl_serial_recv_buf, size_t len) {
    int bytes_recv;
    bytes_recv = recv(len);
    fdcl_serial_recv_buf.clear();
    fdcl_serial_recv_buf.init(buf_recv, bytes_recv);

    return bytes_recv;
}