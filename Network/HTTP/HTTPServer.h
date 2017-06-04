//
// Created by 张程易 on 02/06/2017.
//

#ifndef MEOW_HTTPSERVER_H
#define MEOW_HTTPSERVER_H


#include "HTTPRequest.h"
#include "HTTPContext.h"
#include "../TCP/TCPServer.h"

class HTTPServer {
    TCPServer tcpServer;
    std::map<int, HTTPContext> ctx;
    std::function<void(HTTPContext &)> onSuccess = nullptr;
public:
    HTTPServer(const std::string &address, const std::string &port)
            : tcpServer(address, port) {
        tcpServer.onRead([this](TCPServer & tcpServer, int len){
            printf("#");
            int ord = tcpServer.getClientord();
            auto iter = ctx.find(ord);
            if( iter == ctx.end()){
                ctx.emplace(std::piecewise_construct,
                            std::forward_as_tuple(ord),
                            std::forward_as_tuple());
                iter = ctx.find(ord);
            }
            if(iter->second.append(tcpServer.getRecvBuffer(), len)){
                if(onSuccess != nullptr){
                    this->onSuccess(iter->second);
                    std::string res = iter->second.getResponse()->toString();
                    tcpServer.write(res.c_str(), res.length());
                    iter->second.reset();
                }
            }
        }, [this](TCPServer & tcpServer){
            int ord = tcpServer.getClientord();
            auto iter = ctx.find(ord);
            if(iter != ctx.end()){
                iter->second.reset();
            }
        });
    }
    HTTPServer & onRecv(std::function<void(HTTPContext &)> onSuccess){
        this->onSuccess = onSuccess;
    }
    HTTPServer & listen(){
        tcpServer.listen();
        return *this;
    }
};



#endif //MEOW_HTTPSERVER_H
