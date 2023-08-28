#include "./pyFloat.hpp"
#include "./pyInt.hpp"
#include "./pyBool.hpp"

PyFloat::PyFloat(const std::string& v) 
    : PyObject(ObjectType::Float, new llf(std::stold(v))) {}
    
PyFloat::PyFloat(llf v)
    : PyObject(ObjectType::Float, new llf(v)) {}

PyObject* PyFloat::operator+(const PyObject& other) const {
    if (other.isInt()) {
        const PyInt* rhs = dynamic_cast<const PyInt*>(&other);
        return new PyFloat(getFloat() + static_cast<llf>(rhs->getInt()));
    } else if(other.isFloat()) {
        const PyFloat* rhs = dynamic_cast<const PyFloat*>(&other);
        return new PyFloat(getFloat() + rhs->getFloat());
    } else {
        throw std::runtime_error("Unsupported operands for +.");
    }
}

PyObject* PyFloat::operator-(const PyObject& other) const {
    if (other.isInt()) {
        const PyInt* rhs = dynamic_cast<const PyInt*>(&other);
        return new PyFloat(getFloat() - static_cast<llf>(rhs->getInt()));
    } else if(other.isFloat()) {
        const PyFloat* rhs = dynamic_cast<const PyFloat*>(&other);
        return new PyFloat(getFloat() - rhs->getFloat());
    } else {
        throw std::runtime_error("Unsupported operands for -.");
    }
}

PyObject* PyFloat::operator*(const PyObject& other) const {
    if (other.isInt()) {
        const PyInt* rhs = dynamic_cast<const PyInt*>(&other);
        return new PyFloat(getFloat() * static_cast<llf>(rhs->getInt()));
    } else if(other.isFloat()) {
        const PyFloat* rhs = dynamic_cast<const PyFloat*>(&other);
        return new PyFloat(getFloat() * rhs->getFloat());
    } else {
        throw std::runtime_error("Unsupported operands for *.");
    }
}

PyObject* PyFloat::operator/(const PyObject& other) const {
    if (other.isInt()) {
        const PyInt* rhs = dynamic_cast<const PyInt*>(&other);
        llf rvalue = static_cast<llf>(rhs->getInt());
        if(rvalue == 0.0) throw std::runtime_error("Attempted to divide by zero");
        return new PyFloat(getFloat() / rvalue);
    } else if(other.isFloat()) {
        const PyFloat* rhs = dynamic_cast<const PyFloat*>(&other);
        llf rvalue = rhs->getFloat();
        if(rvalue == 0.0) throw std::runtime_error("Attempted to divide by zero");
        return new PyFloat(getFloat() / rvalue);
    } else {
        throw std::runtime_error("Unsupported operands for /.");
    }
}

PyObject* PyFloat::operator==(const PyObject& other) const {
    if (other.isInt()) {
        const PyInt* rhs = dynamic_cast<const PyInt*>(&other);
        return new PyBool(getFloat() == static_cast<llf>(rhs->getInt()));
    } else if(other.isFloat()) {
        const PyFloat* rhs = dynamic_cast<const PyFloat*>(&other);
        return new PyBool(getFloat() == rhs->getFloat());
    } else {
        return new PyBool(false);
    }
}

PyObject* PyFloat::operator<(const PyObject& other) const {
    if (other.isInt()) {
        const PyInt* rhs = dynamic_cast<const PyInt*>(&other);
        return new PyBool(getFloat() < static_cast<llf>(rhs->getInt()));
    } else if(other.isFloat()) {
        const PyFloat* rhs = dynamic_cast<const PyFloat*>(&other);
        return new PyBool(getFloat() < rhs->getFloat());
    } else {
        throw std::runtime_error("Unsupported operands for <.");
    }
}

PyObject* PyFloat::operator>(const PyObject& other) const {
    if (other.isInt()) {
        const PyInt* rhs = dynamic_cast<const PyInt*>(&other);
        return new PyBool(getFloat() > static_cast<llf>(rhs->getInt()));
    } else if(other.isFloat()) {
        const PyFloat* rhs = dynamic_cast<const PyFloat*>(&other);
        return new PyBool(getFloat() > rhs->getFloat());
    } else {
        throw std::runtime_error("Unsupported operands for >.");
    }
}

PyObject* PyFloat::operator-() const {
    return new PyFloat(-getFloat());
}

PyObject* PyFloat::operator!() const {
    return new PyBool(!(this->isTruthy()));
}

llf PyFloat::getFloat() const {
    return *getFloatData();
}

void PyFloat::write(std::ostream& out) const {
    out << getFloat();
}

const llf* PyFloat::getFloatData() const {
    return static_cast<llf*>(data);
}

void PyFloat::deleteData() {
    delete getFloatData();
}
