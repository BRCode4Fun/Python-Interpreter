#pragma once

#include "./pyObject.hpp"

class PyFloat : public PyObject {
public:
    explicit PyFloat(const std::string& v);
    explicit PyFloat(llf v);
        
    inline bool isFloat() const override { return true; } 
    inline bool isTruthy() const override { return getFloat() != 0.0; }
    
    PyObject* operator+(const PyObject& other) const override;
    PyObject* operator-(const PyObject& other) const override;
    PyObject* operator*(const PyObject& other) const override;
    PyObject* operator/(const PyObject& other) const override;
    PyObject* operator==(const PyObject& other) const override;
    PyObject* operator<(const PyObject& other) const override;
    PyObject* operator>(const PyObject& other) const override;
    PyObject* operator-() const override;
    PyObject* operator!() const override;
    
    llf getFloat() const;
    void write(std::ostream& out) const override;
        
private:
    const llf* getFloatData() const;
    void deleteData() override;
};
