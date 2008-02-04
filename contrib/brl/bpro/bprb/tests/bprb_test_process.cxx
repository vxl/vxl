#include "bprb_test_process.h"
#include <brdb/brdb_relation.h>
#include <brdb/brdb_query.h>
#include <brdb/brdb_tuple.h>
#include <brdb/brdb_selection.h>
#include <brdb/brdb_database_manager.h>
#include <bprb/bprb_parameters.h>
#include <brdb/brdb_value.h>

bprb_test_process::bprb_test_process()
{
  if( !parameters()->add( "Param 1" ,      "-prm1" ,  4.0f ) ||
      !parameters()->add( "Add/Subtract" , "-add_sub" , true ) )
    {
     vcl_cerr << " Error in adding parameters \n";
    }
  input_data_.resize(2, brdb_value_sptr(0));
  output_data_.resize(1,brdb_value_sptr(0));
  input_types_.resize(2);
  output_types_.resize(1);
  input_types_[0]="float";
  input_types_[1]="float";
  output_types_[0]="float";
}

bool bprb_test_process::execute()
{
  // the process is simple, just add the two values
  brdb_value_t<float>* i0 = 
    static_cast<brdb_value_t<float>* >(input_data_[0].ptr());

  brdb_value_t<float>* i1 = 
    static_cast<brdb_value_t<float>* >(input_data_[1].ptr());

  float sum = i0->value() + i1->value();
  
  // add in one of the parameters
  float argp;
  if(!parameters()->get_value("-prm1", argp))
    return false;
  sum += argp;
  
  // set the output
  brdb_value_sptr v = new brdb_value_t<float>(sum);
  output_data_[0]=v;
  return true;
}
