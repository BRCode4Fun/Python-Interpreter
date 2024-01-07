#include "./pyInstance.hpp"
#include "./pyClass.hpp"

PyInstance::PyInstance(PyClass* klass)
 : PyObject(ObjectType::Instance), base(klass){
    fields = new Scope(klass->getContext());
}

PyObject* PyInstance::operator==(const PyObject& other) const {
    return new PyBool(other.isNone() ? false : true);
}

void PyInstance::write(std::ostream& out) const {
    out << "<\'" << base->kname << "\' instance>";
}
