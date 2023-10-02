#pragma once

#include <vector>
#include <stdexcept>
#include <iostream>

using ll = long long int;
using llf = long double;

class PyObject {
public:
    enum class ObjectType {
        None, 
        Int, Float,
        Boolean, String,
        List, Func, 
        Klass, Instance
    };
    PyObject(ObjectType type, void* data = nullptr) 
        : type(type), data(data) {}
        
    ~PyObject() {
        deleteData();
    }
    virtual inline bool isInt() const { return false; }
    virtual inline bool isFloat() const { return false; }
    virtual inline bool isStr() const { return false; }
    virtual inline bool isBool() const { return false; }
    virtual inline bool isList() const { return false; }
    virtual inline bool isFunc() const { return false; }
    virtual inline bool isKlass() const { return false; }
    virtual inline bool isInstance() const { return false; }
    virtual inline bool isNone() const { return false; }
    
    virtual inline bool isTruthy() const { 
        throw std::runtime_error("Yet not evaluatable object.");
    }
    
    virtual PyObject* operator+(const PyObject& other) const {
        throw std::runtime_error("Unsupported operands for +.");
    }
    virtual PyObject* operator-(const PyObject& other) const {
        throw std::runtime_error("Unsupported operands for -.");
    }
    virtual PyObject* operator*(const PyObject& other) const {
        throw std::runtime_error("Unsupported operands for *.");
    }
    virtual PyObject* operator/(const PyObject& other) const {
        throw std::runtime_error("Unsupported operands for /.");
    }
    virtual PyObject* operator%(const PyObject& other) const {
        throw std::runtime_error("Unsupported operands for %.");
    }
    virtual PyObject* operator&(const PyObject& other) const {
        throw std::runtime_error("Unsupported operands for &.");
    }
    virtual PyObject* operator|(const PyObject& other) const {
        throw std::runtime_error("Unsupported operands for |.");
    }
    virtual PyObject* operator^(const PyObject& other) const {
        throw std::runtime_error("Unsupported operands for ^.");
    }
    virtual PyObject* operator<<(const PyObject& other) const {
        throw std::runtime_error("Unsupported operands for <<.");
    }
    virtual PyObject* operator>>(const PyObject& other) const {
        throw std::runtime_error("Unsupported operands for >>.");
    }
    virtual PyObject* operator==(const PyObject& other) const {
        throw std::runtime_error("Unsupported operands for ==.");
    }
    virtual PyObject* operator<(const PyObject& other) const {
        throw std::runtime_error("Unsupported operands for <.");
    }
    virtual PyObject* operator>(const PyObject& other) const {
        throw std::runtime_error("Unsupported operands for >.");
    }
    virtual PyObject* operator-() const {
        throw std::runtime_error("Unsupported operands for unary -.");
    }
    virtual PyObject* operator~() const {
        throw std::runtime_error("Unsupported operands for unary ~.");
    }
    virtual PyObject* operator!() const {
        throw std::runtime_error("Unsupported operands for unary !.");
    }
        
    ObjectType getType() const {
        return type;
    }
    void incRc() { ++rc; }
    void decRc() { if(rc > 0) --rc; }
    int getRc() const { return rc; }
    
    friend std::ostream& operator<<(std::ostream& out, const PyObject& value) {
        value.write(out);
        return out;
    }
    virtual void write(std::ostream& out) const {
        throw std::runtime_error("Yet not printable object.");
    }
    
protected:
    void* data;
    ObjectType type;
    int rc; // reference counting
    virtual void deleteData() {}
};
