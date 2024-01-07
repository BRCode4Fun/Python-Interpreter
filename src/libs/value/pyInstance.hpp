#pragma once

#include "../value/pyObject.hpp"
#include "../scope/scope.hpp"

class PyClass;

class PyInstance : public PyObject {
public:
    explicit PyInstance(PyClass* klass);
    inline bool isInstance() const { return true; }
    inline bool isTruthy() const override { return true; }
    
    PyObject* operator==(const PyObject&) const override;
    
    void define(const std::string& name, PyObject* value) {
        return this->fields->define(name, value);
    }
    Scope* getContext() { return fields; }

    void write(std::ostream& out) const override;

private:
    PyClass* base;
    Scope* fields;
    
    void deleteData() override {
        delete getContext();
    }
};
