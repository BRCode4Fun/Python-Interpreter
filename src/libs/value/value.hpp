#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>

using namespace std;

class Value {
    
    public:
    
        enum class ValueType {
            NONE,
            INT, FLOAT,
            BOOLEAN,
            STRING,
            LIST
        };
      
        Value() : type(ValueType::NONE), data(nullptr) {}
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

        inline bool is_int() const { return type == ValueType::INT; }
        inline bool is_float() const { return type == ValueType::FLOAT; }
        inline bool is_bool() const { return type == ValueType::BOOLEAN; }
        inline bool is_string() const { return type == ValueType::STRING; }
        inline bool is_list() const { return type == ValueType::LIST; }
        inline bool is_none() const { return type == ValueType::NONE; }

        int getInt() const {
            if (not is_int()) {
                throw runtime_error("Value is not an integer.");
            }
            return *getIntData();
        }
        double getFloat() const {
            if (not is_float()) {
                throw runtime_error("Value is not a float.");
            }
            return *getFloatData();
        }
        bool getBoolean() const {
            if (not is_bool()) {
                throw runtime_error("Value is not a boolean.");
            }
            return *getBoolData();
        }
        const string& getString() const {
            if (not is_string()) {
                throw runtime_error("Value is not a string.");
            }
            return *getStringData();
        }
        const std::vector<Value>& getList() const {
            if (not is_list()) {
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

        // TODO: replace with __eq__ later
        bool operator==(const Value& other) const {
            
            if((*this).is_none()) {
                if(other.is_none()) return true;
                else return false; // None is only equal to None
            
            } else if((*this).is_float() and other.is_bool()) {

              return getFloat() == ((double)other.getBoolean());  

            } else if((*this).is_bool() and other.is_float()) {

              return ((float)getBoolean()) == other.getFloat();  

            } else if(type != other.type) {
                return false;

            } else if((*this).is_float()){
                
                return getFloat() == other.getFloat();

            } else if((*this).is_bool()){
                
                return getBoolean() == other.getBoolean();

            } else {
                throw runtime_error("Unsupported operands for equality.");
            }
        }

        // TODO: replace with __repr__ later
        friend ostream& operator<<(ostream& out, const Value& value) {
            if(value.is_none()) {
                out << "None";
            } else if(value.is_bool()) {
                out << (value.getBoolean() ? "True" : "False");
            } else if(value.is_float()) {
                out << value.getFloat();
            } else {
                throw runtime_error("Yet not printable object.");
            }
            return out;
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
