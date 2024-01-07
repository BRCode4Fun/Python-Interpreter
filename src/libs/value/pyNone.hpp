#pragma once

#include "./pyObject.hpp"
#include "./pyBool.hpp"

class PyNone : public PyObject {
public:
    explicit PyNone()
        : PyObject(ObjectType::None, nullptr) {}
        
    inline bool isNone() const override { return true; }
    inline bool isTruthy() const override { return false; }

    PyObject* operator==(const PyObject& other) const override {
        return new PyBool(other.isNone() ? true : false); // None is only equal to None
    }

    void write(std::ostream& out) const override {
        out << "None";
    }
};


