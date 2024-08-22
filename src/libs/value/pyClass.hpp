#pragma once

#include "./pyObject.hpp"

class PyClass : public PyObject {

public:
    explicit PyClass(const std::string& kname, Scope* closure);

    inline bool is_klass_type() const override { return true; }

    const std::string& get_class_name() const { return kname; }

private:
    std::string kname;

    void registerMethods() override;
    void deleteData() override {}
};