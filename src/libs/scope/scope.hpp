#pragma once

#include <memory>
#include <unordered_map>
#include <string>

class PyObject;

/*
 * A lexical environment: name -> PyObject*. `define`/`~Scope` cooperate with PyObject refcounts
 * so bindings keep values alive and releasing a binding drops the refcount. Enclosing scopes are
 * raw pointers forming a stack-linked chain; `clone_chain` materializes an owned tree for closures.
 */
class Scope {

public:
    explicit Scope(Scope* enclosing = nullptr);
    ~Scope();

    bool empty();
    void clear() { values.clear(); }

    void define(const std::string& name, PyObject* value);
    PyObject* get(const std::string& name);

    Scope* get_enclosing() { return this->enclosing; }

    /** Deep copy of the chain from outer frames down to `frame` (for closures that outlive a popped call frame). */
    static std::unique_ptr<Scope> clone_chain(Scope* frame);

private:
    void copy_bindings_from(const Scope& source); /* duplicate name map into another Scope */

    std::unordered_map<std::string, PyObject*> values;
    Scope* enclosing = nullptr;
    /** When this node is the root of a clone_chain result, owns all outer cloned frames (enclosing is inside this tree). */
    std::unique_ptr<Scope> owning_prefix;
};
