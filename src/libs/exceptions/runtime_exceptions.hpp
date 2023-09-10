#pragma once

#include "../value/pyObject.hpp"

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
