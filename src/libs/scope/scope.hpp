#pragma once

#include <unordered_map>
#include "../value/pyObject.hpp"


enum class ScopeType
{
   FUNC,
   CLASS,
   NOT_ALL
};


class Scope {

public:
    Scope(Scope* parent = nullptr ,  ScopeType type = ScopeType::FUNC) : Scope_Type(type) , parent(parent){}
    ~Scope();

    void define(const std::string& name, PyObject* value);
    void decRefCount(PyObject* value);
    void incRefCount(PyObject* value);

    PyObject* get(const std::string& name);
    PyObject* peek(const std::string& name);
    void show_scope_content() const;

private:
    std::unordered_map<std::string, PyObject*> values;

    Scope* parent;
    ScopeType  Scope_Type;
};
