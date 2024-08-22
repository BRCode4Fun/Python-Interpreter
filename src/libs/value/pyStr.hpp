#pragma once

#include <string>
#include "./pyObject.hpp"

class PyStr : public PyObject {
public:
    explicit PyStr(const std::string&);
    
    inline bool is_str_type() const override { return true; }
        
    const std::string& getStr() const {
        return *getStrData();
    }
    
private:
    const std::string* getStrData() const {
        return static_cast<std::string*>(data);
    }
    void registerMethods() override;
    
    void deleteData() override {
        delete getStrData();
    }
};
