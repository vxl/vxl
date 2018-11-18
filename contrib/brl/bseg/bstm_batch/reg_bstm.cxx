#include "reg_bstm.h"
#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <brdb/brdb_selection.h>

// processes
#include <bstm/cpp/pro/bstm_cpp_register.h>
#include <bstm/pro/bstm_register.h>
#include <bstm/ocl/pro/bstm_ocl_register.h>

#include <boxm2/pro/boxm2_register.h>

#include <boxm2/cpp/pro/boxm2_cpp_register.h>
#include <vpgl_pro/vpgl_register.h>
#include <vil_pro/vil_register.h>
#include <sdet_pro/sdet_register.h>
#include <brip_pro/brip_register.h>
#if defined(HAS_OPENCL) && HAS_OPENCL
#include <bocl/pro/bocl_register.h>
#include <boxm2/ocl/pro/boxm2_ocl_register.h>
#if defined(HAS_GLEW) && HAS_GLEW
#include <boxm2/view/pro/boxm2_view_register.h>
#endif
#endif
#include <bbas_pro/bbas_register.h>
#include <brad_pro/brad_register.h>
#include <bsta/pro/bsta_register.h>
#include <bvpl/kernels/pro/bvpl_kernels_register.h>
#include <ihog_pro/ihog_register.h>
#if defined(HAS_HDFS) && HAS_HDFS
#include <bhdfs/pro/bhdfs_register.h>
#endif

PyObject *
register_processes(PyObject *self, PyObject *args)
{

  boxm2_register::register_process();
  boxm2_cpp_register::register_process();
  vpgl_register::register_process();
  vil_register::register_process();
  brip_register::register_process();
  bvpl_kernels_register::register_process();
#if defined(HAS_OPENCL) && HAS_OPENCL
  bocl_register::register_process();
  boxm2_ocl_register::register_process();
  bstm_ocl_register::register_process();
#if defined(HAS_GLEW) && HAS_GLEW
  boxm2_view_register::register_process();
#endif
#endif
  bbas_register::register_process();
  brad_register::register_process();
  bsta_register::register_process();
  ihog_register::register_process();
  sdet_register::register_process();
#if defined(HAS_HDFS) && HAS_HDFS
  bhdfs_register::register_process();
#endif

  bstm_register::register_process();
  bstm_cpp_register::register_process();


  Py_INCREF(Py_None);
  return Py_None;
}


PyObject *
register_datatypes(PyObject *self, PyObject *args)
{
  register_basic_datatypes();



  boxm2_register::register_datatype();
  boxm2_cpp_register::register_datatype();
  vpgl_register::register_datatype();
  vil_register::register_datatype();
  brip_register::register_datatype();
  bvpl_kernels_register::register_datatype();
#if defined(HAS_OPENCL) && HAS_OPENCL
  bocl_register::register_datatype();
  boxm2_ocl_register::register_datatype();
  bstm_ocl_register::register_datatype();
#if defined(HAS_GLEW) && HAS_GLEW
  boxm2_view_register::register_datatype();
#endif
#endif
  bbas_register::register_datatype();
  sdet_register::register_datatype();
  brad_register::register_datatype();
  bsta_register::register_datatype();
  ihog_register::register_datatype();
#if defined(HAS_HDFS) && HAS_HDFS
  bhdfs_register::register_datatype();
#endif

  bstm_register::register_datatype();
  bstm_cpp_register::register_datatype();


  Py_INCREF(Py_None);
  return Py_None;
}


PyMODINIT_FUNC
initbstm_batch(void)
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

  bstm_batch_methods[0]=reg_pro;
  bstm_batch_methods[1]=reg_data;

  for (int i=0; i<METHOD_NUM; ++i) {
    bstm_batch_methods[i+2]=batch_methods[i];
  }

  Py_InitModule("bstm_batch", bstm_batch_methods);
}
