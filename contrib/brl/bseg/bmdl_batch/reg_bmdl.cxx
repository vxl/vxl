#include "reg_bmdl.h"
#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>
// processes
#include <vil_pro/vil_register.h>
#include <vpgl_pro/vpgl_register.h>

#include <bmdl/pro/bmdl_processes.h>
#include <bmdl/pro/bmdl_register.h>

PyObject *
register_processes(PyObject *self, PyObject *args)
{
  bmdl_register::register_process();
  Py_INCREF(Py_None);
  return Py_None;
}

PyObject *
register_datatypes(PyObject *self, PyObject *args)
{
  register_basic_datatypes();
  vil_register::register_datatype();
  vpgl_register::register_datatype();

  Py_INCREF(Py_None);
  return Py_None;
}

PyMODINIT_FUNC
initbmdl_batch(void)
{
  PyMethodDef reg_pro;
  reg_pro.ml_name = "register_processes";
  reg_pro.ml_meth = register_processes;
  reg_pro.ml_doc = "register_processes() create instances of each defined process";
  reg_pro.ml_flags = METH_VARARGS;

  PyMethodDef reg_data;
  reg_data.ml_name = "register_datatypes";
  reg_data.ml_meth = register_datatypes;
  reg_data.ml_doc = "register_datatypes() insert tables in the database for each type";
  reg_data.ml_flags = METH_VARARGS;

  bmdl_batch_methods[0]=reg_pro;
  bmdl_batch_methods[1]=reg_data;

  for (int i=0; i<METHOD_NUM; ++i) {
    bmdl_batch_methods[i+2]=batch_methods[i];
  }

  Py_InitModule("bmdl_batch", bmdl_batch_methods);
}
