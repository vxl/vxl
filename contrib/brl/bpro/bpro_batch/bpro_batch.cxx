#include <string>
#include <iostream>
#include "bpro_batch.h"

#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_macros.h>

#include <brdb/brdb_value.h>
#include <brdb/brdb_query.h>
#include <brdb/brdb_selection.h>
#include <brdb/brdb_database_manager.h>
#include <bpro/core/bbas_pro/bbas_1d_array_float.h>
#include <bpro/core/bbas_pro/bbas_1d_array_double.h>
#include <bpro/core/bbas_pro/bbas_1d_array_string.h>
#include <bpro/core/bbas_pro/bbas_1d_array_int.h>
#include <bpro/core/bbas_pro/bbas_1d_array_unsigned.h>
#include <bpro/core/bbas_pro/bbas_1d_array_byte.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

static PyObject *init_process(PyObject *self, PyObject *args);
static PyObject *set_input_bool(PyObject *self, PyObject *args);
static PyObject *set_input_string(PyObject *self, PyObject *args);
static PyObject *set_input_string_array(PyObject *self, PyObject *args);
static PyObject *set_input_int(PyObject *self, PyObject *args);
static PyObject *set_input_int_array(PyObject *self, PyObject *args);
static PyObject *set_input_unsigned(PyObject *self, PyObject *args);
static PyObject *set_input_unsigned_array(PyObject *self, PyObject *args);
static PyObject *set_input_long(PyObject *self, PyObject *args);
static PyObject *set_input_float(PyObject *self, PyObject *args);
static PyObject *set_input_float_array(PyObject *self, PyObject *args);
static PyObject *set_input_double(PyObject * self, PyObject *args);
static PyObject *set_input_double_array(PyObject *self, PyObject *args);
static PyObject *get_output_string(PyObject *self, PyObject *args);
static PyObject *get_output_float(PyObject *self, PyObject *args);
static PyObject *get_output_double(PyObject *self, PyObject *args);
static PyObject *get_output_int(PyObject *self, PyObject *args);
static PyObject *get_output_unsigned(PyObject *self, PyObject *args);
static PyObject *process_print_default_params(PyObject *self, PyObject *args);
static PyObject *process_init(PyObject *self, PyObject *args);
static PyObject *set_params_process(PyObject *self, PyObject *args);
static PyObject *run_process(PyObject *self, PyObject *args);
static PyObject *finish_process(PyObject *self, PyObject *args);
static PyObject *verbose(PyObject *self, PyObject *args);
static PyObject *not_verbose(PyObject *self, PyObject *args);
static PyObject *commit_output(PyObject *self, PyObject *args);
static PyObject *set_input_from_db(PyObject* self, PyObject *args);
static PyObject *remove_data(PyObject *self, PyObject *args);
static PyObject *remove_data_obj(PyObject *self, PyObject *args);
static PyObject *print_db(PyObject *self, PyObject *args);
static PyObject *clear(PyObject *self, PyObject *args);
static PyObject *get_bbas_1d_array_float(PyObject *self, PyObject *args);
static PyObject *get_output_double_array(PyObject *self, PyObject *args);
static PyObject *get_bbas_1d_array_int(PyObject *self, PyObject *args);
static PyObject *get_bbas_1d_array_byte(PyObject *self, PyObject *args);
static PyObject *get_bbas_1d_array_unsigned(PyObject *self, PyObject *args);
static PyObject *get_bbas_1d_array_string(PyObject *self, PyObject *args);
static PyObject *set_stdout(PyObject *self, PyObject *args);
static PyObject *reset_stdout(PyObject *self, PyObject *args);

// a flag to stop informative printout of arguments
static bool verbose_state = true;

PyMethodDef batch_methods[] =
  {
#if 0
    {"register_processes", register_processes, METH_VARARGS,
     "register_processes() create instances of each defined process"},
    {"register_datatypes", register_datatypes, METH_VARARGS,
     "register_datatypes() insert tables in the database for each type"},
#endif // 0
    #include "bpro_defs.h"
#if 0
    {initp, init_process, METH_VARARGS,  initp_com},
    {proc_print_def, process_print_default_params, METH_VARARGS, proc_print_def_com},
    {set_prm_proc, set_params_process, METH_VARARGS, set_prm_proc_com},
    {set_in_bool, set_input_bool, METH_VARARGS, set_in_bool_com},
    {set_in_string, set_input_string, METH_VARARGS, set_in_string_com},
    {set_in_int, set_input_int, METH_VARARGS,set_in_int_com},
    {set_in_unsign, set_input_unsigned, METH_VARARGS,set_in_unsign_com},
    {set_in_long, set_input_long, METH_VARARGS,set_in_long_com},
    {set_in_float, set_input_float, METH_VARARGS,set_in_float_com},
    {set_in_double, set_input_double, METH_VARARGS,set_in_double_com},
    {get_in_float, get_input_float, METH_VARARGS,get_in_float_com},
    {get_in_unsigned, get_input_unsigned, METH_VARARGS,get_in_unsigned_com},
    {proc_init, process_init, METH_VARARGS,proc_init_com},
    {run_proc, run_process, METH_VARARGS,run_proc_com},
    {fin_proc, finish_process, METH_VARARGS,fin_proc_com},
    {verb, verbose, METH_VARARGS,verb_com},
    {not_verb, not_verbose, METH_VARARGS,not_verb_com},
    {comt_out, commit_output, METH_VARARGS,comt_out_com},
    {set_idb, set_input_from_db, METH_VARARGS,set_idb_com},
    {rm_dat, remove_data, METH_VARARGS,rm_dat_com},
    {rm_dato, remove_data_obj, METH_VARARGS, rm_dato_com},
    {pdb, print_db, METH_VARARGS,pdb_com },
    {clr, clear, METH_VARARGS, clr_com},
#endif
    {NULL, NULL, 0, NULL} // trailing "NULL process" to indicate end of list
  };

PyObject *init_process(PyObject * /*self*/, PyObject *args)
{
  const char* name;
  if (!PyArg_ParseTuple(args, "s:init_process", &name))
    return NULL;
  std::string n(name);
  if (verbose_state)
    std::cout << n << '\n';
  bool result = bprb_batch_process_manager::instance()->init_process(n);
  return Py_BuildValue("b", result);
}

PyObject *set_input_bool(PyObject * /*self*/, PyObject *args)
{
  int input;
  bool value;
  if (!PyArg_ParseTuple(args, "ib:set_input_bool", &input, &value))
    return NULL;
  brdb_value_sptr v = new brdb_value_t<bool>(value);
  if (verbose_state)
    std::cout << "input[" << input << "](bool): " << value << '\n';
  bool result = bprb_batch_process_manager::instance()->set_input(input, v);
  return Py_BuildValue("b", result);
}

PyObject *
set_input_string(PyObject * /*self*/, PyObject *args)
{
  int input;
  const char* value;
  if (!PyArg_ParseTuple(args, "is:set_input_string", &input, &value))
    return NULL;
  brdb_value_sptr v = new brdb_value_t<std::string>(value);
  if (verbose_state)
    std::cout << "input[" << input << "](string): " << value << '\n';
  bool result = bprb_batch_process_manager::instance()->set_input(input, v);
  return Py_BuildValue("b", result);
}

PyObject *set_input_string_array(PyObject * /*self*/, PyObject *args){
        int input;
        PyObject *list;
        if(!PyArg_ParseTuple(args, "iO:set_input_string_array", &input, &list))
                return NULL;

        if(!PyList_Check(list))
                return Py_BuildValue("b", false);

        unsigned int length=PyList_GET_SIZE(list);

        bbas_1d_array_string_sptr sarray = new bbas_1d_array_string(length);
        for(unsigned i = 0; i < length; ++i){
                PyObject *obj = PyList_GetItem(list,i);
                std::string s = static_cast<std::string>(PyString_AsString(obj));
                sarray->data_array[i] = s;
        }
        brdb_value_sptr v = new brdb_value_t<bbas_1d_array_string_sptr>(sarray);
        bool result = bprb_batch_process_manager::instance()->set_input(input,v);
        return Py_BuildValue("b", result);
}

PyObject *set_input_unsigned(PyObject * /*self*/, PyObject *args)
{
  int input;
  unsigned ivalue;
  if (!PyArg_ParseTuple(args, "ii:set_input_unsigned", &input, &ivalue))
    return NULL;
  brdb_value_sptr iv = new brdb_value_t<unsigned>(ivalue);
  if (verbose_state)
    std::cout << "input[" << input << "](unsigned): " << ivalue << '\n';
  bool result = bprb_batch_process_manager::instance()->set_input(input, iv);
  return Py_BuildValue("b", result);
}

PyObject *set_input_unsigned_array(PyObject * /*self*/, PyObject *args)
{
  int input;
  PyObject  * list;
  if (!PyArg_ParseTuple(args, "iO:set_input_int_array", &input, &list))
    return NULL;

  if (!PyList_Check(list))
    return Py_BuildValue("b", false);
  unsigned int length=PyList_GET_SIZE(list);

  bbas_1d_array_unsigned_sptr farray=new bbas_1d_array_unsigned(length);
  for (unsigned i = 0; i < length; ++i)
  {
    PyObject * obj = PyList_GetItem(list,i);
    unsigned f=(unsigned)PyInt_AsLong(obj);
    farray->data_array[i]=f;
  }

  brdb_value_sptr v = new brdb_value_t<bbas_1d_array_unsigned_sptr>(farray);
  bool result = bprb_batch_process_manager::instance()->set_input(input, v);
  return Py_BuildValue("b", result);
}

PyObject *set_input_int(PyObject * /*self*/, PyObject *args)
{
  int input;
  int ivalue;
  if (!PyArg_ParseTuple(args, "ii:set_input_int", &input, &ivalue))
    return NULL;
  brdb_value_sptr iv = new brdb_value_t<int>(ivalue);
  if (verbose_state)
    std::cout << "input[" << input << "](int): " << ivalue << '\n';
  bool result = bprb_batch_process_manager::instance()->set_input(input, iv);
  return Py_BuildValue("b", result);
}

PyObject *set_input_int_array(PyObject * /*self*/, PyObject *args)
{
  int input;
  PyObject  * list;
  if (!PyArg_ParseTuple(args, "iO:set_input_int_array", &input, &list))
    return NULL;

  if (!PyList_Check(list))
    return Py_BuildValue("b", false);
  unsigned int length=PyList_GET_SIZE(list);

  bbas_1d_array_int_sptr farray=new bbas_1d_array_int(length);
  for (unsigned i = 0; i < length; ++i)
  {
    PyObject * obj = PyList_GetItem(list,i);
    int f=(int)PyInt_AsLong(obj);
    farray->data_array[i]=f;
  }

  brdb_value_sptr v = new brdb_value_t<bbas_1d_array_int_sptr>(farray);
  bool result = bprb_batch_process_manager::instance()->set_input(input, v);
  return Py_BuildValue("b", result);
}

PyObject *set_input_long(PyObject * /*self*/, PyObject *args)
{
  int input;
  long value;
  if (!PyArg_ParseTuple(args, "il:set_input_long", &input, &value))
    return NULL;
  brdb_value_sptr v = new brdb_value_t<long>(value);
  if (verbose_state)
    std::cout << "input[" << input << "](long): " << value << '\n';
  bool result = bprb_batch_process_manager::instance()->set_input(input, v);
  return Py_BuildValue("b", result);
}

PyObject *set_input_float(PyObject * /*self*/, PyObject *args)
{
  int input;
  float value;
  if (!PyArg_ParseTuple(args, "if:set_input_float", &input, &value))
    return NULL;
  brdb_value_sptr v = new brdb_value_t<float>(value);
  if (verbose_state)
    std::cout << "input[" << input << "](float): " << value << '\n';
  bool result = bprb_batch_process_manager::instance()->set_input(input, v);
  return Py_BuildValue("b", result);
}

PyObject *set_input_float_array(PyObject * /*self*/, PyObject *args)
{
  int input;
  PyObject  * list;
  if (!PyArg_ParseTuple(args, "iO:set_input_float_array", &input, &list))
    return NULL;

  if (!PyList_Check(list))
    return Py_BuildValue("b", false);
  unsigned int length=PyList_GET_SIZE(list);

  bbas_1d_array_float_sptr farray=new bbas_1d_array_float(length);
  for (unsigned i = 0; i < length; ++i)
  {
    PyObject * obj = PyList_GetItem(list,i);
    float f=PyFloat_AsDouble(obj);
    farray->data_array[i]=f;
  }

  brdb_value_sptr v = new brdb_value_t<bbas_1d_array_float_sptr>(farray);
  bool result = bprb_batch_process_manager::instance()->set_input(input, v);
  return Py_BuildValue("b", result);
}

PyObject *set_input_double(PyObject * /*self*/, PyObject *args)
{
  int input;
  double value;
  if (!PyArg_ParseTuple(args, "id:set_input_double", &input, &value))
    return NULL;
  brdb_value_sptr v = new brdb_value_t<double>(value);
  if (verbose_state)
    std::cout << "input[" << input << "](double): " << value << '\n';
  bool result = bprb_batch_process_manager::instance()->set_input(input, v);
  return Py_BuildValue("b", result);
}

PyObject *set_input_double_array(PyObject * /*self*/, PyObject *args)
{
  int input;
  PyObject  * list;
  if (!PyArg_ParseTuple(args, "iO:set_input_double_array", &input, &list))
    return NULL;

  if (!PyList_Check(list))
    return Py_BuildValue("b", false);
  Py_ssize_t length=PyList_GET_SIZE(list);

  bbas_1d_array_double_sptr farray=new bbas_1d_array_double(length);
  for (Py_ssize_t i = 0; i < length; ++i)
  {
    PyObject * obj = PyList_GetItem(list,i);
    farray->data_array[i]=PyFloat_AsDouble(obj);
  }

  brdb_value_sptr v = new brdb_value_t<bbas_1d_array_double_sptr>(farray);
  bool result = bprb_batch_process_manager::instance()->set_input(input, v);
  return Py_BuildValue("b", result);
}

PyObject *get_output_string(PyObject * /*self*/, PyObject *args)
{
  unsigned id;
  std::string value;
  if (!PyArg_ParseTuple(args, "i:get_output_string", &id))
    return NULL;

  std::string relation_name = "vcl_string_data";

  // query to get the data
  brdb_query_aptr Q = brdb_query_comp_new("id", brdb_query::EQ, id);
  brdb_selection_sptr selec = DATABASE->select(relation_name, std::move(Q));

  if (selec->size()!=1) {
    std::cout << "in get_output_string() - no relation with type" << relation_name << " id: " << id << std::endl;
    return Py_BuildValue("s", "");
  }

  brdb_value_sptr brdb_value;
  if (!selec->get_value(std::string("value"), brdb_value)) {
    std::cout << "in get_output_string() didn't get value\n";
    return Py_BuildValue("s","");
  }

  if (!brdb_value) {
    std::cout << "in get_output_string() - null value\n";
    return Py_BuildValue("s","");
  }
  brdb_value_t<std::string>* result_out = static_cast<brdb_value_t<std::string>* >(brdb_value.ptr());
  value = result_out->value();

  if( verbose_state )
    std::cout << " In get_output_string, the output string is: "
             << value << std::endl;

  return Py_BuildValue("s", value.c_str());
}


PyObject *get_output_float(PyObject * /*self*/, PyObject *args)
{
  unsigned id;
  float value;
  if (!PyArg_ParseTuple(args, "i:get_output_float", &id))
    return NULL;

  std::string relation_name = "float_data";

  // query to get the data
  brdb_query_aptr Q = brdb_query_comp_new("id", brdb_query::EQ, id);
  brdb_selection_sptr selec = DATABASE->select(relation_name, std::move(Q));

  if (selec->size()!=1) {
    std::cout << "in get_output_float() - no relation with type" << relation_name << " id: " << id << std::endl;
    return Py_BuildValue("f",-1.0);
  }

  brdb_value_sptr brdb_value;
  if (!selec->get_value(std::string("value"), brdb_value)) {
    std::cout << "in get_output_float() didn't get value\n";
    return Py_BuildValue("f",-1.0);
  }

  if (!brdb_value) {
    std::cout << "in get_output_float() - null value\n";
    return Py_BuildValue("f",-1.0);
  }
  brdb_value_t<float>* result_out = static_cast<brdb_value_t<float>* >(brdb_value.ptr());
  value = result_out->value();

  return Py_BuildValue("f", value);
}

PyObject *get_output_double(PyObject * /*self*/, PyObject *args)
{
  unsigned id;
  double value;
  if (!PyArg_ParseTuple(args, "i:get_output_double", &id))
    return NULL;

  std::string relation_name = "double_data";

  // query to get the data
  brdb_query_aptr Q = brdb_query_comp_new("id", brdb_query::EQ, id);
  brdb_selection_sptr selec = DATABASE->select(relation_name, std::move(Q));

  if (selec->size()!=1) {
    std::cout << "in get_output_double() - no relation with type" << relation_name << " id: " << id << std::endl;
    return Py_BuildValue("d",-1.0);
  }

  brdb_value_sptr brdb_value;
  if (!selec->get_value(std::string("value"), brdb_value)) {
    std::cout << "in get_output_double() didn't get value\n";
    return Py_BuildValue("d",-1.0);
  }

  if (!brdb_value) {
    std::cout << "in get_output_double() - null value\n";
    return Py_BuildValue("d",-1.0);
  }
  brdb_value_t<double>* result_out = static_cast<brdb_value_t<double>* >(brdb_value.ptr());
  value = result_out->value();

  return Py_BuildValue("d", value);
}

PyObject *get_output_int(PyObject * /*self*/, PyObject *args)
{
  unsigned id;
  unsigned value;
  if (!PyArg_ParseTuple(args, "i:get_output_int", &id))
    return NULL;

  std::string relation_name = "int_data";

  // query to get the data
  brdb_query_aptr Q = brdb_query_comp_new("id", brdb_query::EQ, id);
  brdb_selection_sptr selec = DATABASE->select(relation_name, std::move(Q));

  if (selec->size()!=1) {
    std::cout << "in get_output_int() - no relation with type" << relation_name << " id: " << id << std::endl;
    return Py_BuildValue("i",1000);
  }

  brdb_value_sptr brdb_value;
  if (!selec->get_value(std::string("value"), brdb_value)) {
    std::cout << "in get_output_int() didn't get value\n";
    return Py_BuildValue("i",1000);
  }

  if (!brdb_value) {
    std::cout << "in get_output_int() - null value\n";
    return Py_BuildValue("i",1000);
  }
  brdb_value_t<int>* result_out = static_cast<brdb_value_t<int>* >(brdb_value.ptr());
  value = result_out->value();

  return Py_BuildValue("i", value);
}

PyObject *get_output_unsigned(PyObject * /*self*/, PyObject *args)
{
  unsigned id;
  unsigned value;
  if (!PyArg_ParseTuple(args, "i:get_output_unsigned", &id))
    return NULL;

  std::string relation_name = "unsigned_data";

  // query to get the data
  brdb_query_aptr Q = brdb_query_comp_new("id", brdb_query::EQ, id);
  brdb_selection_sptr selec = DATABASE->select(relation_name, std::move(Q));

  if (selec->size()!=1) {
    std::cout << "in get_output_unsigned() - no relation with type" << relation_name << " id: " << id << std::endl;
    return Py_BuildValue("I",1000);
  }

  brdb_value_sptr brdb_value;
  if (!selec->get_value(std::string("value"), brdb_value)) {
    std::cout << "in get_output_unsigned() didn't get value\n";
    return Py_BuildValue("I",1000);
  }

  if (!brdb_value) {
    std::cout << "in get_output_unsigned() - null value\n";
    return Py_BuildValue("I",1000);
  }
  brdb_value_t<unsigned>* result_out = static_cast<brdb_value_t<unsigned>* >(brdb_value.ptr());
  value = result_out->value();

  return Py_BuildValue("I", value);
}

PyObject *process_print_default_params(PyObject * /*self*/, PyObject *args)
{
  const char* name;
  const char* value;
  if (!PyArg_ParseTuple(args, "ss:process_print_default_params", &name, &value))
    return NULL;

  std::string n(name);
  std::string f(value);

  bool result = bprb_batch_process_manager::instance()->print_default_params(n, f);
  return Py_BuildValue("b", result);
}

PyObject *process_init(PyObject * /*self*/, PyObject * /*args*/)
{
  bool result = bprb_batch_process_manager::instance()->process_init();
  return Py_BuildValue("b", result);
}

PyObject *set_params_process(PyObject * /*self*/, PyObject *args)
{
  const char* value;
  if (!PyArg_ParseTuple(args, "s:set_params_process", &value))
    return NULL;

  std::string f(value);

  bool result = bprb_batch_process_manager::instance()->set_params(f);
  return Py_BuildValue("b", result);
}

PyObject *run_process(PyObject * /*self*/, PyObject * /*args*/)
{
  bool result = bprb_batch_process_manager::instance()->run_process();
  return Py_BuildValue("b", result);
}

PyObject *finish_process(PyObject * /*self*/, PyObject * /*args*/)
{
  bool result = bprb_batch_process_manager::instance()->finish_process();
  return Py_BuildValue("b", result);
}

PyObject *verbose(PyObject * /*self*/, PyObject * /*args*/)
{
  bool result = bprb_batch_process_manager::instance()->verbose();
  verbose_state = result;
  return Py_BuildValue("b", result);
}

PyObject *not_verbose(PyObject * /*self*/, PyObject * /*args*/)
{
  bool result = bprb_batch_process_manager::instance()->not_verbose();
  verbose_state = result;
  return Py_BuildValue("b", result);
}

PyObject *commit_output(PyObject * /*self*/, PyObject *args)
{
  unsigned id;
  unsigned output;
  if (!PyArg_ParseTuple(args, "i:commit_output", &output))
    return NULL;
  std::string type;
  bool result = bprb_batch_process_manager::instance()->commit_output(output,id, type);
  if (!result)
    return Py_BuildValue("i", -1);
  else
    return Py_BuildValue("is", id, type.c_str());
}

PyObject *set_input_from_db(PyObject * /*self*/, PyObject *args)
{
  unsigned input;
  bool result = false;
  PyObject* obj;
  if (!PyArg_ParseTuple(args, "iO:set_input_from_db", &input, &obj))
    return NULL;

  if (PyObject_HasAttrString(obj, "type") && PyObject_HasAttrString(obj, "id")) {
    PyObject* type_obj = PyObject_GetAttrString(obj,"type");
    PyObject* id_obj = PyObject_GetAttrString(obj,"id");

    if (PyInt_Check(id_obj) && PyString_Check(type_obj)) {
      int id = PyInt_AsLong(id_obj);
      char* type = PyString_AsString(type_obj);
      result = bprb_batch_process_manager::instance()->set_input_from_db(input, id, type);
    }
  }

  return Py_BuildValue("b", result);
}

PyObject *remove_data(PyObject * /*self*/, PyObject *args)
{
  unsigned id;
  if (!PyArg_ParseTuple(args, "i:set_input_from_db", &id))
    return NULL;
  bool result = bprb_batch_process_manager::instance()->remove_data(id);

  return Py_BuildValue("b", result);
}

PyObject *remove_data_obj(PyObject * /*self*/, PyObject *args)
{
  PyObject* obj;
  if (!PyArg_ParseTuple(args, "O:set_input_from_db", &obj))
    return NULL;

  bool result = false;
  if (PyObject_HasAttrString(obj, "id")) {
    PyObject* id_obj = PyObject_GetAttrString(obj,"id");
    if (PyInt_Check(id_obj)) {
      unsigned id = PyInt_AsLong(id_obj);
      result = bprb_batch_process_manager::instance()->remove_data(id);
    }
  }
  return Py_BuildValue("b", result);
}

PyObject *print_db(PyObject * /*self*/, PyObject * /*args*/)
{
  bprb_batch_process_manager::instance()->print_db();
  Py_INCREF(Py_None);
  return Py_None;
}

PyObject *
clear(PyObject * /*self*/, PyObject * /*args*/)
{
  bprb_batch_process_manager::instance()->clear();
  Py_INCREF(Py_None);
  return Py_None;
}

PyObject *get_bbas_1d_array_byte(PyObject * /*self*/, PyObject *args)
{
  unsigned id;
  bbas_1d_array_byte_sptr value;
  if (!PyArg_ParseTuple(args, "i:get_bbas_1d_array_byte", &id))
    return NULL;

  std::string relation_name = "bbas_1d_array_byte_sptr_data";

  // query to get the data
  brdb_query_aptr Q = brdb_query_comp_new("id", brdb_query::EQ, id);
  brdb_selection_sptr selec = DATABASE->select(relation_name, std::move(Q));
  PyObject *array_1d=0;
  if (selec->size()!=1) {
    std::cout << "in get_bbas_1d_array_byte() - no relation with type" << relation_name << " id: " << id << std::endl;

    return array_1d;
  }

  brdb_value_sptr brdb_value;
  if (!selec->get_value(std::string("value"), brdb_value)) {
    std::cout << "in get_bbas_1d_array_byte() didn't get value\n";
    return array_1d;
  }

  if (!brdb_value) {
    std::cout << "in get_bbas_1d_array_byte() - null value\n";
    return array_1d;
  }

  brdb_value_t<bbas_1d_array_byte_sptr>* result_out = static_cast<brdb_value_t<bbas_1d_array_byte_sptr>* >(brdb_value.ptr());
  value = result_out->value();

  array_1d = PyList_New(value->data_array.size());
  for (unsigned i=0;i<value->data_array.size();i++)
  {
    PyObject *x = Py_BuildValue("b", value->data_array[i]);
    PyList_SetItem(array_1d, i,x);
  }
  return array_1d;
}

PyObject *get_bbas_1d_array_int(PyObject * /*self*/, PyObject *args)
{
  unsigned id;
  bbas_1d_array_int_sptr value;
  if (!PyArg_ParseTuple(args, "i:get_bbas_1d_array_int", &id))
    return NULL;

  std::string relation_name = "bbas_1d_array_int_sptr_data";

  // query to get the data
  brdb_query_aptr Q = brdb_query_comp_new("id", brdb_query::EQ, id);
  brdb_selection_sptr selec = DATABASE->select(relation_name, std::move(Q));
  PyObject *array_1d=0;
  if (selec->size()!=1) {
    std::cout << "in get_bbas_1d_array_int() - no relation with type" << relation_name << " id: " << id << std::endl;

    return array_1d;
  }

  brdb_value_sptr brdb_value;
  if (!selec->get_value(std::string("value"), brdb_value)) {
    std::cout << "in get_bbas_1d_array_int() didn't get value\n";
    return array_1d;
  }

  if (!brdb_value) {
    std::cout << "in get_bbas_1d_array_int() - null value\n";
    return array_1d;
  }

  brdb_value_t<bbas_1d_array_int_sptr>* result_out = static_cast<brdb_value_t<bbas_1d_array_int_sptr>* >(brdb_value.ptr());
  value = result_out->value();

  array_1d = PyList_New(value->data_array.size());
  PyObject *x;
  for (unsigned i=0;i<value->data_array.size();i++)
  {
    x=PyInt_FromLong((long)value->data_array[i]);
    PyList_SetItem(array_1d, i,x);
  }
  return array_1d;
}

PyObject *get_bbas_1d_array_unsigned(PyObject * /*self*/, PyObject *args)
{
  unsigned id;
  bbas_1d_array_unsigned_sptr value;
  if (!PyArg_ParseTuple(args, "i:get_bbas_1d_array_unsigned", &id))
    return NULL;

  std::string relation_name = "bbas_1d_array_unsigned_sptr_data";

  // query to get the data
  brdb_query_aptr Q = brdb_query_comp_new("id", brdb_query::EQ, id);
  brdb_selection_sptr selec = DATABASE->select(relation_name, std::move(Q));
  PyObject *array_1d=0;
  if (selec->size()!=1) {
    std::cout << "in get_bbas_1d_array_unsigned() - no relation with type" << relation_name << " id: " << id << std::endl;

    return array_1d;
  }

  brdb_value_sptr brdb_value;
  if (!selec->get_value(std::string("value"), brdb_value)) {
    std::cout << "in get_bbas_1d_array_unsigned() didn't get value\n";
    return array_1d;
  }

  if (!brdb_value) {
    std::cout << "in get_bbas_1d_array_unsigned() - null value\n";
    return array_1d;
  }

  brdb_value_t<bbas_1d_array_unsigned_sptr>* result_out = static_cast<brdb_value_t<bbas_1d_array_unsigned_sptr>* >(brdb_value.ptr());
  value = result_out->value();

  array_1d = PyList_New(value->data_array.size());
  PyObject *x;
  for (unsigned i=0;i<value->data_array.size();i++)
  {
    x=PyInt_FromLong((long)value->data_array[i]);
    PyList_SetItem(array_1d, i,x);
  }
  return array_1d;
}

PyObject *get_bbas_1d_array_float(PyObject * /*self*/, PyObject *args)
{
  unsigned id;
  bbas_1d_array_float_sptr value;
  if (!PyArg_ParseTuple(args, "i:get_bbas_1d_array_float", &id))
    return NULL;

  std::string relation_name = "bbas_1d_array_float_sptr_data";

  // query to get the data
  brdb_query_aptr Q = brdb_query_comp_new("id", brdb_query::EQ, id);
  brdb_selection_sptr selec = DATABASE->select(relation_name, std::move(Q));
  PyObject *array_1d=0;
  if (selec->size()!=1) {
    std::cout << "in get_bbas_1d_array_float() - no relation with type" << relation_name << " id: " << id << std::endl;

    return array_1d;
  }

  brdb_value_sptr brdb_value;
  if (!selec->get_value(std::string("value"), brdb_value)) {
    std::cout << "in get_bbas_1d_array_float() didn't get value\n";
    return array_1d;
  }

  if (!brdb_value) {
    std::cout << "in get_bbas_1d_array_float() - null value\n";
    return array_1d;
  }

  brdb_value_t<bbas_1d_array_float_sptr>* result_out = static_cast<brdb_value_t<bbas_1d_array_float_sptr>* >(brdb_value.ptr());
  value = result_out->value();

  array_1d = PyList_New(value->data_array.size());
  PyObject *x;
  for (unsigned i=0;i<value->data_array.size();i++)
  {
    x=PyFloat_FromDouble((double)value->data_array[i]);
    PyList_SetItem(array_1d, i,x);
  }
  return array_1d;
}

PyObject *get_output_double_array(PyObject * /*self*/, PyObject *args)
{
  unsigned id;
  bbas_1d_array_double_sptr value;
  if (!PyArg_ParseTuple(args, "i:get_output_double_array", &id))
    return NULL;

  std::string relation_name = "bbas_1d_array_double_sptr_data";

  // query to get the data
  brdb_query_aptr Q = brdb_query_comp_new("id", brdb_query::EQ, id);
  brdb_selection_sptr selec = DATABASE->select(relation_name, std::move(Q));
  PyObject *array_1d=0;
  if (selec->size()!=1) {
    std::cout << "in get_bbas_1d_array_double() - no relation with type" << relation_name << " id: " << id << std::endl;

    return array_1d;
  }

  brdb_value_sptr brdb_value;
  if (!selec->get_value(std::string("value"), brdb_value)) {
    std::cout << "in get_bbas_1d_array_double() didn't get value\n";
    return array_1d;
  }

  if (!brdb_value) {
    std::cout << "in get_bbas_1d_array_double() - null value\n";
    return array_1d;
  }

  brdb_value_t<bbas_1d_array_double_sptr>* result_out = static_cast<brdb_value_t<bbas_1d_array_double_sptr>* >(brdb_value.ptr());
  value = result_out->value();

  array_1d = PyList_New(value->data_array.size());
  PyObject *x;
  for (Py_ssize_t i=0;i<value->data_array.size();i++)
  {
    x=PyFloat_FromDouble((double)value->data_array[i]);
    PyList_SetItem(array_1d, i,x);
  }
  return array_1d;
}

PyObject *get_bbas_1d_array_string(PyObject * /*self*/, PyObject *args)
{
  unsigned id;
  bbas_1d_array_string_sptr value;
  if (!PyArg_ParseTuple(args, "i:get_bbas_1d_array_string", &id))
    return NULL;

  std::string relation_name = "bbas_1d_array_string_sptr_data";

  // query to get the data
  brdb_query_aptr Q = brdb_query_comp_new("id", brdb_query::EQ, id);
  brdb_selection_sptr selec = DATABASE->select(relation_name, std::move(Q));
  PyObject *array_1d=0;
  if (selec->size()!=1) {
    std::cout << "in get_bbas_1d_array_string() - no relation with type" << relation_name << " id: " << id << std::endl;

    return array_1d;
  }

  brdb_value_sptr brdb_value;
  if (!selec->get_value(std::string("value"), brdb_value)) {
    std::cout << "in get_bbas_1d_array_string() didn't get value\n";
    return array_1d;
  }

  if (!brdb_value) {
    std::cout << "in get_bbas_1d_array_string() - null value\n";
    return array_1d;
  }

  brdb_value_t<bbas_1d_array_string_sptr>* result_out = static_cast<brdb_value_t<bbas_1d_array_string_sptr>* >(brdb_value.ptr());
  value = result_out->value();

  array_1d = PyList_New(value->data_array.size());
  for (unsigned i=0;i<value->data_array.size();i++)
  {
    PyObject *x = Py_BuildValue("s", value->data_array[i].c_str());
    PyList_SetItem(array_1d, i,x);
  }
  return array_1d;
}


PyObject *set_stdout(PyObject * /*self*/, PyObject *args)
{
  const char* file;
  if (!PyArg_ParseTuple(args, "s:set_stdout", &file))
    return NULL;
  std::string f(file);
  std::cout << f << '\n';
  bprb_batch_process_manager::instance()->set_stdout(f);
  Py_INCREF(Py_None);
  return Py_None;
}

PyObject *reset_stdout(PyObject *, PyObject *)
{
  bprb_batch_process_manager::instance()->reset_stdout();
  Py_INCREF(Py_None);
  return Py_None;
}

void
register_basic_datatypes()
{
  REGISTER_DATATYPE(bool);
  REGISTER_DATATYPE_LONG_FORM(std::string,vcl_string);
  REGISTER_DATATYPE(int);
  REGISTER_DATATYPE(unsigned);
  REGISTER_DATATYPE(long);
  REGISTER_DATATYPE(float);
  REGISTER_DATATYPE(double);
  REGISTER_DATATYPE(bbas_1d_array_float_sptr);
  REGISTER_DATATYPE(bbas_1d_array_double_sptr);
  REGISTER_DATATYPE(bbas_1d_array_string_sptr);
  REGISTER_DATATYPE(bbas_1d_array_unsigned_sptr);
  REGISTER_DATATYPE(bbas_1d_array_int_sptr);
  REGISTER_DATATYPE(bbas_1d_array_byte_sptr);
}
