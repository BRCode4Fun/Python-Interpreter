#pragma once

#include <vector>
#include "./pyObject.hpp"

class PyTypeObject : public PyObject {
public:
    explicit PyTypeObject(
        const std::string& name,
        PyTypeObject* metatype,
        const std::vector<PyTypeObject*>& bases = {},
        Scope* context = nullptr
    );

    inline bool is_type_type() const override { return true; }

    const std::string& get_name() const { return type_name; }
    const std::vector<PyTypeObject*>& get_bases() const { return bases; }

    PyObject* call(std::vector<PyObject*>& args, Interpreter* interpreter = nullptr);
    PyObject* instantiate() const;
    void ensure_methods_registered();

    bool is_subclass_of(const PyTypeObject* candidate_base) const;
    bool is_instance(PyObject* obj) const;
    PyObject* find(const std::string& name) override;

private:
    std::string type_name;
    std::vector<PyTypeObject*> bases;
    void registerMethods() override;
    void deleteData() override {}
};
