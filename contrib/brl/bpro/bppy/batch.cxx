#include "Python.h"
#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <brdb/brdb_value.h>
static PyObject *
register_processes(PyObject *self, PyObject *args)
{
  REG_PROCESS(bprb_null_process, bprb_batch_process_manager);
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *
register_datatypes(PyObject *self, PyObject *args)
{
  REGISTER_DATATYPE(bool);
  REGISTER_DATATYPE(vcl_string);
  REGISTER_DATATYPE(int);
  REGISTER_DATATYPE(long);
  REGISTER_DATATYPE(float);
  REGISTER_DATATYPE(double);
	Py_INCREF(Py_None);
	return Py_None;
}


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
	{"register_processes", register_processes, METH_VARARGS, "register_processes() doc string"},
	{"register_datatypes", register_datatypes, METH_VARARGS, "register_datatypes() doc string"},
	{"init_process", init_process, METH_VARARGS, "init_process(s) doc string"},
	{"set_input_bool", set_input_bool, METH_VARARGS, "set_input_(i,b) doc string"},
	{"set_input_string", set_input_string, METH_VARARGS, "set_input_(i,s) doc string"},
	{"set_input_int", set_input_int, METH_VARARGS, "set_input_(i,i) doc string"},
	{"set_input_long", set_input_long, METH_VARARGS, "set_input_(i,l) doc string"},
	{"set_input_float", set_input_float, METH_VARARGS, "set_input_(i,f) doc string"},
	{"set_input_double", set_input_double, METH_VARARGS, "set_input_(i,d) doc string"},
  {"run_process", run_process, METH_VARARGS, "run_process() doc string"},
  {"commit_output", commit_output, METH_VARARGS, "commit_output() doc string"},
  {"set_input_from_db", set_input_from_db, METH_VARARGS, "set_input_from_db(i, i) doc string"},
  {"print_db", print_db, METH_VARARGS, "print_db() doc string"},
  {"clear", clear, METH_VARARGS, "clear() doc string"},
	{NULL, NULL}
};



PyMODINIT_FUNC
initbatch(void)
{
	Py_InitModule("batch", batch_methods);
}
