#pragma once

#include "./pyObject.hpp"

class PyStr : public PyObject {
public:
    explicit PyStr(const std::string& v) 
        : PyObject(ObjectType::String, new std::string(v)) {}
    
    inline bool isStr() const override { return true; } 
    inline bool isTruthy() const override { return getStr() != ""; }
     
    PyObject* operator+(const PyObject& other) const override {
        if(other.isStr()) {
            const PyStr* rhs = dynamic_cast<const PyStr*>(&other);
            return new PyStr(getStr() + rhs->getStr());
        } else {
            throw std::runtime_error("Unsupported operands for +.");
        }
    }
    PyObject* operator==(const PyObject& other) const override {
        if(other.isStr()) {
            const PyStr* rhs = dynamic_cast<const PyStr*>(&other);
            return new PyBool(getStr() == rhs->getStr());
        } else {
            return new PyBool(false);
        }
    }
        
    const std::string& getStr() const {
        return *getStrData();
    }
    void write(std::ostream& out) const override {
        out << "\'" << getStr() << "\'";
    }
    
private:
    const std::string* getStrData() const {
        return static_cast<std::string*>(data);
    }
    void deleteData() override {
        delete getStrData();
    }
};
