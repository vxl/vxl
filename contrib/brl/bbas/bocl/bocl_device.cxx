#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <cstring>
#include "bocl_device.h"
#include "bocl_utils.h"
//:
// \file
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


bocl_device::bocl_device(cl_device_id& device) : device_(device)
{
  info_ = bocl_device_info(&device_);

  //std::cout<<"Device specs: "<<info_<<std::endl;

  //Create a context from the device ID
  int status = 1;
  context_ = clCreateContext(nullptr, 1, &device_, nullptr, nullptr, &status);
  if (!check_val(status,CL_SUCCESS,"clCreateContextFromType failed: " + error_to_string(status))) {
    return;
  }
}

std::string bocl_device::device_identifier()
{
    std::stringstream outstr;
    outstr<<(long)(device_);
    return info_.device_vendor_+info_.device_name_+outstr.str();
}
//destructor
bocl_device::~bocl_device()
{
  if (context_) clReleaseContext(context_);
}


std::ostream& operator <<(std::ostream &s, bocl_device& dev)
{
  s << dev.info() << std::endl;
  return s;
}


//: Binary save bocl_device  from stream.
void vsl_b_write(vsl_b_ostream&  /*os*/, bocl_device const&  /*scene*/){}
void vsl_b_write(vsl_b_ostream&  /*os*/, const bocl_device* & /*p*/){}
void vsl_b_write(vsl_b_ostream&  /*os*/, bocl_device_sptr&  /*sptr*/){}
void vsl_b_write(vsl_b_ostream&  /*os*/, bocl_device_sptr const&  /*sptr*/){}

//: Binary load bocl_device  from stream.
void vsl_b_read(vsl_b_istream&  /*is*/, bocl_device & /*scene*/){}
void vsl_b_read(vsl_b_istream&  /*is*/, bocl_device*  /*p*/){}
void vsl_b_read(vsl_b_istream&  /*is*/, bocl_device_sptr&  /*sptr*/){}
void vsl_b_read(vsl_b_istream&  /*is*/, bocl_device_sptr const&  /*sptr*/){}
