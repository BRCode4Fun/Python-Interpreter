#include "./pyInt.hpp"
#include "./pyFloat.hpp"
#include "./pyBool.hpp"

PyInt::PyInt(const std::string& v)
    : PyObject(ObjectType::Int, new ll(std::stoll(v))) {}

PyInt::PyInt(ll v)
    : PyObject(ObjectType::Int, new ll(v)) {}

PyObject* PyInt::operator+(const PyObject& other) const {
    if (other.isInt()) {
        const PyInt* rhs = dynamic_cast<const PyInt*>(&other);
        return new PyInt(getInt() + rhs->getInt());
    } else if(other.isFloat()) {
        const PyFloat* rhs = dynamic_cast<const PyFloat*>(&other);
        return new PyFloat((llf)getInt() + rhs->getFloat());
    } else {
        throw std::runtime_error("Unsupported operands for +.");
    }
 }

PyObject* PyInt::operator-(const PyObject& other) const {
    if (other.isInt()) {
        const PyInt* rhs = dynamic_cast<const PyInt*>(&other);
        return new PyInt(getInt() - rhs->getInt());
    } else if(other.isFloat()) {
        const PyFloat* rhs = dynamic_cast<const PyFloat*>(&other);
        return new PyFloat((llf)getInt() - rhs->getFloat());
    } else {
        throw std::runtime_error("Unsupported operands for -.");
    }
}

PyObject* PyInt::operator*(const PyObject& other) const {
    if (other.isInt()) {
        const PyInt* rhs = dynamic_cast<const PyInt*>(&other);
        return new PyInt(getInt() * rhs->getInt());
    } else if(other.isFloat()) {
        const PyFloat* rhs = dynamic_cast<const PyFloat*>(&other);
        return new PyFloat(static_cast<llf>(getInt()) * rhs->getFloat());
    } else {
        throw std::runtime_error("Unsupported operands for *.");
    }
}

PyObject* PyInt::operator/(const PyObject& other) const {
    if (other.isInt()) {
        const PyInt* rhs = dynamic_cast<const PyInt*>(&other);
        llf rvalue = static_cast<llf>(rhs->getInt());
        if(rvalue == 0.0) throw std::runtime_error("Attempted to divide by zero");
        return new PyFloat(static_cast<llf>(getInt()) / rvalue);
    } else if(other.isFloat()) {
        const PyFloat* rhs = dynamic_cast<const PyFloat*>(&other);
        llf rvalue = rhs->getFloat();
        if(rvalue == 0.0) throw std::runtime_error("Attempted to divide by zero");
        return new PyFloat(static_cast<llf>(getInt()) / rvalue);
    } else {
        throw std::runtime_error("Unsupported operands for /.");
    }
}

PyObject* PyInt::operator%(const PyObject& other) const {
    if (other.isInt()) {
        const PyInt* rhs = dynamic_cast<const PyInt*>(&other);
        ll rvalue = rhs->getInt();
        if(rvalue == 0L) throw std::runtime_error("Modulo by zero");
        return new PyInt(getInt() % rvalue);
    } else {
        throw std::runtime_error("Unsupported operands for %.");
    }
}

PyObject* PyInt::operator&(const PyObject& other) const {
    if (other.isInt()) {
        const PyInt* rhs = dynamic_cast<const PyInt*>(&other);
        return new PyInt(getInt() & rhs->getInt());
    } else {
        throw std::runtime_error("Unsupported operands for &.");
    }
}

PyObject* PyInt::operator<<(const PyObject& other) const {
    if (other.isInt()) {
        const PyInt* rhs = dynamic_cast<const PyInt*>(&other);
        return new PyInt(getInt() << rhs->getInt());
    } else {
        throw std::runtime_error("Unsupported operands for <<.");
    }
}

PyObject* PyInt::operator>>(const PyObject& other) const {
    if (other.isInt()) {
        const PyInt* rhs = dynamic_cast<const PyInt*>(&other);
        return new PyInt(getInt() >> rhs->getInt());
    } else {
        throw std::runtime_error("Unsupported operands for >>.");
    }
}

PyObject* PyInt::operator|(const PyObject& other) const {
    if (other.isInt()) {
        const PyInt* rhs = dynamic_cast<const PyInt*>(&other);
        return new PyInt(getInt() | rhs->getInt());
    } else {
        throw std::runtime_error("Unsupported operands for |.");
    }
}

PyObject* PyInt::operator^(const PyObject& other) const {
    if (other.isInt()) {
        const PyInt* rhs = dynamic_cast<const PyInt*>(&other);
        return new PyInt(getInt() ^ rhs->getInt());
    } else {
        throw std::runtime_error("Unsupported operands for ^.");
    }
}

PyObject* PyInt::operator==(const PyObject& other) const {
    if (other.isInt()) {
        const PyInt* rhs = dynamic_cast<const PyInt*>(&other);
        return new PyBool(getInt() == rhs->getInt());
    } else if(other.isFloat()) {
        const PyFloat* rhs = dynamic_cast<const PyFloat*>(&other);
        return new PyBool(static_cast<llf>(getInt()) == rhs->getFloat());
    } else {
        return new PyBool(false);
    }
}

PyObject* PyInt::operator<(const PyObject& other) const {
    if (other.isInt()) {
        const PyInt* rhs = dynamic_cast<const PyInt*>(&other);
        return new PyBool(getInt() < rhs->getInt());
    } else if(other.isFloat()) {
        const PyFloat* rhs = dynamic_cast<const PyFloat*>(&other);
        return new PyBool(static_cast<llf>(getInt()) < rhs->getFloat());
    } else {
        throw std::runtime_error("Unsupported operands for <.");
    }
}

PyObject* PyInt::operator>(const PyObject& other) const {
    if (other.isInt()) {
        const PyInt* rhs = dynamic_cast<const PyInt*>(&other);
        return new PyBool(getInt() > rhs->getInt());
    } else if(other.isFloat()) {
        const PyFloat* rhs = dynamic_cast<const PyFloat*>(&other);
        return new PyBool(static_cast<llf>(getInt()) > rhs->getFloat());
    } else {
        throw std::runtime_error("Unsupported operands for >.");
    }
}

PyObject* PyInt::operator-() const {
    return new PyInt(-getInt());
}

PyObject* PyInt::operator~() const {
    return new PyInt(~getInt());
}

PyObject* PyInt::operator!() const {
    return new PyBool(!(this->isTruthy()));
}

ll PyInt::getInt() const {
    return *getIntData();
}

void PyInt::write(std::ostream& out) const {
    out << getInt();
}

const ll* PyInt::getIntData() const {
    return static_cast<const ll*>(data);
}

void PyInt::deleteData() {
    delete getIntData();
}

