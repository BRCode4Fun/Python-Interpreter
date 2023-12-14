#pragma once

#include "./pyClass.hpp"
#include "../scope/scope.hpp"

class PyInstance : public PyObject {
public:
    explicit PyInstance(Scope* objEnv , PyClass * class_ref)
        : PyObject(ObjectType::Instance, std::move(objEnv)) , m_class(class_ref) {}

    inline bool isInstance() const { return true; }
    inline bool isTruthy() const override { return true; }

    Scope* getScope() {
        return getInstanceData();
    }

    PyClass *get_class()
    {
        return m_class;
    }


private:
    Scope* getInstanceData() const {
        return static_cast<Scope*>(data);
    }
    void deleteData() override {
        delete getInstanceData();
    }

    PyClass * m_class;
};
