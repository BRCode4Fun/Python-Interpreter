#pragma once

#include "./pyClass.hpp"
#include "../scope/scope.hpp"

class PyInstance : public PyObject {
public:
    explicit PyInstance(Scope* objEnv)
        : PyObject(ObjectType::Instance, std::move(objEnv)){}
    
    inline bool isInstance() const { return true; }
    inline bool isTruthy() const override { return true; }
    
    Scope* getScope() {
        return getInstanceData();
    } 
       
private:
    Scope* getInstanceData() const {
        return static_cast<Scope*>(data);
    }
    void deleteData() override {
        delete getInstanceData();
    }
};
