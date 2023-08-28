#pragma once

#include <unordered_map>
#include "../value/pyObject.hpp"

class Scope {

public:
    Scope();
    Scope(Scope* parent);

    void define(const std::string& name, PyObject* value);
    void decRefCount(PyObject* value);
    void incRefCount(PyObject* value);

    PyObject* get(const std::string& name);

private:
    Scope* parent = nullptr;
    std::unordered_map<std::string, PyObject*> values;
};
