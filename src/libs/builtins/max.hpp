#pragma once

#include "../value/primitives.hpp"



bool compute_max_int(PyObject * i,  PyObject * j);

PyObject* max_builtin(std::vector<PyObject*>& values);
