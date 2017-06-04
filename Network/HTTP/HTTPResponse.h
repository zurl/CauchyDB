//
// Created by 张程易 on 03/06/2017.
//

#ifndef MEOW_HTTPRESPONSE_H
#define MEOW_HTTPRESPONSE_H

#include "HTTPResponseHeader.h"

class HTTPResponse{
    HTTPResponseHeader header;
    std::string body;
public:
    HTTPResponse() {

    }
    void appendBody(const std::string & content) {
        body += content;
    }

    std::string toString(){
        header.setHeader("Content-Length", zcy::itos(body.length()));
        std::string result;
        result += header.toString() + "\r\n";
        result += body;
        return result;
    }

    const HTTPResponseHeader &getHeader() const {
        return header;
    }

};

#endif //MEOW_HTTPRESPONSE_H
