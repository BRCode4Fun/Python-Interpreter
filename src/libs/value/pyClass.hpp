#pragma once

#include "./pyObject.hpp"
#include "./pyFnObj.hpp"

#include "../ast/ast.hpp"

class PyClass : public PyObject {
public:    
    explicit PyClass(ClassNode* obj) 
        : PyObject(ObjectType::Klass, std::move(obj)){}
    
    inline bool isKlass() const override { return true; }
    inline bool isTruthy() const override { return true; }    
    
    const ClassNode* getKlass() const {
        return getClassData();
    }
    void write(std::ostream& out) const override {
        out << ("<class \'" + getClassData()->getName() + "\'>");
    }
        
private:
    ClassNode* getClassData() const {
        return static_cast<ClassNode*>(data);
    }
    void deleteData() override {
        delete getClassData();
    }
};

/*class PyClass : public PyObject {
public:
    explicit PyClass(const std::string& name, PyClass* superclass = nullptr)
        : PyObject(ObjectType::Klass, new std::unordered_map<std::string, PyObject*>()), 
          name(name), superclass(superclass) {}
         
    inline bool isKlass() const override { return true; }
    inline bool isTruthy() const override { return true; }
    
    void addMember(const std::string& name, PyObject* member) {
        *data[name] = member;
    }
    
    PyObject* getMember(const std::string& name) const {
        auto it = (*data).find(name);
        if(it != data->end()) {
            return it->second;
        } else {
            throw std::runtime_error("Member not found.");
        }
    }
    
    void write(std::ostream& out) const override {
        out << ("<class \'" + name + "\'>");
    }
    
private:
    const std::string name;
    PyClass* superclass;
    
    std::vector<std::string, PyObject*>* getKlassData() const {
        return static_cast<std::unordered_map<std::string, PyObject*>*>(data);
    }
    
    void deleteData() override {
        delete superclass;
        
        for (auto& kv : data) {
            delete kv.second;
        }
        delete getKlassData();
    }
};*/
