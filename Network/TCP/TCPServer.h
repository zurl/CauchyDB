//
// Created by 张程易 on 01/06/2017.
//

#ifndef MEOW_TCPSERVER_H
#define MEOW_TCPSERVER_H
#include <functional>
#include <cstdlib>
#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <string>

#include "../../Common.h"

const int TCP_SERVER_BUFFER_SIZE = 4096;
const int TCP_SERVER_CLIENT_MAX = 256;

class TCPServer {
    static struct addrinfo * hint;
    struct addrinfo * serverAddress;
    int socket;
    int clientord;
    char recvBuffer[TCP_SERVER_BUFFER_SIZE];
    fd_set read_fs;
    int client[TCP_SERVER_CLIENT_MAX];
    std::function<void(TCPServer &, int len)> onSuccess;
    std::function<void(TCPServer &)> onError;
public:
    TCPServer(const std::string &address, const std::string &port);
    TCPServer & onRead(std::function<void(TCPServer &, int len)> onSuccess, std::function<void(TCPServer &)> onError= nullptr);
    TCPServer & listen();
    TCPServer & write(const char * data, size_t len);
    TCPServer & disconnect();
    const char *getRecvBuffer() const;
    int getClientord() const;
};


#endif //MEOW_TCPSERVER_H
