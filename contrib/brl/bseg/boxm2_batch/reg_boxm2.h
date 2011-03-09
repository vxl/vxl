#ifndef reg_boxm2_h_
#define reg_boxm2_h_

#include <bpro/bpro_batch/bpro_batch.h>

static PyObject *register_processes(PyObject *self, PyObject *args);
static PyObject *register_datatypes(PyObject *self, PyObject *args);

static PyMethodDef boxm2_batch_methods[METHOD_NUM+2];

#endif
