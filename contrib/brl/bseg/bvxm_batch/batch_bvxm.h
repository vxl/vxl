// This is brl/bseg/bvxm_batch/batch_bvxm.h
#ifndef batch_bvxm_h_
#define batch_bvxm_h_
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

#endif // batch_bvxm_h_
