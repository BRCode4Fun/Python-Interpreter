#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include "../ast/ast.hpp"
#include "./FuncObject.hpp"

using llf = long double;
using lld = long long int;

class Value {
    
public:
    enum class ValueType {
        NONE,
        INT, FLOAT,
        BOOLEAN,
        STRING,
        LIST, 
        FUNC 
    };
      
    Value() : type(ValueType::NONE), data(nullptr) {}
        
    explicit Value(lld v) : type(ValueType::INT), data(new lld(v)) {}
    explicit Value(llf v) : type(ValueType::FLOAT), data(new llf(v)) {}
    explicit Value(bool v) : type(ValueType::BOOLEAN), data(new bool(v)) {}
    explicit Value(const std::string& v) : type(ValueType::STRING), data(new std::string(v)) {}
    explicit Value(const std::vector<Value>& v) : type(ValueType::LIST), data(new std::vector<Value>(v)) {}
    explicit Value(FunctionNode* func) : type(ValueType::FUNC), data(new FuncObj(func)) {}

        // copy constructor
        Value(const Value& other) : type(other.type), data(nullptr) {
            switch (type) {
                case ValueType::INT:
                    data = new lld(*other.getIntData());
                    break;
                case ValueType::BOOLEAN:
                    data = new bool(*other.getBoolData());
                    break;    
                case ValueType::FLOAT:
                    data = new llf(*other.getFloatData());
                    break;
                case ValueType::STRING:
                    data = new std::string(*other.getStringData());
                    break;
                case ValueType::LIST:
                    data = new std::vector<Value>(*other.getListData());
                    break;
                case ValueType::FUNC:
                    data = new FuncObj(other.getFuncData()->function_node);
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

        lld getInt() const {
            if (not is_int()) {
                throw std::runtime_error("Value is not an integer.");
            }
            return *getIntData();
        }
        llf getFloat() const {
            if (not is_float()) {
                throw std::runtime_error("Value is not a float.");
            }
            return *getFloatData();
        }
        bool getBoolean() const {
            if (not is_bool()) {
                throw std::runtime_error("Value is not a boolean.");
            }
            return *getBoolData();
        }
        const std::string& getString() const {
            if (not is_string()) {
                throw std::runtime_error("Value is not a std::string.");
            }
            return *getStringData();
        }
        const std::vector<Value>& getList() const {
            if (not is_list()) {
                throw std::runtime_error("Value is not a list.");
            }
            return *getListData();
        }

        const FuncObj getFunc() const {
            if (not is_function()) {
                throw std::runtime_error("Value is not a Function.");
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
                        data = new lld(*other.getIntData());
                        break;
                    case ValueType::FLOAT:
                        data = new llf(*other.getFloatData());
                        break;
                    case ValueType::BOOLEAN:
                        data = new bool(*other.getBoolData());
                        break;
                    case ValueType::STRING:
                        data = new std::string(*other.getStringData());
                        break;
                    case ValueType::LIST:
                        data = new std::vector<Value>(*other.getListData());
                        break;
                    case ValueType::FUNC:
                        data = new FuncObj(other.getFuncData()->function_node);
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
                        return new Value(getFloat() + (llf)other.getInt());
                    case ValueType::BOOLEAN:
                        return new Value(getFloat() + (llf)other.getBoolean());
                    default:
                        throw std::runtime_error("Unsupported operands for +.");
                }
            } else if((*this).is_int()) {
                switch(other.type) {
                    case ValueType::INT:
                        return new Value(getInt() + other.getInt());
                    case ValueType::FLOAT:
                        return new Value((llf)getInt() + other.getFloat());
                    case ValueType::BOOLEAN:
                        return new Value(getInt() + (lld)other.getBoolean());
                    default:
                        throw std::runtime_error("Unsupported operands for +.");
                }
            } else if((*this).is_bool()) {
                switch(other.type) {
                    case ValueType::INT:
                        return new Value((lld)getBoolean() + other.getInt());
                    case ValueType::FLOAT:
                        return new Value((llf)getBoolean() + other.getFloat());
                    case ValueType::BOOLEAN:
                        return new Value((lld)getBoolean() + (lld)other.getBoolean());
                    default:
                        throw std::runtime_error("Unsupported operands for +.");
                }
            } else if((*this).is_string() and other.is_string()) {  
                return new Value(getString() + other.getString());
            } else {
                throw std::runtime_error("Unsupported operands for +.");
            }
        }
        
        // TODO: replace with call to __mul__ later
        Value* operator*(const Value& other) const {
            
            if((*this).is_float()) {
                switch(other.type) {
                    case ValueType::FLOAT:
                        return new Value(getFloat() * other.getFloat());
                    case ValueType::INT:
                        return new Value(getFloat() * (llf)other.getInt());
                    case ValueType::BOOLEAN:
                        return new Value(getFloat() * (llf)other.getBoolean());
                    default:
                        throw std::runtime_error("Unsupported operands for *.");
                }
            } else if((*this).is_int()) {
                switch(other.type) {
                    case ValueType::INT:
                        return new Value(getInt() * other.getInt());
                    case ValueType::FLOAT:
                        return new Value((llf)getInt() * other.getFloat());
                    case ValueType::BOOLEAN:
                        return new Value(getInt() * (lld)other.getBoolean());
                    case ValueType::STRING:
                        throw std::logic_error("Function not implemented yet");
                    default:
                        throw std::runtime_error("Unsupported operands for *.");
                }
            } else if((*this).is_bool()) {
                switch(other.type) {
                    case ValueType::INT:
                        return new Value((lld)getBoolean() * other.getInt());
                    case ValueType::FLOAT:
                        return new Value((llf)getBoolean() * other.getFloat());
                    case ValueType::BOOLEAN:
                        return new Value((lld)getBoolean() * (lld)other.getBoolean());
                    default:
                        throw std::runtime_error("Unsupported operands for *.");
                }
            } else if((*this).is_string() and other.is_int()) {  
                throw std::logic_error("Feature not implemented yet");
            } else {
                throw std::runtime_error("Unsupported operands for *.");
            }
        }
        
        // TODO: replace with call to __truediv__ later
        Value* operator/(const Value& other) const {
            
            if((*this).is_float()) {
                switch(other.type) {
                    case ValueType::FLOAT: {
                        llf rvalue = other.getFloat();
                        if(rvalue == 0.0) throw std::runtime_error("Attempted to divide by zero");
                        else return new Value(getFloat() / rvalue);
                    } 
                    case ValueType::INT: {
                        llf rvalue = (llf) other.getInt();
                        if(rvalue == 0.0) throw std::runtime_error("Attempted to divide by zero");
                        else return new Value(getFloat() / rvalue);
                    }
                    case ValueType::BOOLEAN: {    
                        llf rvalue = (llf) other.getBoolean();
                        if(rvalue == 0.0) throw std::runtime_error("Attempted to divide by zero");
                        else return new Value(getFloat() / rvalue);
                    }
                    default:
                        throw std::runtime_error("Unsupported operands for /.");
                }
            } else if((*this).is_int()) {
                switch(other.type) {
                    case ValueType::INT: {
                        lld rvalue = other.getInt();
                        if(rvalue == 0L) throw std::runtime_error("Attempted to divide by zero");
                        return new Value(getInt() / rvalue);
                    }
                    case ValueType::FLOAT: {
                        llf rvalue = other.getFloat();
                        if(rvalue == 0.0) throw std::runtime_error("Attempted to divide by zero");
                        return new Value((llf)getInt() / rvalue);
                    }
                    case ValueType::BOOLEAN: {
                        lld rvalue = (lld) other.getBoolean();
                        if(rvalue == 0L) throw std::runtime_error("Attempted to divide by zero");
                        return new Value(getInt() / rvalue);
                    }
                    default:
                        throw std::runtime_error("Unsupported operands for /.");
                }
            } else if((*this).is_bool()) {
                switch(other.type) {
                    case ValueType::INT: {
                        lld rvalue = other.getInt();
                        if(rvalue == 0L) throw std::runtime_error("Attempted to divide by zero");
                        else return new Value((lld)getBoolean() / rvalue);
                    }
                    case ValueType::FLOAT: {
                        llf rvalue = other.getFloat();
                        if(rvalue == 0.0) throw std::runtime_error("Attempted to divide by zero");
                        else return new Value((llf)getBoolean() / rvalue);
                    }
                    case ValueType::BOOLEAN: {
                        lld rvalue = (lld) other.getBoolean();
                        if(rvalue == 0L) throw std::runtime_error("Attempted to divide by zero");
                        else return new Value((lld)getBoolean() / rvalue);
                    }
                    default:
                        throw std::runtime_error("Unsupported operands for /.");
                }
            } else {
                throw std::runtime_error("Unsupported operands for /.");
            }
        }
        
        // TODO: replace with call to __mod__ later
        Value* operator%(const Value& other) const {
            
            if((*this).is_float()) {
                throw std::logic_error("Feature not implemented yet");
                
            } else if((*this).is_int()) {
                switch(other.type) {
                    case ValueType::INT: {
                        lld rvalue = other.getInt();
                        if(rvalue == 0L) throw std::runtime_error("Modulo by zero");
                        return new Value(getInt() % rvalue);
                    }
                    case ValueType::BOOLEAN: {
                        lld rvalue = (lld) other.getBoolean();
                        if(rvalue == 0L) throw std::runtime_error("Modulo by zero");
                        return new Value(getInt() % rvalue);
                    }
                    case ValueType::FLOAT: {
                        throw std::logic_error("Feature not implemented yet");
                    }
                    default:
                        throw std::runtime_error("Unsupported operands for %.");
                }
            } else if((*this).is_bool()) {
                switch(other.type) {
                    case ValueType::INT: {
                        lld rvalue = other.getInt();
                        if(rvalue == 0L) throw std::runtime_error("Modulo by zero");
                        else return new Value((lld)getBoolean() % rvalue);
                    }
                    case ValueType::FLOAT: {
                        throw std::logic_error("Feature not implemented yet");
                    }
                    case ValueType::BOOLEAN: {
                        lld rvalue = (lld) other.getBoolean();
                        if(rvalue == 0L) throw std::runtime_error("Modulo by zero");
                        else return new Value((lld)getBoolean() % rvalue);
                    }
                    default:
                        throw std::runtime_error("Unsupported operands for %.");
                }
            } else {
                throw std::runtime_error("Unsupported operands for %.");
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
                    return new Value(-(lld)getBoolean());
                default:
                    throw std::runtime_error("Unsupported operands for unary -.");
            }
        }
        
        // TODO: replace with call to __inv__ later
        Value* operator~() {
            switch(type) {
                case ValueType::INT:
                    return new Value(~getInt());
                case ValueType::BOOLEAN:
                    return new Value(~(lld)getBoolean());
                default:
                    throw std::runtime_error("Unsupported operands for unary .");
            }
        }
        
        // TODO: replace with call to __not__ later
        Value* operator!() {
            return new Value(!(this->isTruthy())); 
        }
        
        // TODO: replace with call to __sub__ later
        Value* operator-(const Value& other) const {
            
            if((*this).is_float()) {
                switch(other.type) {
                    case ValueType::FLOAT:
                        return new Value(getFloat() - other.getFloat());
                    case ValueType::INT:
                        return new Value(getFloat() - (llf)other.getInt());
                    case ValueType::BOOLEAN:
                        return new Value(getFloat() - (llf)other.getBoolean());
                    default:
                        throw std::runtime_error("Unsupported operands for -.");
                }
            } else if((*this).is_int()) {
                switch(other.type) {
                    case ValueType::INT:
                        return new Value(getInt() - other.getInt());
                    case ValueType::FLOAT:
                        return new Value((llf)getInt() - other.getFloat());
                    case ValueType::BOOLEAN:
                        return new Value(getInt() - (lld)other.getBoolean());
                    default:
                        throw std::runtime_error("Unsupported operands for -.");
                }
            } else if((*this).is_bool()) {
                switch(other.type) {
                    case ValueType::BOOLEAN:
                        return new Value((lld)getBoolean() - (lld)other.getBoolean());
                    case ValueType::INT:
                        return new Value((lld)getBoolean() - other.getInt());
                    case ValueType::FLOAT:
                        return new Value((llf)getBoolean() - other.getFloat());
                    default:
                        throw std::runtime_error("Unsupported operands for -.");
                }
            } else {
                throw std::runtime_error("Unsupported operands for -.");
            }
        }
        // TODO: replace with call to __and__ later
        Value* operator&(const Value& other) const {
            
            if((*this).is_int()) {
                switch(other.type) {
                    case ValueType::INT:
                        return new Value(getInt() & other.getInt());
                    case ValueType::BOOLEAN:
                        return new Value(getInt() & (lld)other.getBoolean());
                    default:
                        throw std::runtime_error("Unsupported operands for &.");
                }
            } else if((*this).is_bool()) {
                switch(other.type) {
                    case ValueType::BOOLEAN:
                        return new Value((lld)getBoolean() & (lld)other.getBoolean());
                    case ValueType::INT:
                        return new Value((lld)getBoolean() & other.getInt());
                    default:
                        throw std::runtime_error("Unsupported operands for &.");
                }
            } else {
                throw std::runtime_error("Unsupported operands for &.");
            }
        }
        
        // TODO: replace with call to __or__ later
        Value* operator|(const Value& other) const {
            
            if((*this).is_int()) {
                switch(other.type) {
                    case ValueType::INT:
                        return new Value(getInt() | other.getInt());
                    case ValueType::BOOLEAN:
                        return new Value(getInt() | (lld)other.getBoolean());
                    default:
                        throw std::runtime_error("Unsupported operands for |.");
                }
            } else if((*this).is_bool()) {
                switch(other.type) {
                    case ValueType::BOOLEAN:
                        return new Value((lld)getBoolean() | (lld)other.getBoolean());
                    case ValueType::INT:
                        return new Value((lld)getBoolean() | other.getInt());
                    default:
                        throw std::runtime_error("Unsupported operands for |.");
                }
            } else {
                throw std::runtime_error("Unsupported operands for |.");
            }
        }

        // TODO: replace with call to __xor__ later
        Value* operator^(const Value& other) const {
            
            if((*this).is_int()) {
                switch(other.type) {
                    case ValueType::INT:
                        return new Value(getInt() ^ other.getInt());
                    case ValueType::BOOLEAN:
                        return new Value(getInt() ^ (lld)other.getBoolean());
                    default:
                        throw std::runtime_error("Unsupported operands for ^.");
                }
            } else if((*this).is_bool()) {
                switch(other.type) {
                    case ValueType::BOOLEAN:
                        return new Value((lld)getBoolean() ^ (lld)other.getBoolean());
                    case ValueType::INT:
                        return new Value((lld)getBoolean() ^ other.getInt());
                    default:
                        throw std::runtime_error("Unsupported operands for ^");
                }
            } else {
                throw std::runtime_error("Unsupported operands for ^.");
            }
        }

        // TODO: replace with call to __lshift__ later
        Value* operator<<(const Value& other) const {
            
            if((*this).is_int()) {
                switch(other.type) {
                    case ValueType::INT:
                        return new Value(getInt() << other.getInt());
                    case ValueType::BOOLEAN:
                        return new Value(getInt() << (lld)other.getBoolean());
                    default:
                        throw std::runtime_error("Unsupported operands for <<.");
                }
            } else if((*this).is_bool()) {
                switch(other.type) {
                    case ValueType::BOOLEAN:
                        return new Value((lld)getBoolean() << (lld)other.getBoolean());
                    case ValueType::INT:
                        return new Value((lld)getBoolean() << other.getInt());
                    default:
                        throw std::runtime_error("Unsupported operands for <<.");
                }
            } else {
                throw std::runtime_error("Unsupported operands for <<.");
            }
        }

        // TODO: replace with call to __rshift__ later
        Value* operator>>(const Value& other) const {
            
            if((*this).is_int()) {
                switch(other.type) {
                    case ValueType::INT:
                        return new Value(getInt() >> other.getInt());
                    case ValueType::BOOLEAN:
                        return new Value(getInt() >> (lld)other.getBoolean());
                    default:
                        throw std::runtime_error("Unsupported operands for >>.");
                }
            } else if((*this).is_bool()) {
                switch(other.type) {
                    case ValueType::BOOLEAN:
                        return new Value((lld)getBoolean() >> (lld)other.getBoolean());
                    case ValueType::INT:
                        return new Value((lld)getBoolean() >> other.getInt());
                    default:
                        throw std::runtime_error("Unsupported operands for >>.");
                }
            } else {
                throw std::runtime_error("Unsupported operands for >>.");
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
                        return new Value((lld)getBoolean() == other.getInt());
                        break;
                    case ValueType::FLOAT:
                        return new Value((llf)getBoolean() == other.getFloat());
                        break;
                    default:
                        throw std::runtime_error("Unsupported operands for ==.");
                } 
            } else if((*this).is_int()) {
                switch(other.type) {
                    case ValueType::INT:
                        return new Value(getInt() == other.getInt());
                        break;
                    case ValueType::FLOAT:
                        return new Value((llf)getInt() == other.getFloat());
                        break;
                    case ValueType::BOOLEAN:
                        return new Value(getInt() == (lld)other.getBoolean());
                        break;
                    default:
                        throw std::runtime_error("Unsupported operands for ==.");
                }
                
            } else if((*this).is_float()) {
                switch(other.type) {
                    case ValueType::FLOAT:
                        return new Value(getFloat() == other.getFloat());
                        break;
                    case ValueType::INT:
                        return new Value(getFloat() == (llf)other.getInt());
                        break;
                    case ValueType::BOOLEAN:
                        return new Value(getFloat() == (llf)other.getBoolean());
                        break;
                    default:
                        throw std::runtime_error("Unsupported operands for ==.");
                }
            } else {
                throw std::runtime_error("Unsupported operands for equality.");
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
                        return new Value((llf)getInt() < other.getFloat());
                        break;
                    case ValueType::BOOLEAN:
                        return new Value(getInt() < (lld)other.getBoolean());
                        break;
                    default:
                        throw std::runtime_error("Unsupported operands for comparison.");
                }
            } else if((*this).is_float()) {
                switch(other.type) {
                    case ValueType::FLOAT:
                        return new Value(getFloat() < other.getFloat());
                        break;
                    case ValueType::INT:
                        return new Value(getFloat() < (llf)other.getInt());
                        break;
                    case ValueType::BOOLEAN:
                        return new Value(getFloat() < (llf)other.getBoolean());
                        break;
                    default:
                        throw std::runtime_error("Unsupported operands for comparison.");
                }
            } else if((*this).is_bool()) {
                switch(other.type) {
                    case ValueType::BOOLEAN:
                        return new Value(getBoolean() < other.getBoolean());
                        break;
                    case ValueType::INT:
                        return new Value((lld)getBoolean() < other.getInt());
                        break;
                    case ValueType::FLOAT:
                        return new Value((llf)getBoolean() < other.getFloat());
                        break;
                    default:
                        throw std::runtime_error("Unsupported operands for comparison.");
                }
            } else {
                throw std::runtime_error("Unsupported operands for comparison.");
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
                        return new Value((llf)getInt() > other.getFloat());
                        break;
                    case ValueType::BOOLEAN:
                        return new Value(getInt() > (lld)other.getBoolean());
                        break;
                    default:
                        throw std::runtime_error("Unsupported operands for comparison.");
                }
            } else if((*this).is_float()) {
                switch(other.type) {
                    case ValueType::FLOAT:
                        return new Value(getFloat() > other.getFloat());
                        break;
                    case ValueType::INT:
                        return new Value(getFloat() > (llf)other.getInt());
                        break;
                    case ValueType::BOOLEAN:
                        return new Value(getFloat() > (llf)other.getBoolean());
                        break;
                    default:
                        throw std::runtime_error("Unsupported operands for comparison.");
                }
            } else if((*this).is_bool()) {
                switch(other.type) {
                    case ValueType::BOOLEAN:
                        return new Value(getBoolean() > other.getBoolean());
                        break;
                    case ValueType::INT:
                        return new Value((lld)getBoolean() > other.getInt());
                        break;
                    case ValueType::FLOAT:
                        return new Value((llf)getBoolean() > other.getFloat());
                        break;
                    default:
                        throw std::runtime_error("Unsupported operands for comparison.");
                }
            } else {
                throw std::runtime_error("Unsupported operands for comparison.");
            }
        }

        // TODO: replace with __repr__ later
        friend std::ostream& operator<<(std::ostream& out, const Value& value) {
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
                throw std::runtime_error("Yet not printable object.");
            }
            return out;
        }
        
        // TODO: replace with truth or __bool__ later
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
                    throw std::runtime_error("Yet not evaluatable object.");
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

        const lld* getIntData() const {
            return static_cast<lld*>(data);
        }
        const llf* getFloatData() const {
            return static_cast<llf*>(data);
        }
        const bool* getBoolData() const {
            return static_cast<bool*>(data);
        }
        const std::string* getStringData() const {
            return static_cast<std::string*>(data);
        } 
        const std::vector<Value>* getListData() const {
            return static_cast<std::vector<Value>*>(data);
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
