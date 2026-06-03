#pragma once

#include "../value/py_object.hpp"

class ReturnException : public std::exception {
public:
    ReturnException(PyObject* value) : value(value) {}
    PyObject* value;
};

class BreakException : public std::exception {
public:
    BreakException() {}
};

class ContinueException : public std::exception {
public:
    ContinueException() {}
};
