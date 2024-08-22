#pragma once

#include "./pyObject.hpp"

class PyFloat : public PyObject {
public:
    explicit PyFloat(const std::string&);
    explicit PyFloat(llf v);

    inline bool is_float_type() const override { return true; }
        
    llf getFloat() const {
        return *getFloatData();
    }

protected:
    void registerMethods() override;
    
    void deleteData() override {
        delete getFloatData();
    }

private:
    llf* getFloatData() const {
        return static_cast<llf*>(data);
    }
};
