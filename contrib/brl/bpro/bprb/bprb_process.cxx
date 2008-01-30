// This is brl/bpro/bprb/bprb_process.cxx

//:
// \file

#include <bprb/bprb_process.h>

#include <vcl_iostream.h>
#include <vcl_cassert.h>

#include <bprb/bprb_parameters.h>


bprb_process::bprb_process()
{
  parameters_ = new bprb_parameters;
}


bprb_process::~bprb_process()
{
}


//: Copy Constructor
bprb_process::bprb_process(const bprb_process& other)
  : parameters_(new bprb_parameters(other.parameters_))
{
}


bprb_parameters_sptr bprb_process::parameters()
{
  return parameters_;
}




void bprb_process::set_parameters(const bprb_parameters_sptr& params)
{
  parameters_ = params;
}

 

