#ifndef reg_betr_h_
#define reg_betr_h_

#include <bpro/bpro_batch/bpro_batch.h>

static PyObject *register_processes(PyObject *self, PyObject *args);
static PyObject *register_datatypes(PyObject *self, PyObject *args);
static PyMethodDef betr_batch_methods[METHOD_NUM+2];

#endif
