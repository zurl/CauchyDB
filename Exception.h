//
// Created by 张程易 on 03/06/2017.
//

#ifndef DB_EXCEPTION_H
#define DB_EXCEPTION_H

#include <iostream>
#include <string>

class Exception{
public:
    int code;
    std::string message;
    inline Exception(int code = 0, const std::string & message = "")
            :code(code), message(message){};
};

class SQLException : public Exception{
public:
    inline SQLException(int code = 0, const std::string & message = "")
            :Exception(code, std::move(message)){}
};

class SQLExecuteException: public SQLException{
public:
    inline SQLExecuteException(int code = 0,const std::string & message = "")
            :SQLException(code, std::move(message)){}
};

class SQLSyntaxException: public SQLException{
public:
    inline SQLSyntaxException(int code = 0, const std::string &message = "")
            :SQLException(code, std::move(message)){}
};

class SQLTypeException: public SQLException{
public:
    inline SQLTypeException(int code = 0,const std::string & message = "")
            :SQLException(code, std::move(message)){}
};

class NetworkException: public Exception {
public:
    inline NetworkException(int code = 0,const std::string & message = "")
            :Exception(code, std::move(message)){}
};

class HTTPFormatException: public Exception {
public:
    inline HTTPFormatException(int code = 0, const std::string & message = "")
            :Exception(code, std::move(message)){}
};



#endif //DB_EXCEPTION_H
