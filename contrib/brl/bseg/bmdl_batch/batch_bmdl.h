// This is brl/bseg/bmdl_batch/batch_bmdl.h
#ifndef batch_bmdl_h_
#define batch_bmdl_h_
//:
// \file
// \brief External function declarations
// \author J.L. Mundy
// \date February 5, 2008
//
//
// \verbatim
//  Modifications
// \endverbatim
#include "Python.h"


extern PyObject *
register_processes(PyObject *self, PyObject *args);

extern PyObject *
register_datatypes(PyObject *self, PyObject *args);

#endif // batch_bmdl_h_
