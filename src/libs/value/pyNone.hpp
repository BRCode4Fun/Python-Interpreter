#pragma once

#include "./pyObject.hpp"

class PyNone : public PyObject {

public:
    explicit PyNone();

    inline bool is_none_type() const override { return true; }

private:
    void registerMethods() override;
    void deleteData() override {}
};
