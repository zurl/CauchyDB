//
// Created by 张程易 on 03/06/2017.
//

#ifndef MEOW_HTTPRESPONSEHEADER_H
#define MEOW_HTTPRESPONSEHEADER_H

#include "HTTPHeader.h"
#include "../../Common.h"

const char * HTTP_VERSION = "HTTP/1.1";
const char * ENGINE_INFO = "Meow";

class HTTPResponseHeader : public HTTPHeader{
    int code = 200;
    std::string message = "OK";
public:
    HTTPResponseHeader() {
        HTTPHeader::setHeader("Server", ENGINE_INFO);
    }

    int getCode() const {
        return code;
    }

    void setCode(int code) {
        HTTPResponseHeader::code = code;
    }

    const std::string &getMessage() const {
        return message;
    }

    void setMessage(const std::string &message) {
        HTTPResponseHeader::message = message;
    }

    std::string toString(){
        std::string result = HTTP_VERSION + zcy::itos(code) + " " + message  + "\r\n";
        for(auto & header : headers){
            result += header.first + ": " + header.second + "\r\n";
        }
        return result;
    }

};

#endif //MEOW_HTTPRESPONSEHEADER_H
