#include "batch_bvxm.h"
#include <vcl_string.h>
#include <bprb/bprb_batch_process_manager.h>
#include <brdb/brdb_value.h>
#include <vcl_iostream.h>
static PyObject *
init_process(PyObject *self, PyObject *args)
{
  const char* name;
  if (!PyArg_ParseTuple(args, "s:init_process", &name))
    return NULL;
  vcl_string n(name);
  vcl_cout << n << '\n';
  bool result = 
    bprb_batch_process_manager::instance()->init_process(n);
  return Py_BuildValue("b", result);
}

static PyObject *
set_input_bool(PyObject *self, PyObject *args)
{
  int input;
  bool value;
  if (!PyArg_ParseTuple(args, "ib:set_input_bool", &input, &value))
    return NULL;
  brdb_value_sptr v = new brdb_value_t<bool>(value);
  vcl_cout << "input[" << input << "](bool): " << value << '\n';
  bool result = 
    bprb_batch_process_manager::instance()->set_input(input, v);
  return Py_BuildValue("b", result);
}

static PyObject *
set_input_string(PyObject *self, PyObject *args)
{
  int input;
  const char* value;
  if (!PyArg_ParseTuple(args, "is:set_input_string", &input, &value))
    return NULL;
  brdb_value_sptr v = new brdb_value_t<vcl_string>(value);
  vcl_cout << "input[" << input << "](string): " << value << '\n';
  bool result = 
    bprb_batch_process_manager::instance()->set_input(input, v);
  return Py_BuildValue("b", result);
}

static PyObject *
set_input_int(PyObject *self, PyObject *args)
{
  int input;
  int ivalue;
  if (!PyArg_ParseTuple(args, "ii:set_input_int", &input, &ivalue))
    return NULL;
  brdb_value_sptr iv = new brdb_value_t<int>(ivalue);
  vcl_cout << "input[" << input << "](int): " << ivalue << '\n';
  bool result = 
    bprb_batch_process_manager::instance()->set_input(input, iv);
  return Py_BuildValue("b", result);
}

static PyObject *
set_input_long(PyObject *self, PyObject *args)
{
  int input;
  long value;
  if (!PyArg_ParseTuple(args, "il:set_input_long", &input, &value))
    return NULL;
  brdb_value_sptr v = new brdb_value_t<long>(value);
  vcl_cout << "input[" << input << "](long): " << value << '\n';
  bool result = 
    bprb_batch_process_manager::instance()->set_input(input, v);
  return Py_BuildValue("b", result);
}

static PyObject *
set_input_float(PyObject *self, PyObject *args)
{
  int input;
  float value;
  if (!PyArg_ParseTuple(args, "if:set_input_float", &input, &value))
    return NULL;
  brdb_value_sptr v = new brdb_value_t<float>(value);
  vcl_cout << "input[" << input << "](float): " << value << '\n';
  bool result = 
    bprb_batch_process_manager::instance()->set_input(input, v);
  return Py_BuildValue("b", result);
}

static PyObject *
set_input_double(PyObject *self, PyObject *args)
{
  int input;
  double value;
  if (!PyArg_ParseTuple(args, "id:set_input_double", &input, &value))
    return NULL;
  brdb_value_sptr v = new brdb_value_t<double>(value);
  vcl_cout << "input[" << input << "](double): " << value << '\n';
  bool result = 
    bprb_batch_process_manager::instance()->set_input(input, v);
  return Py_BuildValue("b", result);
}

static PyObject *
process_print_default_params(PyObject *self, PyObject *args)
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

static PyObject *
process_init(PyObject *self, PyObject *args)
{
  bool result = bprb_batch_process_manager::instance()->process_init();
  return Py_BuildValue("b", result);
}

static PyObject *
set_params_process(PyObject *self, PyObject *args)
{
  const char* value;
  if (!PyArg_ParseTuple(args, "s:set_params_process", &value))
    return NULL;

  vcl_string f(value);
  
  bool result = bprb_batch_process_manager::instance()->set_params(f);
  return Py_BuildValue("b", result);
}

static PyObject *
run_process(PyObject *self, PyObject *args)
{
  bool result = bprb_batch_process_manager::instance()->run_process();
  return Py_BuildValue("b", result);
}

static PyObject *
commit_output(PyObject *self, PyObject *args)
{
  unsigned id;
  unsigned output;
  if (!PyArg_ParseTuple(args, "i:commit_output", &output))
    return NULL;
  bool result = 
    bprb_batch_process_manager::instance()->commit_output(output,id);
  if(!result)
    return Py_BuildValue("i", -1);
  return Py_BuildValue("i", id);
}

static PyObject *
set_input_from_db(PyObject *self, PyObject *args)
{
  unsigned input;
  unsigned id;
  if (!PyArg_ParseTuple(args, "ii:set_input_from_db", &input, &id))
    return NULL;
  bool result = 
    bprb_batch_process_manager::instance()->set_input_from_db(input,id);

  return Py_BuildValue("b", result);
}

static PyObject *
remove_data(PyObject *self, PyObject *args)
{
  unsigned id;
  if (!PyArg_ParseTuple(args, "i:set_input_from_db", &id))
    return NULL;
  bool result = 
    bprb_batch_process_manager::instance()->remove_data(id);

  return Py_BuildValue("b", result);
}

static PyObject *
print_db(PyObject *self, PyObject *args){
  bprb_batch_process_manager::instance()->print_db();
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
clear(PyObject *self, PyObject *args){
  bprb_batch_process_manager::instance()->clear();
  Py_INCREF(Py_None);
  return Py_None;
}

static PyMethodDef batch_methods[] = {
  {"register_processes", register_processes, METH_VARARGS,
  "register_processes() create instances of each defined process"},
  {"register_datatypes", register_datatypes, METH_VARARGS,
  "register_datatypes() insert tables in the database for each type"},
  {"init_process", init_process, METH_VARARGS,
  "init_process(s) create a new process instance by name"},
  {"process_print_default_params", process_print_default_params, METH_VARARGS,
  "process_print_default_params(s,s) print the default values of the process by name"},
  {"set_params_process", set_params_process, METH_VARARGS, 
  "set_params_process(s) set the parameter values of the current process from the XML file"},
  {"set_input_bool", set_input_bool, METH_VARARGS,
  "set_input_(i,b) set input i on current process to a bool value"},
  {"set_input_string", set_input_string, METH_VARARGS,
  "set_input_(i,s) set input i on current process to a string value"},
  {"set_input_int", set_input_int, METH_VARARGS,
  "set_input_(i,i) set input i on current process to an int value"},
  {"set_input_long", set_input_long, METH_VARARGS,
  "set_input_(i,l) set input i on current process to a long value"},
  {"set_input_float", set_input_float, METH_VARARGS,
  "set_input_(i,f) set input i on current process to a float value"},
  {"set_input_double", set_input_double, METH_VARARGS,
  "set_input_(i,d) set input i on current process to a double value"},
  {"process_init", process_init, METH_VARARGS,
  "process_init() initialize the current process state before execution"},
  {"run_process", run_process, METH_VARARGS,
  "run_process() run the current process"},
  {"commit_output", commit_output, METH_VARARGS,
  "commit_output(i) put output i in the database "},
  {"set_input_from_db", set_input_from_db, METH_VARARGS,
  "set_input_from_db(i, i) set input i of the current process to db id value"},
  {"remove_data", remove_data, METH_VARARGS,
  "remove_data(i) remove data with id from db"},
  {"print_db", print_db, METH_VARARGS, "print_db() print the database"},
  {"clear", clear, METH_VARARGS, "clear() clear the database tables"},
  {NULL, NULL},
  {NULL, NULL}
};



PyMODINIT_FUNC
initbvxm_batch(void)
{
  Py_InitModule("bvxm_batch", batch_methods);
}
