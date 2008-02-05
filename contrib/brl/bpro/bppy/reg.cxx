#include "batch.h"
#include <bprb/bprb_macros.h>

PyObject *
register_processes(PyObject *self, PyObject *args)
{
  REG_PROCESS(bprb_null_process, bprb_batch_process_manager);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *
register_datatypes(PyObject *self, PyObject *args)
{
  REGISTER_DATATYPE(bool);
  REGISTER_DATATYPE(vcl_string);
  REGISTER_DATATYPE(int);
  REGISTER_DATATYPE(long);
  REGISTER_DATATYPE(float);
  REGISTER_DATATYPE(double);
	Py_INCREF(Py_None);
	return Py_None;
}

