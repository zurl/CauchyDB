//
// Created by 张程易 on 02/06/2017.
//

#ifndef MEOW_HTTPHEADER_H
#define MEOW_HTTPHEADER_H


#include <string>
#include <map>


class HTTPHeader {
protected:
    std::map<std::string, std::string> headers;
public:
    inline bool hasHeader(const std::string & key){
        auto iter = headers.find(key);
        return iter != headers.end();
    }
    inline const std::string & getHeader(const std::string & key){
        return headers[key];
    }
    inline void setHeader(const std::string & key, const std::string & value){
        headers[key] = value;
    }
};


#endif //MEOW_HTTPHEADER_H
