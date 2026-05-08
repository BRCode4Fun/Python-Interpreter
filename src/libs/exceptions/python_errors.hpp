#pragma once

#include <stdexcept>
#include <string>

class RuntimeError : public std::runtime_error {
public:
    explicit RuntimeError(const std::string& message) : std::runtime_error(message) {}
};

class SyntaxError : public RuntimeError {
public:
    explicit SyntaxError(const std::string& message) : RuntimeError(message) {}
};

class TypeError : public RuntimeError {
public:
    explicit TypeError(const std::string& message) : RuntimeError(message) {}
};

class NameError : public RuntimeError {
public:
    explicit NameError(const std::string& message) : RuntimeError(message) {}
};

class IndexError : public RuntimeError {
public:
    explicit IndexError(const std::string& message) : RuntimeError(message) {}
};

class ZeroDivisionError : public RuntimeError {
public:
    explicit ZeroDivisionError(const std::string& message) : RuntimeError(message) {}
};
