#pragma once

#include "./pyObject.hpp"

class PyInt : public PyObject {
public:
    explicit PyInt(const std::string&);
    explicit PyInt(lld v);

    inline bool is_int_type() const override { return true; }
        
    lld getInt() const {
        return *getIntData();
    }

protected:
    void registerMethods() override;
    
    void deleteData() override {
        delete getIntData();
    }

private:
    lld* getIntData() const {
        return static_cast<lld*>(data);
    }
};
