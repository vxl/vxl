#include <boxm2/boxm2_cpp_processor.h>

bool boxm2_cpp_processor::init()
{
return true;
}

bool boxm2_cpp_processor::run(boxm2_process * process, vcl_vector<brdb_value_sptr> & input, vcl_vector<brdb_value_sptr> & output)
{
  process->execute(input, output); 
  return true;
}

bool boxm2_cpp_processor::finish()
{
return true;
}
