#include <testlib/testlib_test.h>
#include <brdb/brdb_relation.h>
#include <brdb/brdb_query.h>
#include <brdb/brdb_tuple.h>
#include <brdb/brdb_selection.h>
#include <brdb/brdb_database_manager.h>
#include <brdb/brdb_value.h>
#include <vcl_iostream.h>
#include "bprb_test_process.h"
#include <bprb/bprb_parameters.h>
MAIN( test_process )
{
  //Set up a simple process database that holds some sample inputs, and outputs,
  //this step would be done by the process manager from a script or from menu inputs
    //define the process input relation
  vcl_vector<vcl_string> r_names(2);
  vcl_vector<vcl_string> r_types(2);
  vcl_vector<brdb_tuple_sptr> r_tuples(2);
  r_names[0]="input";
  r_names[1]="value";
  r_types[0]=brdb_value_t<vcl_string>::type();
  r_types[1]=brdb_value_t<float>::type();
  brdb_tuple_sptr t =new brdb_tuple(vcl_string("input0"), 1.0f); 
  r_tuples[0]=t;
  r_tuples[1]=new brdb_tuple(vcl_string("input1"), 2.0f);

  brdb_relation_sptr r_in = new brdb_relation(r_names, r_tuples, r_types);
  //add the relation to the database
  DATABASE->add_relation(vcl_string("process_input_relation"), r_in);

  //define the output relation that stores process results
  r_names[0]="output";
  brdb_relation_sptr r_out = new brdb_relation(r_names, r_types);
  DATABASE->add_relation(vcl_string("process_output_relation"), r_out);

  //create the process - the database is global
  bprb_process_sptr proc = new bprb_test_process();
  
  proc->parameters()->set_value("-prm1", 5.0f);
  //execute the process (command pattern) just adds the two inputs and the parameter
  proc->execute();

  //test that output is correct
  brdb_query_aptr Q = brdb_query_comp_new("output", brdb_query::EQ,
                                          vcl_string("output0"));
  bool good = true;
  brdb_selection_sptr selec = DATABASE->select("process_output_relation", Q);
  if(selec->size()!=1)
    good = false;
  brdb_value_t<float> arg;

  if(!selec->get_value(vcl_string("value"), arg))
    good = false;
  TEST("Process using db", arg.value(), 8.0f);
      
  SUMMARY();
}
