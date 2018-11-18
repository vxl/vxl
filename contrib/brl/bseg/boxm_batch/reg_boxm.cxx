#include "reg_boxm.h"
#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <brdb/brdb_selection.h>

// processes
#include <vil_pro/vil_register.h>
#include <vpgl_pro/vpgl_register.h>
#include <sdet_pro/sdet_register.h>
//#include <icam_pro/icam_register.h>
#include <boxm/pro/boxm_register.h>
#include <boxm/algo/pro/boxm_algo_register.h>
#include <boxm/algo/rt/pro/boxm_algo_rt_register.h>
#include <boxm/algo/sp/pro/boxm_algo_sp_register.h>

#include <bsta/pro/bsta_register.h>
#include <bapl/pro/bapl_register.h>

#include <bbas_pro/bbas_register.h>

#include <boxm/basic/boxm_util_data_types.h>
PyObject *
register_processes(PyObject *self, PyObject *args)
{
  vil_register::register_process();
  vpgl_register::register_process();
  boxm_register::register_process();
  boxm_algo_register::register_process();

  boxm_algo_rt_register::register_process();
  boxm_algo_sp_register::register_process();
  sdet_register::register_process();
  //icam_register::register_process();
  bapl_register::register_process();
  bbas_register::register_process();
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
  boxm_algo_register::register_process();

  boxm_algo_rt_register::register_datatype();
  boxm_algo_sp_register::register_datatype();

  bsta_register::register_datatype();
  //icam_register::register_datatype();
  bapl_register::register_datatype();
  bbas_register::register_datatype();
  Py_INCREF(Py_None);
  return Py_None;
}


PyObject *get_boxm_array_1d_float(PyObject * /*self*/, PyObject *args)
{
  unsigned id;
  boxm_array_1d_float_sptr value;
  if (!PyArg_ParseTuple(args, "i:get_boxm_array_1d_float", &id))
    return NULL;

  std::string relation_name = "boxm_array_1d_float_sptr_data";

  // query to get the data
  brdb_query_aptr Q = brdb_query_comp_new("id", brdb_query::EQ, id);
  brdb_selection_sptr selec = DATABASE->select(relation_name, std::move(Q));
  PyObject *array_1d=0;
  if (selec->size()!=1) {
    std::cout << "in get_boxm_array_1d_float() - no relation with type" << relation_name << " id: " << id << std::endl;

    return array_1d;
  }

  brdb_value_sptr brdb_value;
  if (!selec->get_value(std::string("value"), brdb_value)) {
    std::cout << "in get_boxm_array_1d_float() didn't get value\n";
    return array_1d;
  }

  if (!brdb_value) {
    std::cout << "in get_boxm_array_1d_float() - null value\n";
    return array_1d;
  }

  brdb_value_t<boxm_array_1d_float_sptr>* result_out = static_cast<brdb_value_t<boxm_array_1d_float_sptr>* >(brdb_value.ptr());
  value = result_out->value();

  array_1d = PyList_New(value->data_array.size());
  PyObject *x;
  for(unsigned i=0;i<value->data_array.size();i++)
  {
    x=PyFloat_FromDouble((double)value->data_array[i]);
    PyList_SetItem(array_1d, i,x);//Py_DECREF(x);
  }
  Py_INCREF(array_1d);
  return array_1d;
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

  PyMethodDef get_boxm_array_1d_float_func;
  get_boxm_array_1d_float_func.ml_name = "get_boxm_array_1d_float";
  get_boxm_array_1d_float_func.ml_meth = get_boxm_array_1d_float;
  get_boxm_array_1d_float_func.ml_doc = "get_boxm_array_1d_float() Get 1-d array in python";
  get_boxm_array_1d_float_func.ml_flags = METH_VARARGS;

  boxm_batch_methods[0]=reg_pro;
  boxm_batch_methods[1]=reg_data;
  boxm_batch_methods[2]=get_boxm_array_1d_float_func;

  for (int i=0; i<METHOD_NUM; ++i) {
    boxm_batch_methods[i+3]=batch_methods[i];
  }

  Py_InitModule("boxm_batch", boxm_batch_methods);
}
