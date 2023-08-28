#pragma once

#include "./pyInt.hpp"

class PyBool : public PyInt {
public:
    explicit PyBool(bool v)
        : PyInt(v ? 1L : 0L) {}

    inline bool isBool() const override { return true; }
    inline bool isTruthy() const override { return getBool(); }

    bool getBool() const {
        return getInt() != 0L;
    }
    void write(std::ostream& out) const override {
        out << (getBool() ? "True" : "False");
    }
};
