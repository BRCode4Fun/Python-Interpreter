#pragma once

#include "./pyClass.hpp"

class PyInstance : public PyObject {

public:
    explicit PyInstance(PyClass* klass);

    inline bool is_instance_type() const override { return true; }

    PyClass* get_class() const { return base; }

private:
    PyClass* base;

    void registerMethods() override;
    
    void deleteData() override {
        //delete klass;
    }
};
