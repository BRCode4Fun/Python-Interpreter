#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include "../ast/ast.hpp"
#include "./FuncObject.hpp"

using namespace std;

class Value {
    
    public:
    
        enum class ValueType {
            NONE,
            INT, FLOAT,
            BOOLEAN,
            STRING,
            LIST , 
            FUNC 
        };
      
        Value() : type(ValueType::NONE), data(nullptr) {}
        
        explicit Value(long long v) : type(ValueType::INT), data(new long long(v)) {}
        explicit Value(double v) : type(ValueType::FLOAT), data(new double(v)) {}
        explicit Value(bool v) : type(ValueType::BOOLEAN), data(new bool(v)) {}
        explicit Value(const string& v) : type(ValueType::STRING), data(new string(v)) {}
        explicit Value(const vector<Value>& v) : type(ValueType::LIST), data(new vector<Value>(v)) {}
        explicit Value(FunctionNode * func) : type(ValueType::INT), data(new FuncObj(func)) {}

        // copy constructor
        Value(const Value& other) : type(other.type), data(nullptr) {
            switch (type) {
                case ValueType::INT:
                    data = new long long(*other.getIntData());
                    break;
                case ValueType::FLOAT:
                    data = new double(*other.getFloatData());
                    break;
                case ValueType::STRING:
                    data = new string(*other.getStringData());
                    break;
                case ValueType::LIST:
                    data = new vector<Value>(*other.getListData());
                    break;
                case ValueType::FUNC:
                    data = new FuncObj(*other.getFuncData());
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
        inline bool is_function() const { return type == ValueType::FUNC; }
        inline bool is_none() const { return type == ValueType::NONE; }

        const long long getInt() const {
            if (not is_int()) {
                throw runtime_error("Value is not an integer.");
            }
            return *getIntData();
        }
        const double getFloat() const {
            if (not is_float()) {
                throw runtime_error("Value is not a float.");
            }
            return *getFloatData();
        }
        const bool getBoolean() const {
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
        const vector<Value>& getList() const {
            if (not is_list()) {
                throw runtime_error("Value is not a list");
            }
            return *getListData();
        }

        const FuncObj& getFunc() const {

            if (not is_function()) {
                throw runtime_error("Value is not a   Function");
            }
            return *getFuncData();


        }

        // Copy assignment operator
        Value& operator=(const Value& other) {
            if(this != &other) {
                deleteData();
                type = other.type;

                switch(type) {
                    case ValueType::INT:
                        data = new long long(*other.getIntData());
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
                    case ValueType::FUNC:
                        data = new FuncObj(*other.getFuncData());
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

        // TODO: replace with call to __add__ later
        Value* operator+(const Value& other) const {
            
            if((*this).is_float()) {
                switch(other.type) {
                    case ValueType::FLOAT:
                        return new Value(getFloat() + other.getFloat());
                    case ValueType::INT:
                        return new Value(getFloat() + (double)other.getInt());
                    case ValueType::BOOLEAN:
                        return new Value(getFloat() + (long long)other.getBoolean());
                    default:
                        throw runtime_error("Unsupported operands for +.");
                }
            } else if((*this).is_int()) {
                switch(other.type) {
                    case ValueType::INT:
                        return new Value(getInt() + other.getInt());
                    case ValueType::FLOAT:
                        return new Value((double)getInt() + other.getFloat());
                    case ValueType::BOOLEAN:
                        return new Value(getInt() + (long long)other.getBoolean());
                    default:
                        throw runtime_error("Unsupported operands for +.");
                }
            } else if((*this).is_bool()) {
                switch(other.type) {
                    case ValueType::INT:
                        return new Value((long long)getBoolean() + other.getInt());
                    case ValueType::FLOAT:
                        return new Value((double)getBoolean() + other.getFloat());
                    case ValueType::BOOLEAN:
                        return new Value((long long)getBoolean() + (long long)other.getBoolean());
                    default:
                        throw runtime_error("Unsupported operands for +.");
                }
            } else if((*this).is_string() and other.is_string()) {  
                return new Value(getString() + other.getString());
            } else {
                throw runtime_error("Unsupported operands for +.");
            }
        }
        
        // TODO: replace with call to __mul__ later
        Value* operator*(const Value& other) const {
            
            if((*this).is_float()) {
                switch(other.type) {
                    case ValueType::FLOAT:
                        return new Value(getFloat() * other.getFloat());
                    case ValueType::INT:
                        return new Value(getFloat() * (double)other.getInt());
                    case ValueType::BOOLEAN:
                        return new Value(getFloat() * (double)other.getBoolean());
                    default:
                        throw runtime_error("Unsupported operands for *.");
                }
            } else if((*this).is_int()) {
                switch(other.type) {
                    case ValueType::INT:
                        return new Value(getInt() * other.getInt());
                    case ValueType::FLOAT:
                        return new Value((double)getInt() * other.getFloat());
                    case ValueType::BOOLEAN:
                        return new Value(getInt() * (long long)other.getBoolean());
                    case ValueType::STRING:
                        throw logic_error("Function not implemented yet");
                    default:
                        throw runtime_error("Unsupported operands for *.");
                }
            } else if((*this).is_bool()) {
                switch(other.type) {
                    case ValueType::INT:
                        return new Value((int)getBoolean() * other.getInt());
                    case ValueType::FLOAT:
                        return new Value((double)getBoolean() * other.getFloat());
                    case ValueType::BOOLEAN:
                        return new Value((long long)getBoolean() * (long long)other.getBoolean());
                    default:
                        throw runtime_error("Unsupported operands for *.");
                }
            } else if((*this).is_string() and other.is_int()) {  
                throw logic_error("Feature not implemented yet");
            } else {
                throw runtime_error("Unsupported operands for +.");
            }
        }
        
        // TODO: replace with call to __truediv__ later
        Value* operator/(const Value& other) const {
            
            if((*this).is_float()) {
                switch(other.type) {
                    case ValueType::FLOAT: {
                        double rvalue = other.getFloat();
                        if(rvalue == 0.0) throw runtime_error("Attempted to divide by zero");
                        else return new Value(getFloat() / rvalue);
                    } 
                    case ValueType::INT: {
                        double rvalue = (double) other.getInt();
                        if(rvalue == 0.0) throw runtime_error("Attempted to divide by zero");
                        else return new Value(getFloat() / rvalue);
                    }
                    case ValueType::BOOLEAN: {    
                        double rvalue = (double) other.getBoolean();
                        if(rvalue == 0.0) throw runtime_error("Attempted to divide by zero");
                        else return new Value(getFloat() / rvalue);
                    }
                    default:
                        throw runtime_error("Unsupported operands for /.");
                }
            } else if((*this).is_int()) {
                switch(other.type) {
                    case ValueType::INT: {
                        long long rvalue = other.getInt();
                        if(rvalue == 0L) throw runtime_error("Attempted to divide by zero");
                        return new Value(getInt() / rvalue);
                    }
                    case ValueType::FLOAT: {
                        double rvalue = other.getFloat();
                        if(rvalue == 0.0) throw runtime_error("Attempted to divide by zero");
                        return new Value((double)getInt() / rvalue);
                    }
                    case ValueType::BOOLEAN: {
                        long long rvalue = (int) other.getBoolean();
                        if(rvalue == 0L) throw runtime_error("Attempted to divide by zero");
                        return new Value(getInt() / rvalue);
                    }
                    default:
                        throw runtime_error("Unsupported operands for /.");
                }
            } else if((*this).is_bool()) {
                switch(other.type) {
                    case ValueType::INT: {
                        long long rvalue = other.getInt();
                        if(rvalue == 0L) throw runtime_error("Attempted to divide by zero");
                        else return new Value((int)getBoolean() / rvalue);
                    }
                    case ValueType::FLOAT: {
                        double rvalue = other.getFloat();
                        if(rvalue == 0.0) throw runtime_error("Attempted to divide by zero");
                        else return new Value((double)getBoolean() / rvalue);
                    }
                    case ValueType::BOOLEAN: {
                        long long rvalue = (int) other.getBoolean();
                        if(rvalue == 0L) throw runtime_error("Attempted to divide by zero");
                        else return new Value((int)getBoolean() / rvalue);
                    }
                    default:
                        throw runtime_error("Unsupported operands for /.");
                }
            } else {
                throw runtime_error("Unsupported operands for /.");
            }
        }
        
        // TODO: replace with call to __truediv__ later
        Value* operator%(const Value& other) const {
            
            if((*this).is_float()) {
                throw logic_error("Feature not implemented yet");
                
            } else if((*this).is_int()) {
                switch(other.type) {
                    case ValueType::INT: {
                        long long rvalue = other.getInt();
                        if(rvalue == 0L) throw runtime_error("Modulo by zero");
                        auto value = new Value(getInt() % rvalue);
                        return value;
                    }
                    case ValueType::FLOAT: {
                        throw logic_error("Feature not implemented yet");
                    }
                    case ValueType::BOOLEAN: {
                        long long rvalue = (int) other.getBoolean();
                        if(rvalue == 0L) throw runtime_error("Modulo by zero");
                        return new Value(getInt() % rvalue);
                    }
                    default:
                        throw runtime_error("Unsupported operands for %.");
                }
            } else if((*this).is_bool()) {
                switch(other.type) {
                    case ValueType::INT: {
                        long long rvalue = other.getInt();
                        if(rvalue == 0L) throw runtime_error("Modulo by zero");
                        else return new Value((int)getBoolean() % rvalue);
                    }
                    case ValueType::FLOAT: {
                        throw logic_error("Feature not implemented yet");
                    }
                    case ValueType::BOOLEAN: {
                        long long rvalue = (int) other.getBoolean();
                        if(rvalue == 0L) throw runtime_error("Modulo by zero");
                        else return new Value((int)getBoolean() % rvalue);
                    }
                    default:
                        throw runtime_error("Unsupported operands for %.");
                }
            } else {
                throw runtime_error("Unsupported operands for %.");
            }
        }
        
        // TODO: replace with call to __neg__ later
        Value* operator-() {
            switch(type) {
                case ValueType::INT:
                    return new Value(-getInt());
                case ValueType::FLOAT:
                    return new Value(-getFloat());
                case ValueType::BOOLEAN:
                    return new Value(-(long long)getBoolean());
                default:
                    throw runtime_error("Unsupported operands for unary -.");
            }
        }
        
        // TODO: replace with call to __not__ later
        Value* operator!() {
            return new Value(!(this->isTruthy())); 
        }
        
        // TODO: replace with call to __and__ later
        Value* operator&&(Value& other) {
            /* 
            *  try to do short-circuit: if after evaluating the left operand,
            *  the result of the logical expression is known, do not evaluate the right operand 
            */
            if(!(this->isTruthy())) return move(this);
            else return move(&other);
        }
        
        // TODO: replace with call to __or__ later
        Value* operator||(Value& other) {
            /* 
             *  try to do short-circuit: if after evaluating the left operand,
             *  the result of the logical expression is known, do not evaluate the right operand 
            */
            if(this->isTruthy()) return move(this);
            else return move(&other);
        }
        
        // TODO: replace with call to __sub__ later
        Value* operator-(const Value& other) const {
            
            if((*this).is_float()) {
                switch(other.type) {
                    case ValueType::FLOAT:
                        return new Value(getFloat() - other.getFloat());
                    case ValueType::INT:
                        return new Value(getFloat() - (double)other.getInt());
                    case ValueType::BOOLEAN:
                        return new Value(getFloat() - (double)other.getBoolean());
                    default:
                        throw runtime_error("Unsupported operands for -.");
                }
            } else if((*this).is_int()) {
                switch(other.type) {
                    case ValueType::INT:
                        return new Value(getInt() - other.getInt());
                    case ValueType::FLOAT:
                        return new Value((double)getInt() - other.getFloat());
                    case ValueType::BOOLEAN:
                        return new Value(getInt() - (long long)other.getBoolean());
                    default:
                        throw runtime_error("Unsupported operands for -.");
                }
            } else if((*this).is_bool()) {
                switch(other.type) {
                    case ValueType::BOOLEAN:
                        return new Value((long long)getBoolean() - (long long)other.getBoolean());
                    case ValueType::INT:
                        return new Value((long long)getBoolean() - other.getInt());
                    case ValueType::FLOAT:
                        return new Value((double)getBoolean() - other.getFloat());
                    default:
                        throw runtime_error("Unsupported operands for -.");
                }
            } else {
                throw runtime_error("Unsupported operands for -.");
            }
        }

        // TODO: replace with call to __eq__ later
        Value* operator==(const Value& other) const {
            
            if((*this).is_none()) {
                if(other.is_none()) return new Value(true);
                else return new Value(false); // None is only equal to None
            
            } else if(other.is_none()) {
                return new Value(false);
                
            } else if((*this).is_bool()) {
                switch(other.type) {
                    case ValueType::BOOLEAN:
                        return new Value(getBoolean() == other.getBoolean());
                        break;
                    case ValueType::INT:
                        return new Value((long long)getBoolean() == other.getInt());
                        break;
                    case ValueType::FLOAT:
                        return new Value((double)getBoolean() == other.getFloat());
                        break;
                    default:
                        throw runtime_error("Unsupported operands for ==.");
                } 
            } else if((*this).is_int()) {
                switch(other.type) {
                    case ValueType::INT:
                        return new Value(getInt() == other.getInt());
                        break;
                    case ValueType::FLOAT:
                        return new Value((double)getInt() == other.getFloat());
                        break;
                    case ValueType::BOOLEAN:
                        return new Value(getInt() == (long long)other.getBoolean());
                        break;
                    default:
                        throw runtime_error("Unsupported operands for ==.");
                }
                
            } else if((*this).is_float()) {
                switch(other.type) {
                    case ValueType::FLOAT:
                        return new Value(getFloat() == other.getFloat());
                        break;
                    case ValueType::INT:
                        return new Value(getFloat() == (double)other.getInt());
                        break;
                    case ValueType::BOOLEAN:
                        return new Value(getFloat() == (double)other.getBoolean());
                        break;
                    default:
                        throw runtime_error("Unsupported operands for ==.");
                }
            } else {
                throw runtime_error("Unsupported operands for equality.");
            }
        }

        // TODO: replace with call to __lt__ later
        Value* operator<(const Value& other) const {
            
            if((*this).is_int()) {
                switch(other.type) {
                    case ValueType::INT:
                        return new Value(getInt() < other.getInt());
                        break;
                    case ValueType::FLOAT:
                        return new Value((double)getInt() < other.getFloat());
                        break;
                    case ValueType::BOOLEAN:
                        return new Value(getInt() < (long long)other.getBoolean());
                        break;
                    default:
                        throw runtime_error("Unsupported operands for comparison.");
                }
            } else if((*this).is_float()) {
                switch(other.type) {
                    case ValueType::FLOAT:
                        return new Value(getFloat() < other.getFloat());
                        break;
                    case ValueType::INT:
                        return new Value(getFloat() < (double)other.getInt());
                        break;
                    case ValueType::BOOLEAN:
                        return new Value(getFloat() < (double)other.getBoolean());
                        break;
                    default:
                        throw runtime_error("Unsupported operands for comparison.");
                }
            } else if((*this).is_bool()) {
                switch(other.type) {
                    case ValueType::BOOLEAN:
                        return new Value(getBoolean() < other.getBoolean());
                        break;
                    case ValueType::INT:
                        return new Value((long long)getBoolean() < other.getInt());
                        break;
                    case ValueType::FLOAT:
                        return new Value((double)getBoolean() < other.getFloat());
                        break;
                    default:
                        throw runtime_error("Unsupported operands for comparison.");
                }
            } else {
                throw runtime_error("Unsupported operands for comparison.");
            }
        }

        // TODO: replace with call to __gt__ later
        Value* operator>(const Value& other) const {
            
            if((*this).is_int()) {
                switch(other.type) {
                    case ValueType::INT:
                        return new Value(getInt() > other.getInt());
                        break;
                    case ValueType::FLOAT:
                        return new Value((double)getInt() > other.getFloat());
                        break;
                    case ValueType::BOOLEAN:
                        return new Value(getInt() > (long long)other.getBoolean());
                        break;
                    default:
                        throw runtime_error("Unsupported operands for comparison.");
                }
            } else if((*this).is_float()) {
                switch(other.type) {
                    case ValueType::FLOAT:
                        return new Value(getFloat() > other.getFloat());
                        break;
                    case ValueType::INT:
                        return new Value(getFloat() > (double)other.getInt());
                        break;
                    case ValueType::BOOLEAN:
                        return new Value(getFloat() > (double)other.getBoolean());
                        break;
                    default:
                        throw runtime_error("Unsupported operands for comparison.");
                }
            } else if((*this).is_bool()) {
                switch(other.type) {
                    case ValueType::BOOLEAN:
                        return new Value(getBoolean() > other.getBoolean());
                        break;
                    case ValueType::INT:
                        return new Value((long long)getBoolean() > other.getInt());
                        break;
                    case ValueType::FLOAT:
                        return new Value((double)getBoolean() > other.getFloat());
                        break;
                    default:
                        throw runtime_error("Unsupported operands for comparison.");
                }
            } else {
                throw runtime_error("Unsupported operands for comparison.");
            }
        }

        // TODO: replace with __repr__ later
        friend ostream& operator<<(ostream& out, const Value& value) {
            if(value.is_none()) {
                out << "None";
            } else if(value.is_bool()) {
                out << (value.getBoolean() ? "True" : "False");
            } else if(value.is_int()) {
                out << value.getInt();
            } else if(value.is_float()) {
                out << value.getFloat();
            } else if(value.is_string()) {
                out << value.getString();
            } else {
                throw runtime_error("Yet not printable object.");
            }
            return out;
        }
        
        // TODO: replace with truth later
        bool isTruthy() {

            switch(type) {
                case ValueType::NONE:
                    return false; break;
                case ValueType::BOOLEAN:
                    return getBoolean(); break;
                case ValueType::INT:
                    return getInt() != 0; break;
                case ValueType::FLOAT:
                    return getFloat() != 0.0; break;
                case ValueType::STRING:
                    return getString() != ""; break;
                default:
                    throw runtime_error("Yet not evaluatable object.");
            }
        }

        void Increment_Reference_counting() {
            ++Reference_counting;   
        }

        void Decrement_Reference_counting() {

            if(Reference_counting == 0)
               return; 
            --Reference_counting;
        }
        
        int Get_Reference_counting(){
            return Reference_counting; 
        }
    
    private:
        ValueType type;
        void* data;
        int Reference_counting = 0;

        const long long* getIntData() const {
            return static_cast<long long*>(data);
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
         const FuncObj* getFuncData() const {
            return static_cast<FuncObj*>(data);
        }


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
                    case ValueType::FUNC:
                        delete getFuncData();
                        
                        break;

                    default:
                        break;
                }
            }
        }
};
