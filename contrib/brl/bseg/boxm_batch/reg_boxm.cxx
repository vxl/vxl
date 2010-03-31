#include "reg_boxm.h"
#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>

// processes
#include <vil_pro/vil_register.h>
#include <vpgl_pro/vpgl_register.h>
#include <boxm/pro/boxm_register.h>
#include <sdet_pro/sdet_register.h>
#include <boxm/opt/pro/boxm_opt_register.h>
#if (HAS_OPENGL)
#include <boxm/opt/open_cl/pro/boxm_opt_opencl_register.h>
#endif
#include <boxm/algo/pro/boxm_algo_register.h>
#include <bvpl/bvpl_octree/pro/bvpl_octree_register.h>
#include <bvxm/grid/pro/bvxm_grid_register.h>

PyObject *
register_processes(PyObject *self, PyObject *args)
{
  vil_register::register_process();
  vpgl_register::register_process();
  boxm_register::register_process();
  boxm_opt_register::register_process();
  boxm_algo_register::register_process();
  sdet_register::register_process();
  bvxm_grid_register::register_process();
  bvpl_octree_register::register_process();
#if (HAS_OPENGL)
  boxm_opt_opencl_register::register_process();
#endif
  Py_INCREF(Py_None);
  return Py_None;
}

PyObject *
register_datatypes(PyObject *self, PyObject *args)
{
  register_basic_datatypes();
  vil_register::register_datatype();
  vpgl_register::register_datatype();
  boxm_register::register_datatype();
  boxm_opt_register::register_datatype();
  boxm_algo_register::register_datatype();
  bvxm_grid_register::register_datatype();
#if (HAS_OPENGL)
  boxm_opt_opencl_register::register_datatype();
#endif
  Py_INCREF(Py_None);
  return Py_None;
}

PyMODINIT_FUNC
initboxm_batch(void)
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

  boxm_batch_methods[0]=reg_pro;
  boxm_batch_methods[1]=reg_data;

  for (int i=0; i<METHOD_NUM; ++i) {
    boxm_batch_methods[i+2]=batch_methods[i];
  }

  Py_InitModule("boxm_batch", boxm_batch_methods);
}
