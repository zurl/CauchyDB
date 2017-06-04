//
// Created by 张程易 on 03/06/2017.
//

#ifndef MEOW_HTTPREQUEST_H
#define MEOW_HTTPREQUEST_H

#include "HTTPRequestHeader.h"
#include "../../Common.h"

class HTTPRequest{
    HTTPRequestHeader header;
    int bodyLen;
    int nowLen;
    char * body;
public:
    HTTPRequest(const char * header) : header(header) {
        if(this->header.hasHeader("Content-Length")){
            bodyLen = Util::stoi(this->header.getHeader("Content-Length"));
        }else{
            bodyLen = 0;
        }
        body = new char[bodyLen];
        nowLen = 0;
    }
    bool appendBody(const char * buffer, int len){
        if(nowLen >= bodyLen) return true;
        else{
            int rlen = std::min(bodyLen - nowLen, len);
            memcpy(body, buffer, rlen);
            nowLen += bodyLen;
            if(nowLen == bodyLen) return true;
        }
        return false;
    }

    const HTTPRequestHeader &getHeader() const {
        return header;
    }

    char *getBody() const {
        return body;
    }
};

#endif //MEOW_HTTPREQUEST_H
