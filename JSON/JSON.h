#ifndef JSON_H
#define JSON_H

#include <cstdio>
#include <cstring>
#include <unordered_map>
#include <cmath>
#include <vector>
#include <sstream>
#include <iostream>

const int JSON_BUFFER_SIZE = 10888897;

class JSONInteger; class JSONDouble;
class JSONArray; class JSONObject;
class JSONBoolean;class JSONString;

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
    JSON * get(int index);
    JSON * get(const std::string &index);
    JSON * get(const char * index);
    virtual std::string toString(bool format = false, int indent = 0) = 0;
    JSONInteger * toInteger();
    JSONString * toJString();
    JSONDouble * toDouble();
    JSONArray * toArray();
    JSONObject * toObject();
    JSONBoolean * toBoolean();
    virtual ~JSON() = default;
    virtual Type type() = 0;
};

class JSONString : public JSON{
    public:
    std::string str;
    JSONString( const std::string & str );
    JSONString( const char * str );
    JSONString( const char * str , int n );
    virtual std::string toString(bool format = false, int indent = 0) override ;
    virtual Type type() override ;
};

class JSONInteger : public JSON{
    public:
    long long value;
    JSONInteger( long long value );
    virtual std::string toString(bool format = false, int indent = 0) override ;
    virtual Type type() override ;
};

class JSONDouble : public JSON {
    public:
    double value;
    JSONDouble( double value) ;
    virtual std::string toString(bool format = false, int indent = 0) override;
    virtual Type type() override;
};



class JSONBoolean : public JSON{
    public:
    bool value;
    JSONBoolean(bool value);
    virtual std::string toString(bool format = false, int indent = 0) override;
    virtual Type type() override ;
};

class JSONArray : public JSON{
    std::vector<JSON *> elements;
public:
    ~JSONArray() ;
    virtual std::string toString(bool format = false, int indent = 0) override ;
    virtual Type type() override ;
    inline const std::vector<JSON *> &getElements() const {
        return elements;
    }
    inline void put(JSON * json){
        this->elements.emplace_back(json);
    }
};

class JSONNull : public JSON {
    virtual std::string toString(bool format = false, int indent = 0) override ;
    virtual Type type() override ;
};
class JSONObject : public JSON{
    std::unordered_map<std::string, JSON *> hashMap;
public:
    JSONObject();
    ~JSONObject();
    virtual std::string toString(bool format = false, int indent = 0) override;
    virtual Type type() override;
    inline void set(const std::string & key, const std::string & value){
        this->hashMap.emplace(key, new JSONString(value));
    }
    inline void set(const std::string & key, const char * value){
        this->hashMap.emplace(key, new JSONString(value));
    }
    inline void set(const std::string & key, JSON * json){
        this->hashMap.emplace(key, json);
    }
    inline void set(const std::string & key, long long value){
        this->hashMap.emplace(key, new JSONInteger(value));
    }
    inline void set(const std::string & key, int value){
        this->hashMap.emplace(key, new JSONInteger(value));
    }
    inline void set(const std::string & key, double value){
        this->hashMap.emplace(key, new JSONDouble(value));
    }
    inline void set(const std::string & key, bool value){
        this->hashMap.emplace(key, new JSONBoolean(value));
    }
    const std::unordered_map<std::string, JSON *> &getHashMap() const;

};
#endif
