#include "bprb_test_process.h"
#include <brdb/brdb_relation.h>
#include <brdb/brdb_query.h>
#include <brdb/brdb_tuple.h>
#include <brdb/brdb_selection.h>
#include <brdb/brdb_database_manager.h>
#include <bprb/bprb_parameters.h>

bprb_test_process::bprb_test_process()
{
  if( !parameters()->add( "Param 1" ,      "-prm1" ,  4.0f ) ||
      !parameters()->add( "Add/Subtract" , "-add_sub" , true ) )
    {
     vcl_cerr << " Error in adding parameters \n";
    }
}

bool bprb_test_process::execute()
{
  // the database is assumed to contain of the following relations
  //
  // "process_input_relation"
  //  ----- tuples -----
  //   input         value
  //  "input0"      float
  //  "input1"      float
  // 
  // "process_output_relation"
  //  ----- tuples -----
  //   output      value
  //  "output0"    float
  //
  // query to get the first input - these steps could be rolled into a method
  brdb_query_aptr Q0 = brdb_query_comp_new("input", brdb_query::EQ,
                                           vcl_string("input0"));

  brdb_selection_sptr selec0 = DATABASE->select("process_input_relation", Q0);
  if(selec0->size()!=1)
    return false;
  brdb_value_t<float> arg0, arg1;

  if(!selec0->get_value(vcl_string("value"), arg0))
    return false;

  // query to get the second input
  brdb_query_aptr Q1 = brdb_query_comp_new("input", brdb_query::EQ, 
                                           vcl_string("input1"));
  brdb_selection_sptr selec1 = DATABASE->select("process_input_relation", Q1);
  if(selec1->size()!=1)
    return false;
  if(!selec1->get_value(vcl_string("value"), arg1))
    return false;

  // the process is simple, just add the two values
  float sum = arg0.value() + arg1.value();

  // add in one of the parameters
  float argp;
  if(!parameters()->get_value("-prm1", argp))
    return false;
  
  //store the result in the output relation

  brdb_tuple_sptr result_tuple = new brdb_tuple(vcl_string("output0"), sum+argp);

  brdb_relation_sptr r = DATABASE->get_relation("process_output_relation");
  r->add_tuple(result_tuple);

  return true;
}
