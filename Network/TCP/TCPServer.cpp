//
// Created by 张程易 on 01/06/2017.
//

#include <netdb.h>
#include <cerrno>
#include <iostream>
#include <arpa/inet.h>
#include "TCPServer.h"

struct addrinfo * TCPServer::hint = nullptr;

TCPServer::TCPServer(const std::string &address, const std::string &port) {
    if( hint == nullptr){
        hint = (addrinfo *) malloc (sizeof(struct addrinfo));
        memset(hint, 0, sizeof(struct addrinfo));
        hint->ai_socktype = SOCK_STREAM; // allow tcp
        hint->ai_family = AF_UNSPEC; // allow 0.0.0.0
    }
    int ret = getaddrinfo(address.c_str(), port.c_str(), hint, &serverAddress);
    if( ret != 0){
        throw NetworkException(errno, "Error at `getaddrinfo`");
    }
}

TCPServer &TCPServer::listen() {
    struct addrinfo *addr_ptr = serverAddress;
    while (addr_ptr != nullptr) {
        this->socket = ::socket(AF_INET, SOCK_STREAM, 0);
        if (this->socket >= 0) break;
        addr_ptr = addr_ptr->ai_next;
    }
    if (this->socket < 0 || addr_ptr == nullptr) {
        this->socket = -1;
        throw NetworkException(errno, "Error at `socket`");
    }

    int opt = 1;
    if (::setsockopt(this->socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        throw NetworkException(errno, "Error at `setsockopt`");
    }
    socklen_t socklen = sizeof(struct sockaddr_in);

    if (::bind(this->socket, this->serverAddress->ai_addr, socklen) == -1) {
        throw NetworkException(errno, "Error at `bind`");
    }

    if (::listen(this->socket, 1024) < 0) {
        throw NetworkException(errno, "Error at `listen`");
    }
    FD_ZERO(&read_fs);
    struct sockaddr_in cliaddr;
    for(int i = 0; i < TCP_SERVER_CLIENT_MAX; i ++){
        client[i] = -1;
    }
    int maxfd = this->socket;
    while(true){
        FD_SET(this->socket, &read_fs);
        ::select(maxfd + 1, &read_fs, nullptr, nullptr, nullptr);
        if(FD_ISSET(this->socket, &read_fs)){
            int fd = ::accept(this->socket, (struct sockaddr*) &cliaddr, &socklen);
            if( fd < 0 ){
                throw NetworkException(errno, "Error at `accept`");
            }
            printf("a client connected: %s\n", inet_ntoa(cliaddr.sin_addr));
            FD_SET(fd, &read_fs);
            for(int i = 0; i < TCP_SERVER_CLIENT_MAX; i ++){
                if(client[i] == -1){
                    client[i] = fd;
                    break;
                }
            }
            if(fd > maxfd){
                maxfd = fd;
            }
        }
        else{
            for(int i = 0; i < TCP_SERVER_CLIENT_MAX; i ++){
                if(client[i] != -1 && FD_ISSET(client[i], &read_fs)){
                    ssize_t len = ::read(client[i], recvBuffer, sizeof(recvBuffer));
                    this->clientord = i;
                    for(int i = 0; i < len; i++) putchar(recvBuffer[i]);
                    if( len > 0 ){
                        printf("@#$");
                        if(this->onSuccess != nullptr){
                            this->onSuccess(*this, len);
                        }
                    }
                    else{
                        printf("$");
                        this->clientord = i;
                        if(this->onError != nullptr){
                            this->onError(*this);
                        }
                        FD_CLR(client[i], &read_fs);
                        client[i] = -1;
                    }
                }
            }
        }
    }
    return *this;
}

TCPServer &TCPServer::onRead(std::function<void(TCPServer &, int len)> onSuccess, std::function<void(TCPServer &)> onError) {
    this->onSuccess = onSuccess;
    this->onError = onError;
    return *this;
}

const char *TCPServer::getRecvBuffer() const {
    return recvBuffer;
}

TCPServer &TCPServer::write(const char *data, size_t len) {
    if(::write(this->client[this->clientord], data, len) < 0){
        throw NetworkException(errno, "error at write");
    }
    return *this;
}

TCPServer &TCPServer::disconnect() {
    this->client[this->clientord] = -1;
    FD_CLR(this->clientord,&this->read_fs);
    return * this;
}

int TCPServer::getClientord() const {
    return clientord;
}
