#ifndef BPRO_DEFS_H
#define BPRO_DEFS_H

#define py_funcs(name,process,arg,doc) {name,process,arg,doc}

py_funcs("init_process",init_process,METH_VARARGS,"init_process(s) create a new process instance by name"),
py_funcs("process_print_default_params",process_print_default_params,METH_VARARGS,"process_print_default_params(s,s) print the default values of the process by name"),
py_funcs("set_params_process",set_params_process,METH_VARARGS,"set_params_process(s) set the parameter values of the current process from the XML file"),
py_funcs("set_input_bool",set_input_bool,METH_VARARGS,"set_input_(i,b) set input i on current process to a bool value"),
py_funcs("set_input_string",set_input_string,METH_VARARGS,"set_input_(i,s) set input i on current process to a string value"),
py_funcs("set_input_string_array",set_input_string_array,METH_VARARGS,"set_input_(i,s) set input i on current process to a string value"),
py_funcs("set_input_int",set_input_int,METH_VARARGS,"set_input_(i,i) set input i on current process to an int value"),
py_funcs("set_input_int_array",set_input_int_array,METH_VARARGS,"set_input_(i,f)_array set input i on current process to a int value"),
py_funcs("set_input_unsigned",set_input_unsigned,METH_VARARGS,"set_input_(i,i) set input i on current process to an unsigned value"),
py_funcs("set_input_unsigned_array",set_input_unsigned_array,METH_VARARGS,"set_input_(i,f)_array set input i on current process to a unsigned value"),
py_funcs("set_input_long",set_input_long,METH_VARARGS,"set_input_(i,l) set input i on current process to a long value"),
py_funcs("set_input_float",set_input_float,METH_VARARGS,"set_input_(i,f) set input i on current process to a float value"),
py_funcs("set_input_float_array",set_input_float_array,METH_VARARGS,"set_input_(i,f)_array set input i on current process to a float value"),
py_funcs("set_input_double",set_input_double,METH_VARARGS,"set_input_(i,d) set input i on current process to a double value"),
py_funcs("set_input_double_array",set_input_double_array,METH_VARARGS,"set_input_(i,d)_array set input i on current process to a double value"),
py_funcs("get_output_string", get_output_string, METH_VARARGS, "get_output_string(i) return value of output i in the database"),
py_funcs("get_output_float",get_output_float,METH_VARARGS,"get_output_float(i) return value of output i in the database"),
py_funcs("get_output_double",get_output_double,METH_VARARGS,"get_output_double(i) return value of output i in the database"),
py_funcs("get_output_int",get_output_int,METH_VARARGS,"get_output_int(i) return value of output i in the database"),
py_funcs("get_output_unsigned",get_output_unsigned,METH_VARARGS,"get_output_unsigned(i) return value of output i in the database"),
py_funcs("process_init",process_init,METH_VARARGS,"process_init() initialize the current process state before execution"),
py_funcs("run_process",run_process,METH_VARARGS,"run_process() run the current process"),
py_funcs("finish_process",finish_process,METH_VARARGS,"finish_process() finish the current process"),
py_funcs("verbose",verbose,METH_VARARGS,"verbose() print process info"),
py_funcs("not_verbose",not_verbose,METH_VARARGS,"not_verbose() stop process info"),
py_funcs("commit_output",commit_output,METH_VARARGS,"commit_output(i) put output i in the database"),
py_funcs("set_input_from_db",set_input_from_db,METH_VARARGS,"set_input_from_db(i, i) set input i of the current process to db id value"),
py_funcs("remove_data",remove_data,METH_VARARGS,"remove_data(i) remove data with id from db"),
py_funcs("remove_data_obj",remove_data_obj,METH_VARARGS,"remove_data_obj(i) remove data with obj.id from db"),
py_funcs("print_db",print_db,METH_VARARGS,"print_db() print the database"),
py_funcs("clear",clear,METH_VARARGS,"clear() clear the database tables"),
py_funcs("get_bbas_1d_array_float",get_bbas_1d_array_float,METH_VARARGS,"get_bbas_1d_array_float(i) set the database 1d array"),
py_funcs("get_output_double_array",get_output_double_array,METH_VARARGS,"get_output_double_array(i) get the database 1d array"),
py_funcs("get_bbas_1d_array_byte",get_bbas_1d_array_byte,METH_VARARGS,"get_bbas_1d_array_byte(i) set the database 1d array"),
py_funcs("get_bbas_1d_array_int",get_bbas_1d_array_int,METH_VARARGS,"get_bbas_1d_array_int(i) set the database 1d array"),
py_funcs("get_bbas_1d_array_unsigned",get_bbas_1d_array_unsigned,METH_VARARGS,"get_bbas_1d_array_unsigned(i) set the database 1d array"),
py_funcs("get_bbas_1d_array_string",get_bbas_1d_array_string,METH_VARARGS,"get_bbas_1d_array_string(i) set the database 1d array"),
py_funcs("set_stdout",set_stdout,METH_VARARGS,"set_stdout redirects stdout to a file"),
py_funcs("reset_stdout",reset_stdout,METH_VARARGS,"reset_stdout resets stdout to the console"),

#undef py_funcs

#endif
