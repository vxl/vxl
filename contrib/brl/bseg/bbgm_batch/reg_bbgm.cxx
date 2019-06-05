#include "reg_bbgm.h"
#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>
// processes
#include <vil_pro/vil_register.h>
#include <vidl_pro/vidl_register.h>

#include <bbgm/pro/bbgm_processes.h>
#include <bbgm/pro/bbgm_register.h>

PyObject *
register_processes(PyObject *self, PyObject *args)
{
  bbgm_register::register_process();
  vil_register::register_process();
  vidl_register::register_process();
  Py_INCREF(Py_None);
  return Py_None;
}

PyObject *
register_datatypes(PyObject *self, PyObject *args)
{
  register_basic_datatypes();
  vil_register::register_datatype();
  vidl_register::register_datatype();
  bbgm_register::register_datatype();
  Py_INCREF(Py_None);
  return Py_None;
}

void init_bbgm_batch_methods(void)
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

  bbgm_batch_methods[0]=reg_pro;
  bbgm_batch_methods[1]=reg_data;

  for (int i=0; i<METHOD_NUM; ++i) {
    bbgm_batch_methods[i+2]=batch_methods[i];
  }
}

// python3
#if PY_MAJOR_VERSION >= 3
PyMODINIT_FUNC
PyInit_bbgm_batch(void)
{
  init_bbgm_batch_methods();
  static struct PyModuleDef bbgm_batch_def = {
      PyModuleDef_HEAD_INIT,
      "bbgm_batch",        // m_name
      "bbgm_batch module", // m_doc
      -1,                  // m_size
      bbgm_batch_methods,  // m_methods
    };
  return PyModule_Create(&bbgm_batch_def);
}

// python2
#else
PyMODINIT_FUNC
initbbgm_batch(void)
{
  init_bbgm_batch_methods();
  Py_InitModule("bbgm_batch", bbgm_batch_methods);
}

#endif
