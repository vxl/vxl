#ifndef reg_bvxm_h_
#define reg_bvxm_h_

#include <bpro/bpro_batch/bpro_batch.h>

static PyObject *register_processes(PyObject *self, PyObject *args);
static PyObject *register_datatypes(PyObject *self, PyObject *args);
static PyMethodDef bvxm_batch_methods[METHOD_NUM+2];

#endif
