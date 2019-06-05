#include "reg_bvpl.h"
#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>

// processes
#include <bvpl/pro/bvpl_register.h>
#include <bvpl/kernels/pro/bvpl_kernels_register.h>
#include <bvxm/grid/pro/bvxm_grid_register.h>
#include <bvxm/algo/pro/bvxm_algo_register.h>

PyObject *
register_processes(PyObject *self, PyObject *args)
{
  bvpl_register::register_process();
  bvpl_kernels_register::register_process();
  bvxm_grid_register::register_process();
  bvxm_algo_register::register_process();
  Py_INCREF(Py_None);
  return Py_None;
}

PyObject *
register_datatypes(PyObject *self, PyObject *args)
{
  register_basic_datatypes();
  bvpl_register::register_datatype();
  bvpl_kernels_register::register_datatype();
  bvxm_grid_register::register_datatype();
  bvxm_algo_register::register_datatype();
  Py_INCREF(Py_None);
  return Py_None;
}

void init_bvpl_batch_methods(void)
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

  bvpl_batch_methods[0]=reg_pro;
  bvpl_batch_methods[1]=reg_data;

  for (int i=0; i<METHOD_NUM; ++i) {
    bvpl_batch_methods[i+2]=batch_methods[i];
  }
}

// python3
#if PY_MAJOR_VERSION >= 3
PyMODINIT_FUNC
PyInit_bvpl_batch(void)
{
  init_bvpl_batch_methods();
  static struct PyModuleDef bvpl_batch_def = {
      PyModuleDef_HEAD_INIT,
      "bvpl_batch",        // m_name
      "bvpl_batch module", // m_doc
      -1,                  // m_size
      bvpl_batch_methods,  // m_methods
    };
  return PyModule_Create(&bvpl_batch_def);
}

// python2
#else
PyMODINIT_FUNC
initbvpl_batch(void)
{
  init_bvpl_batch_methods();
  Py_InitModule("bvpl_batch", bvpl_batch_methods);
}

#endif
