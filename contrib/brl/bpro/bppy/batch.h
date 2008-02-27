// This is brl/bpro/bppy/batch.h
#ifndef batch_h_
#define batch_h_
//:
// \file
// \brief External function declarations
// \author J.L. Mundy
// \date February 5, 2008
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
#include "Python.h"

extern PyObject *
register_processes(PyObject *self, PyObject *args);

extern PyObject *
register_datatypes(PyObject *self, PyObject *args);

#endif // batch_h_
