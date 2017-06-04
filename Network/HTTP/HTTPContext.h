//
// Created by 张程易 on 03/06/2017.
//

#ifndef MEOW_HTTPCONTEXT_H
#define MEOW_HTTPCONTEXT_H

#include "HTTPRequest.h"
#include "HTTPResponse.h"

class HTTPContext{
    friend class HTTPServer;
    HTTPRequest * request = nullptr;
    HTTPResponse * response = nullptr;
    char * buffer = nullptr;
    int now = 0;
    bool append(const char * buf, int len){
        if(buffer != nullptr){
            memcpy(buffer + now, buf, len);
            for(int i = std::max(0, now - 4); i + 3 < now + len; i++){
                if(strncmp(buffer + i, "\r\n\r\n", 4) == 0){
                    request = new HTTPRequest(buffer);
                    bool ret = request->appendBody(buffer + i + 4, now + len - i);
                    delete[] buffer;
                    buffer = nullptr;
                    if( ret ){
                        response = new HTTPResponse();
                    }
                    return ret;
                }
            }
            return false;
        }
        else{
            if(request->appendBody(buf, len)){
                response = new HTTPResponse();
                return true;
            }
            else{
                return false;
            }
        }
    }
    void reset(){
        if(buffer != nullptr)delete[] buffer;
        if(request != nullptr) delete request;
        if(response != nullptr) delete response;
        request = nullptr;
        response = nullptr;
        buffer = (char * )malloc(1024);
        now = 0;
    }
public:
    HTTPContext() {
        buffer = (char * )malloc(1024);
    }
    ~HTTPContext(){
        if(buffer != nullptr)delete[] buffer;
        if(request != nullptr) delete request;
        if(response != nullptr) delete response;
    }
    HTTPRequest * getRequest() const {
        return request;
    }

    HTTPResponse * getResponse() const {
        return response;
    }

};

#endif //MEOW_HTTPCONTEXT_H
