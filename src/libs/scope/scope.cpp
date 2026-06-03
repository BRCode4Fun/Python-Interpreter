#include <stdexcept>
#include "scope.hpp"
#include "../value/py_object.hpp"
#include "../exceptions/python_exceptions.hpp"

Scope::Scope(Scope* enclosing_in) : enclosing(enclosing_in) {}

Scope::~Scope() {
    /* Releasing bindings must mirror Scope::define refcount bumps — otherwise values leak or double-free. */
    for(std::unordered_map<std::string, PyObject*>::value_type& entry : this->values){
        entry.second->dec_ref_count();
    }
}

void Scope::copy_bindings_from(const Scope& source) {
    for(const std::unordered_map<std::string, PyObject*>::value_type& entry : source.values) {
        this->define(entry.first, entry.second);
    }
}

/* Walk enclosing outward to build clones from outermost inward down to the requested frame. */
std::unique_ptr<Scope> Scope::clone_chain(Scope* frame) {
    if(frame == nullptr) {
        return nullptr;
    }
    std::unique_ptr<Scope> prefix = clone_chain(frame->enclosing);
    Scope* enc = prefix.get();
    auto self = std::make_unique<Scope>(enc);
    self->copy_bindings_from(*frame);
    self->owning_prefix = std::move(prefix);
    return self;
}

bool Scope::empty() {
    return values.empty();
}

void Scope::define(const std::string& name, PyObject* value){

    std::unordered_map<std::string, PyObject*>::iterator it = this->values.find(name);

    if(it != values.end()){
        PyObject* tp_object = it->second;
        tp_object->dec_ref_count();
        it->second = value;

    } else {
        values.emplace(name, value);
    }
    /* Every live binding holds one refcount contribution from the Scope's perspective. */
    value->inc_ref_count();
}

PyObject* Scope::get(const std::string& name){

    std::unordered_map<std::string, PyObject*>::iterator it = this->values.find(name);

    if(it != values.end()) {
        return it->second;

    } else if(enclosing){
        return enclosing->get(name);

    } else {
        throw NameError("Undeclared variable '" + name + "'.");
    }
    return nullptr;
}
