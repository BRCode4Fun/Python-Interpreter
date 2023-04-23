#include <iostream>
#include <string>
#include <vector>
#include <memory>

using namespace std;


class Value {
public:
    enum class Type {
        Undefined,
        Number,
        Boolean,
        String,
        List
    };

    Value() : type(Type::Undefined) {}
    Value(double v) : type(Type::Number), numberValue(v) {}
    Value(bool v) : type(Type::Boolean), booleanValue(v) {}
    Value(const string& v) : type(Type::String), stringValue(v) {}
    Value(const vector<Value>& v) : type(Type::List), listValue(v) {}

    Type getType() const {
        return type;
    }

    double toNumber() const {
        if (type != Type::Number) {
            throw runtime_error("Value is not a number");
        }
        return numberValue;
    }

    bool toBoolean() const {
        if (type != Type::Boolean) {
            throw runtime_error("Value is not a boolean");
        }
        return booleanValue;
    }

    string toString() const {
        if (type != Type::String) {
            throw runtime_error("Value is not a string");
        }
        return stringValue;
    }

    vector<Value> toList() const {
        if (type != Type::List) {
            throw runtime_error("Value is not a list");
        }
        return listValue;
    }


    Type type;
    double numberValue;
    bool booleanValue;
    string stringValue;
    vector<Value> listValue;
};