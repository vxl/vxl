#include "reg_brec.h"
#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>
// processes
#include <vil_pro/vil_register.h>
#include <vpgl_pro/vpgl_register.h>

#include <bvxm/pro/bvxm_register.h>
#include <bbgm/pro/bbgm_register.h>
#include <vidl_pro/vidl_register.h>

#include <brec/pro/brec_processes.h>
#include <brec/pro/brec_register.h>

#include <bvgl_pro/bvgl_processes.h>
#include <bvgl_pro/bvgl_register.h>
#include <brip_pro/brip_register.h>
#include <bxml/bsvg/pro/bsvg_register.h>
#include <bxml/bsvg/pro/bsvg_processes.h>

PyObject *
register_processes(PyObject *self, PyObject *args)
{
  vil_register::register_process();
  vpgl_register::register_process();
  brec_register::register_process();
  bbgm_register::register_process();
  bvxm_register::register_process();
  vidl_register::register_process();
  bvgl_register::register_process();
  brip_register::register_process();
  bsvg_register::register_process();
  Py_INCREF(Py_None);
  return Py_None;
}

PyObject *
register_datatypes(PyObject *self, PyObject *args)
{
  register_basic_datatypes();
  vil_register::register_datatype();
  vpgl_register::register_datatype();
  bbgm_register::register_datatype();
  bvxm_register::register_datatype();
  brec_register::register_datatype();
  vidl_register::register_datatype();
  bvgl_register::register_datatype();
  brip_register::register_datatype();
  bsvg_register::register_datatype();

  Py_INCREF(Py_None);
  return Py_None;
}

PyMODINIT_FUNC
initbrec_batch(void)
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

  brec_batch_methods[0]=reg_pro;
  brec_batch_methods[1]=reg_data;

  for (int i=0; i<METHOD_NUM; ++i) {
    brec_batch_methods[i+2]=batch_methods[i];
  }

  Py_InitModule("brec_batch", brec_batch_methods);
}
