#pragma once

#include <string>
#include <vector>
#include <stdexcept>

using namespace std;

class Value {
    
    public:
    
        enum class ValueType {
            UNDEFINED,
            INT, FLOAT,
            BOOLEAN,
            STRING,
            LIST
        };
      
        Value() : type(ValueType::UNDEFINED), data(nullptr) {}
        explicit Value(int v) : type(ValueType::INT), data(new int(v)) {}
        explicit Value(double v) : type(ValueType::FLOAT), data(new double(v)) {}
        explicit Value(bool v) : type(ValueType::BOOLEAN), data(new bool(v)) {}
        explicit Value(const string& v) : type(ValueType::STRING), data(new string(v)) {}
        explicit Value(const vector<Value>& v) : type(ValueType::LIST), data(new vector<Value>(v)) {}

        // copy constructor
        Value(const Value& other) : type(other.type), data(nullptr) {
            switch (type) {
                case ValueType::INT:
                    data = new int(*other.getIntData());
                    break;
                case ValueType::FLOAT:
                    data = new double(*other.getFloatData());
                    break;
                case ValueType::STRING:
                    data = new std::string(*other.getStringData());
                    break;
                case ValueType::LIST:
                    data = new std::vector<Value>(*other.getListData());
                    break;
                default:
                    break;
            }
        }

        // move constructor
        Value(Value&& other) : type(other.type), data(other.data) {
            other.data = nullptr;
        }

        ~Value() {
            deleteData();
        }
    
        ValueType getType() const {
            return type;
        }

        int getInt() const {
            if (type != ValueType::INT) {
                throw runtime_error("Value is not an integer.");
            }
            return *getIntData();
        }

        double getFloat() const {
            if (type != ValueType::FLOAT) {
                throw runtime_error("Value is not a float.");
            }
            return *getFloatData();
        }

        bool getBoolean() const {
            if (type != ValueType::BOOLEAN) {
                throw runtime_error("Value is not a boolean.");
            }
            return *getBoolData();
        }

        const string& getString() const {
            if (type != ValueType::STRING) {
                throw runtime_error("Value is not a string.");
            }
            return *getStringData();
        }

        const std::vector<Value>& getList() const {
            if (type != ValueType::LIST) {
                throw runtime_error("Value is not a list");
            }
            return *getListData();
        }

        const int* getIntData() const {
            return static_cast<int*>(data);
        }
        const double* getFloatData() const {
            return static_cast<double*>(data);
        }
        const bool* getBoolData() const {
            return static_cast<bool*>(data);
        }
        const string* getStringData() const {
            return static_cast<string*>(data);
        } 
        const vector<Value>* getListData() const {
            return static_cast<vector<Value>*>(data);
        }

        // Copy assignment operator
        Value& operator=(const Value& other) {
            if(this != &other) {
                deleteData();
                type = other.type;

                switch(type) {
                    case ValueType::INT:
                        data = new int(*other.getIntData());
                        break;
                    case ValueType::FLOAT:
                        data = new double(*other.getFloatData());
                        break;
                    case ValueType::BOOLEAN:
                        data = new bool(*other.getBoolData());
                        break;
                    case ValueType::STRING:
                        data = new string(*other.getStringData());
                        break;
                    case ValueType::LIST:
                        data = new vector<Value>(*other.getListData());
                        break;
                    default:
                        break;
                }
            }
            return *this;
        }
        
        // Move assignment operator
        Value& operator=(Value&& other) {
            if(this != &other) {
                deleteData();
                type = other.type;
                data = other.data;
                other.data = nullptr; 
            }
            return *this;
        }
    
    private:
        ValueType type;
        void* data;

        void deleteData() {
            if(data != nullptr) {
                switch (type) {
                    case ValueType::INT:
                        delete getIntData();
                        break;
                    case ValueType::FLOAT:
                        delete getFloatData();
                        break;
                    case ValueType::BOOLEAN:
                        delete getBoolData();
                        break;
                    case ValueType::STRING:
                        delete getStringData();
                        break;
                    case ValueType::LIST:
                        delete getListData();
                        break;
                    default:
                        break;
                }
            }
        }
};
