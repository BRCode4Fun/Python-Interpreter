#pragma once

#include "./pyObject.hpp"
#include "../ast/ast.hpp"

class PyFnObj : public PyObject {
public:    
    explicit PyFnObj(FunctionNode* obj) 
        : PyObject(ObjectType::Func, std::move(obj)){}
    
    inline bool isFunc() const override { return true; }
    inline bool isTruthy() const override { return true; }    
        
    const FunctionNode* getFunc() const {
        return getFuncData();
    }
        
private:
    FunctionNode* getFuncData() const {
        return static_cast<FunctionNode*>(data);
    }
    void deleteData() override {
        delete getFuncData();
    }
};
