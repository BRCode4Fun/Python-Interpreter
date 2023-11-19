#pragma once

#include "./pyObject.hpp"
#include "../ast/ast.hpp"

class PyFunction : public PyObject {
public:    
    explicit PyFunction(FunctionNode* fnode) 
        : PyObject(ObjectType::Func, std::move(fnode)){}
    
    inline bool isFunc() const override { return true; }
    inline bool isTruthy() const override { return true; }    
    
    FunctionNode* getDecl() const {
        return getFuncData();
    }
    const std::vector<AstNode*>& getParams() {
        FunctionNode* fnode = getFuncData();
        return fnode->getParams();
    }
    AstNode* getBody() {
        FunctionNode* fnode = getFuncData();
        return fnode->getBody();
    }
    void write(std::ostream& out) const override {
        out << "<function " << getDecl()->getName() << ">";
    }
        
private:
    FunctionNode* getFuncData() const {
        return static_cast<FunctionNode*>(data);
    }
    void deleteData() override {
        delete getFuncData();
    }
};
