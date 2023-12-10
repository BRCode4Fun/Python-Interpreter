#include "./max.hpp"


bool compute_max_int(PyObject * i,  PyObject * j)
{
   return static_cast<PyInt*>(i)->getInt() < static_cast<PyInt *>(j)->getInt();
}

PyObject* max_builtin(std::vector<PyObject*>& values)
{

  if (values.empty())
  {
    throw std::runtime_error("Max expected at least 1 argument");
  }

  bool has_double =  false;

  return *std::max_element(values.begin(), values.end() , compute_max_int);
}
