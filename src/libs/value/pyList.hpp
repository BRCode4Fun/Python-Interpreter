#pragma once

#include "./pyObject.hpp"

class PyList : public PyObject {
public:
    explicit PyList(const std::vector<PyObject*>& v)
        : PyObject(ObjectType::List, new std::vector<PyObject*>(v)) {}

    inline bool isList() const override { return true; }
    inline bool isTruthy() const override { return getList().size() != 0; }

    const std::vector<PyObject*>& getList() const {
        return *getListData();
    }

    const PyObject* operator[](size_t index) const {
        const auto& listData = getList();
        if (index < listData.size()) {
            return listData[index];
        }
        throw std::out_of_range("Index out of range in PyList");
    }

    std::size_t size() const {
        return getList().size();
    }
    void append(PyObject* item) {
        getListData()->push_back(item);
    }
    void remove(std::size_t index) {
        if (index < getList().size()) {
            getListData()->erase(getListData()->begin() + index);
        }
    }
    
    void write(std::ostream& out) const override {
        out << '[';
        bool not_first = false;
        for (const auto& elem : getList()) {
            if (not_first) out << ", ";
            elem->write(out);
            not_first = true;
        }
        out << ']';
    }
    
private:
    std::vector<PyObject*>* getListData() const {
        return static_cast<std::vector<PyObject*>*>(data);
    }
    void deleteData() override {
        delete getListData();
    }
};
