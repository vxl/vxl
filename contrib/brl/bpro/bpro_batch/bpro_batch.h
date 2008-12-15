// This is brl/bpro/bpro_batch/bpro_batch.h
#ifndef bpro_batch_h_
#define bpro_batch_h_
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

//extern PyObject *
//register_processes(PyObject *self, PyObject *args);

//extern PyObject *
//register_datatypes(PyObject *self, PyObject *args);

const int METHOD_NUM = 23;
static PyObject *init_process(PyObject *self, PyObject *args);
static PyObject *set_input_bool(PyObject *self, PyObject *args);
static PyObject *set_input_string(PyObject *self, PyObject *args);
static PyObject *set_input_int(PyObject *self, PyObject *args);
static PyObject *set_input_unsigned(PyObject *self, PyObject *args);
static PyObject *set_input_long(PyObject *self, PyObject *args);
static PyObject *set_input_float(PyObject *self, PyObject *args);
static PyObject *set_input_double(PyObject *self, PyObject *args);
static PyObject *get_input_float(PyObject *self, PyObject *args);
static PyObject *get_input_unsigned(PyObject *self, PyObject *args);
static PyObject *process_print_default_params(PyObject *self, PyObject *args);
static PyObject *process_init(PyObject *self, PyObject *args);
static PyObject *set_params_process(PyObject *self, PyObject *args);
static PyObject *run_process(PyObject *self, PyObject *args);
static PyObject *commit_output(PyObject *self, PyObject *args);
static PyObject *set_input_from_db(PyObject *self, PyObject *args);
static PyObject *remove_data(PyObject *self, PyObject *args);
static PyObject *print_db(PyObject *self, PyObject *args);
static PyObject *clear(PyObject *self, PyObject *args);

extern PyMethodDef batch_methods[];

#endif // bpro_batch_h_
