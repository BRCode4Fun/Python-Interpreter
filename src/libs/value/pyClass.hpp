#pragma once

#include "./pyObject.hpp"
#include "../scope/scope.hpp"

class PyClass : public PyObject {
public:
    explicit PyClass(const std::string& kname, Scope* objEnv) 
        : PyObject(ObjectType::Klass, std::move(objEnv)), kname(kname) {}
    
    inline bool isKlass() const override { return true; }
    inline bool isTruthy() const override { return true; }    

    Scope* getScope() {
        return getClassData();
    }
    void write(std::ostream& out) const override {
        out << ("<class \'" + kname + "\'>");
    }
        
private:
    std::string kname;
    Scope* getClassData() const {
        return static_cast<Scope*>(data);
    }
    void deleteData() override {
        delete getClassData();
    }
};
