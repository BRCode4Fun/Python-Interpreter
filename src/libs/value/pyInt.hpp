#pragma once

#include "./pyObject.hpp"

class PyInt : public PyObject {
public:
    explicit PyInt(const std::string& v);
    explicit PyInt(ll v);

    inline bool isInt() const override { return true; }
    inline bool isTruthy() const override { return getInt() != 0L; }
    
    PyObject* operator+(const PyObject& other) const override;
    PyObject* operator-(const PyObject& other) const override;
    PyObject* operator*(const PyObject& other) const override;
    PyObject* operator/(const PyObject& other) const override;
    PyObject* operator%(const PyObject& other) const override;
    PyObject* operator&(const PyObject& other) const override;
    PyObject* operator|(const PyObject& other) const override;
    PyObject* operator^(const PyObject& other) const override;
    PyObject* operator<<(const PyObject& other) const override;
    PyObject* operator>>(const PyObject& other) const override;
    PyObject* operator==(const PyObject& other) const override;
    PyObject* operator<(const PyObject& other) const override;
    PyObject* operator>(const PyObject& other) const override;
    PyObject* operator-() const override;
    PyObject* operator~() const override;
    PyObject* operator!() const override;
    
    ll getInt() const;
    void write(std::ostream& out) const override;
    
private:
    const ll* getIntData() const;
    void deleteData() override;
};

