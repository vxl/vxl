#include "bpro_batch.h"
#include <vcl_string.h>
#include <vcl_iostream.h>

#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_macros.h>

#include <brdb/brdb_value.h>
#include <brdb/brdb_query.h>
#include <brdb/brdb_selection.h>
#include <brdb/brdb_database_manager.h>

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
static PyObject *set_input_from_db(PyObject* self, PyObject *args);
static PyObject *remove_data(PyObject *self, PyObject *args);
static PyObject *remove_data_obj(PyObject *self, PyObject *args);
static PyObject *print_db(PyObject *self, PyObject *args);
static PyObject *clear(PyObject *self, PyObject *args);
vcl_string initp = "init_process";
vcl_string initp_com = "init_process(s) create a new process instance by name";
vcl_string proc_print_def = "process_print_default_params";
vcl_string proc_print_def_com = "process_print_default_params(s,s) print the default values of the process by name";
vcl_string set_prm_proc = "set_params_process";
vcl_string set_prm_proc_com = "set_params_process(s) set the parameter values of the current process from the XML file";
vcl_string set_in_bool = "set_input_bool";
vcl_string set_in_bool_com = "set_input_(i,b) set input i on current process to a bool value";
vcl_string set_in_string = "set_input_string";
vcl_string set_in_string_com = "set_input_(i,s) set input i on current process to a string value";
vcl_string set_in_int ="set_input_int";
vcl_string set_in_int_com ="set_input_(i,i) set input i on current process to an int value";
vcl_string set_in_unsign ="set_input_unsigned";
vcl_string set_in_unsign_com ="set_input_(i,i) set input i on current process to an unsigned value";
vcl_string set_in_long ="set_input_long";
vcl_string set_in_long_com ="set_input_(i,l) set input i on current process to a long value";
vcl_string set_in_float = "set_input_float";
vcl_string set_in_float_com ="set_input_(i,f) set input i on current process to a float value";
vcl_string set_in_double = "set_input_double";
vcl_string set_in_double_com ="set_input_(i,d) set input i on current process to a double value";
vcl_string get_in_float = "get_input_float";
vcl_string get_in_float_com ="get_input_(i) return value of output i in the database";
vcl_string get_in_unsigned ="get_input_unsigned";
vcl_string get_in_unsigned_com ="get_input_(i) return value of output i in the database";
vcl_string proc_init ="process_init";
vcl_string proc_init_com ="process_init() initialize the current process state before execution";
vcl_string run_proc ="run_process";
vcl_string run_proc_com ="run_process() run the current process";
vcl_string comt_out ="commit_output";
vcl_string comt_out_com ="commit_output(i) put output i in the database ";
vcl_string set_idb ="set_input_from_db";
vcl_string set_idb_com ="set_input_from_db(i, i) set input i of the current process to db id value";
vcl_string rm_dat ="remove_data";
vcl_string rm_dat_com ="remove_data(i) remove data with id from db";
vcl_string rm_dato ="remove_data_obj";
vcl_string rm_dato_com ="remove_data_obj(i) remove data with obj.id from db";
vcl_string pdb ="print_db";
vcl_string pdb_com ="print_db() print the database";
vcl_string clr ="clear";
vcl_string clr_com ="clear() clear the database tables";

PyMethodDef batch_methods[] =
  {
#if 0
    {"register_processes", register_processes, METH_VARARGS,
     "register_processes() create instances of each defined process"},
    {"register_datatypes", register_datatypes, METH_VARARGS,
     "register_datatypes() insert tables in the database for each type"},
#endif // 0
    {initp.c_str(), init_process, METH_VARARGS,  initp_com.c_str()},
    {proc_print_def.c_str(), process_print_default_params, METH_VARARGS, proc_print_def_com.c_str()},
    {set_prm_proc.c_str(), set_params_process, METH_VARARGS, set_prm_proc_com.c_str()},
    {set_in_bool.c_str(), set_input_bool, METH_VARARGS, set_in_bool_com.c_str()},
    {set_in_string.c_str(), set_input_string, METH_VARARGS, set_in_string_com.c_str()},
    {set_in_int.c_str(), set_input_int, METH_VARARGS,set_in_int_com.c_str()},
    {set_in_unsign.c_str(), set_input_unsigned, METH_VARARGS,set_in_unsign_com.c_str()},
    {set_in_long.c_str(), set_input_long, METH_VARARGS,set_in_long_com.c_str()},
    {set_in_float.c_str(), set_input_float, METH_VARARGS,set_in_float_com.c_str()},
    {set_in_double.c_str(), set_input_double, METH_VARARGS,set_in_double_com.c_str()},
    {get_in_float.c_str(), get_input_float, METH_VARARGS,get_in_float_com.c_str()},
    {get_in_unsigned.c_str(), get_input_unsigned, METH_VARARGS,get_in_unsigned_com.c_str()},
    {proc_init.c_str(), process_init, METH_VARARGS,proc_init_com.c_str()},
    {run_proc.c_str(), run_process, METH_VARARGS,run_proc_com.c_str()},
    {comt_out.c_str(), commit_output, METH_VARARGS,comt_out_com.c_str()},
    {set_idb.c_str(), set_input_from_db, METH_VARARGS,set_idb_com.c_str()},
    {rm_dat.c_str(), remove_data, METH_VARARGS,rm_dat_com.c_str()},
    {rm_dato.c_str(), remove_data_obj, METH_VARARGS, rm_dato_com.c_str()},
    {pdb.c_str(), print_db, METH_VARARGS,pdb_com.c_str() },
    {clr.c_str(), clear, METH_VARARGS, clr_com.c_str()},
    {NULL, NULL},
    {NULL, NULL}
  };

PyObject *init_process(PyObject *self, PyObject *args)
{
  const char* name;
  if (!PyArg_ParseTuple(args, "s:init_process", &name))
    return NULL;
  vcl_string n(name);
  vcl_cout << n << '\n';
  bool result = bprb_batch_process_manager::instance()->init_process(n);
  return Py_BuildValue("b", result);
}

PyObject *set_input_bool(PyObject *self, PyObject *args)
{
  int input;
  bool value;
  if (!PyArg_ParseTuple(args, "ib:set_input_bool", &input, &value))
    return NULL;
  brdb_value_sptr v = new brdb_value_t<bool>(value);
  vcl_cout << "input[" << input << "](bool): " << value << '\n';
  bool result = bprb_batch_process_manager::instance()->set_input(input, v);
  return Py_BuildValue("b", result);
}

PyObject *
set_input_string(PyObject *self, PyObject *args)
{
  int input;
  const char* value;
  if (!PyArg_ParseTuple(args, "is:set_input_string", &input, &value))
    return NULL;
  brdb_value_sptr v = new brdb_value_t<vcl_string>(value);
  vcl_cout << "input[" << input << "](string): " << value << '\n';
  bool result = bprb_batch_process_manager::instance()->set_input(input, v);
  return Py_BuildValue("b", result);
}

PyObject *set_input_unsigned(PyObject *self, PyObject *args)
{
  int input;
  unsigned ivalue;
  if (!PyArg_ParseTuple(args, "ii:set_input_int", &input, &ivalue))
    return NULL;
  brdb_value_sptr iv = new brdb_value_t<unsigned>(ivalue);
  vcl_cout << "input[" << input << "](unsigned): " << ivalue << '\n';
  bool result = bprb_batch_process_manager::instance()->set_input(input, iv);
  return Py_BuildValue("b", result);
}

PyObject *set_input_int(PyObject *self, PyObject *args)
{
  int input;
  int ivalue;
  if (!PyArg_ParseTuple(args, "ii:set_input_int", &input, &ivalue))
    return NULL;
  brdb_value_sptr iv = new brdb_value_t<int>(ivalue);
  vcl_cout << "input[" << input << "](int): " << ivalue << '\n';
  bool result = bprb_batch_process_manager::instance()->set_input(input, iv);
  return Py_BuildValue("b", result);
}

PyObject *set_input_long(PyObject *self, PyObject *args)
{
  int input;
  long value;
  if (!PyArg_ParseTuple(args, "il:set_input_long", &input, &value))
    return NULL;
  brdb_value_sptr v = new brdb_value_t<long>(value);
  vcl_cout << "input[" << input << "](long): " << value << '\n';
  bool result = bprb_batch_process_manager::instance()->set_input(input, v);
  return Py_BuildValue("b", result);
}

PyObject *set_input_float(PyObject *self, PyObject *args)
{
  int input;
  float value;
  if (!PyArg_ParseTuple(args, "if:set_input_float", &input, &value))
    return NULL;
  brdb_value_sptr v = new brdb_value_t<float>(value);
  vcl_cout << "input[" << input << "](float): " << value << '\n';
  bool result = bprb_batch_process_manager::instance()->set_input(input, v);
  return Py_BuildValue("b", result);
}

PyObject *set_input_double(PyObject *self, PyObject *args)
{
  int input;
  double value;
  if (!PyArg_ParseTuple(args, "id:set_input_double", &input, &value))
    return NULL;
  brdb_value_sptr v = new brdb_value_t<double>(value);
  vcl_cout << "input[" << input << "](double): " << value << '\n';
  bool result = bprb_batch_process_manager::instance()->set_input(input, v);
  return Py_BuildValue("b", result);
}

// ozge added the following to access the process outputs while running experiments using Python
PyObject *get_input_float(PyObject *self, PyObject *args)
{
  unsigned id;
  float value;
  if (!PyArg_ParseTuple(args, "i:get_input_float", &id))
    return NULL;

  vcl_string relation_name = "float_data";

  // query to get the data
  brdb_query_aptr Q = brdb_query_comp_new("id", brdb_query::EQ, id);
  brdb_selection_sptr selec = DATABASE->select(relation_name, Q);

  if (selec->size()!=1) {
    vcl_cout << "in get_input_float() - no relation with type" << relation_name << " id: " << id << vcl_endl;
    return Py_BuildValue("f",-1.0);
  }

  brdb_value_sptr brdb_value;
  if (!selec->get_value(vcl_string("value"), brdb_value)) {
    vcl_cout << "in get_input_float() didn't get value\n";
    return Py_BuildValue("f",-1.0);
  }

  if (!brdb_value) {
    vcl_cout << "in get_input_float() - null value\n";
    return Py_BuildValue("f",-1.0);
  }
  brdb_value_t<float>* result_out = static_cast<brdb_value_t<float>* >(brdb_value.ptr());
  value = result_out->value();

  return Py_BuildValue("f", value);
}

// ozge added the following to access the process outputs while running experiments using Python
PyObject *get_input_unsigned(PyObject *self, PyObject *args)
{
  unsigned id;
  unsigned value;
  if (!PyArg_ParseTuple(args, "i:get_input_unsigned", &id))
    return NULL;

  vcl_string relation_name = "unsigned_data";

  // query to get the data
  brdb_query_aptr Q = brdb_query_comp_new("id", brdb_query::EQ, id);
  brdb_selection_sptr selec = DATABASE->select(relation_name, Q);

  if (selec->size()!=1) {
    vcl_cout << "in get_input_unsigned() - no relation with type" << relation_name << " id: " << id << vcl_endl;
    return Py_BuildValue("b",1000);
  }

  brdb_value_sptr brdb_value;
  if (!selec->get_value(vcl_string("value"), brdb_value)) {
    vcl_cout << "in get_input_unsigned() didn't get value\n";
    return Py_BuildValue("b",1000);
  }

  if (!brdb_value) {
    vcl_cout << "in get_input_unsigned() - null value\n";
    return Py_BuildValue("b",1000);
  }
  brdb_value_t<unsigned>* result_out = static_cast<brdb_value_t<unsigned>* >(brdb_value.ptr());
  value = result_out->value();

  return Py_BuildValue("b", value);
}

PyObject *process_print_default_params(PyObject *self, PyObject *args)
{
  const char* name;
  const char* value;
  if (!PyArg_ParseTuple(args, "ss:process_print_default_params", &name, &value))
    return NULL;

  vcl_string n(name);
  vcl_string f(value);

  bool result = bprb_batch_process_manager::instance()->print_default_params(n, f);
  return Py_BuildValue("b", result);
}

PyObject *process_init(PyObject *self, PyObject *args)
{
  bool result = bprb_batch_process_manager::instance()->process_init();
  return Py_BuildValue("b", result);
}

PyObject *set_params_process(PyObject *self, PyObject *args)
{
  const char* value;
  if (!PyArg_ParseTuple(args, "s:set_params_process", &value))
    return NULL;

  vcl_string f(value);

  bool result = bprb_batch_process_manager::instance()->set_params(f);
  return Py_BuildValue("b", result);
}

PyObject *run_process(PyObject *self, PyObject *args)
{
  bool result = bprb_batch_process_manager::instance()->run_process();
  return Py_BuildValue("b", result);
}

PyObject *commit_output(PyObject *self, PyObject *args)
{
  unsigned id;
  unsigned output;
  if (!PyArg_ParseTuple(args, "i:commit_output", &output))
    return NULL;
  vcl_string type;
  bool result = bprb_batch_process_manager::instance()->commit_output(output,id, type);
  if (!result)
    return Py_BuildValue("i", -1);
  else
    return Py_BuildValue("is", id, type.c_str());
}

PyObject *set_input_from_db(PyObject *self, PyObject *args)
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

PyObject *remove_data(PyObject *self, PyObject *args)
{
  unsigned id;
  if (!PyArg_ParseTuple(args, "i:set_input_from_db", &id))
    return NULL;
  bool result = bprb_batch_process_manager::instance()->remove_data(id);

  return Py_BuildValue("b", result);
}

PyObject *remove_data_obj(PyObject *self, PyObject *args)
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

PyObject *print_db(PyObject *self, PyObject *args)
{
  bprb_batch_process_manager::instance()->print_db();
  Py_INCREF(Py_None);
  return Py_None;
}

PyObject *
clear(PyObject *self, PyObject *args)
{
  bprb_batch_process_manager::instance()->clear();
  Py_INCREF(Py_None);
  return Py_None;
}

void
register_basic_datatypes()
{
  REGISTER_DATATYPE(bool);
  REGISTER_DATATYPE(vcl_string);
  REGISTER_DATATYPE(int);
  REGISTER_DATATYPE(unsigned);
  REGISTER_DATATYPE(long);
  REGISTER_DATATYPE(float);
  REGISTER_DATATYPE(double);
}

