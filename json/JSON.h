#ifndef JSON_H
#define JSON_H

#include <cstdio>
#include <cstring>
#include <unordered_map>
#include <cmath>
#include <vector>
#include <sstream>
#include <iostream>

const size_t JSON_BUFFER_SIZE = 10888897;

class JSONInteger; class JSONDouble;
class JSONArray; class JSONObject;

class JSON{
    public:
    enum class Type {
        String, Array, Object, Integer, Boolean, Null, Double
    };
    static JSON * fromFile(const char * fileName);
    static JSON * parse(const char * str);
    bool saveIntoFile(const char * fileName, bool format = true);
    JSON * path(const std::string &path);
    JSON * path(const char * path);
    JSON * get(size_t index);
    JSON * get(const std::string &index);
    JSON * get(const char * index);
    virtual std::string toString(bool format = false, size_t indent = 0) = 0;
    JSONInteger * toInteger();
    JSONDouble * toDouble();
    JSONArray * toArray();
    JSONObject * toObject();
    const char * asCString();
    virtual ~JSON() = default;
    virtual Type type() = 0;
};

class JSONString : public JSON{
    public:
    std::string str;
    JSONString( const std::string & str );
    JSONString( const char * str );
    JSONString( const char * str , size_t n );
    virtual std::string toString(bool format = false, size_t indent = 0) override ;
    virtual Type type() override ;
};

class JSONInteger : public JSON{
    public:
    long long value;
    JSONInteger( long long value );
    virtual std::string toString(bool format = false, size_t indent = 0) override ;
    virtual Type type() override ;
};

class JSONDouble : public JSON {
    public:
    double value;
    JSONDouble( double value) ;
    virtual std::string toString(bool format = false, size_t indent = 0) override;
    virtual Type type() override;
};

class JSONObject : public JSON{
public:
    std::unordered_map<std::string, JSON *> hashMap;
    JSONObject();
    ~JSONObject();
    virtual std::string toString(bool format = false, size_t indent = 0) override;
    virtual Type type() override ;
};

class JSONBoolean : public JSON{
    public:
    bool value;
    JSONBoolean(bool value);
    virtual std::string toString(bool format = false, size_t indent = 0) override;
    virtual Type type() override ;
};

class JSONArray : public JSON{
    public:
    std::vector<JSON *> elements;
    ~JSONArray() ;
    virtual std::string toString(bool format = false, size_t indent = 0) override ;
    virtual Type type() override ;
};

class JSONNull : public JSON {
    virtual std::string toString(bool format = false, size_t indent = 0) override ;
    virtual Type type() override ;
};

#endif
