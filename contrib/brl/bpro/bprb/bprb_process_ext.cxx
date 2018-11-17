// This is brl/bpro/bprb/bprb_process_ext.cxx
#include <iostream>
#include "bprb_process_ext.h"
//:
// \file

#include <bprb/bprb_parameters.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

bprb_process_ext::bprb_process_ext()
: bprb_process()
{
}

bool bprb_process_ext::set_output_types(std::vector<std::string> const& types)
{
  output_types_ = types;
  output_data_.resize(types.size());
  return true;
}

bool bprb_process_ext::set_output(unsigned i, const brdb_value_sptr& val)
{
  if (i < output_data_.size())
    output_data_[i] = val;
  return i < output_data_.size();
}

bool bprb_process_ext::set_input_types(std::vector<std::string> const& types)
{
  input_types_ = types;
  input_data_.resize(types.size());
  return true;
}

//: set a particular input, resize array if necessary
bool bprb_process_ext::set_input(unsigned i, brdb_value_sptr const& value)
{
  if (!value) {
    std::cout << "bprb_process_ext::set_input() - null value\n";
    return false;
  }

  if (input_data_.size()<=i)
    input_data_.resize(i+1);
  input_data_[i]=value;
  return true;
}

//: Set all the inputs at once
bool bprb_process_ext::set_input_data(std::vector<brdb_value_sptr> const& inputs)
{
  for (unsigned i = 0; i<inputs.size(); ++i)
    if (!set_input(i, inputs[i]))
      return false;
  return true;
}
