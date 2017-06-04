//
// Created by 张程易 on 02/06/2017.
//

#ifndef MEOW_HTTPREQUESTHEADER_H
#define MEOW_HTTPREQUESTHEADER_H

#include "HTTPHeader.h"
#include "HTTPMethod.h"
#include "../../Exception.h"

class HTTPRequestHeader : public HTTPHeader{
    HTTPMethod method;
    std::string version;
    std::string path;
public:
    HTTPRequestHeader(const char * content) {
        const char * now = content;
        if( strncasecmp(now, "GET ", 4) == 0){
            method = HTTPMethod::GET;
            now += 4;
        }
        else if( strncasecmp(now, "POST ", 5) == 0 ){
            method = HTTPMethod::POST;
            now += 5;
        }
        else if( strncasecmp(now, "PUT ", 4) == 0){
            method = HTTPMethod::PUT;
            now += 4;
        }
        else if( strncasecmp(now, "DELETE ", 7) == 0 ){
            method = HTTPMethod::DELETE;
            now += 7;
        }
        else{
            throw HTTPFormatException(0, "Error: at method.");
        }
        int len = 0;
        while( *(now + len) != ' ')len++;
        path = std::string(now, len);
        now += len + 1;
        len = 0;
        while( *(now + len) != '\r')len++;
        version = std::string(now, len);
        now += len + 2;
        while(true){
            if( *now == '\r' && *(now + 1) == '\n') break;
            len = 0;
            while( *(now + len) != ':')len++;
            std::string key(now, len);
            now += len + 2;
            len = 0;
            while( *(now + len) != '\r')len++;
            now += len + 2;
            len = 0;
            std::string value(now, len);
            HTTPHeader::setHeader(key, value);
        }
     }

    HTTPMethod getMethod() const {
        return method;
    }

    const std::string &getVersion() const {
        return version;
    }

    const std::string &getPath() const {
        return path;
    }
};

#endif //MEOW_HTTPREQUESTHEADER_H
