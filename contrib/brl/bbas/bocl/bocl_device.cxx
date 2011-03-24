#include "bocl_device.h"
#include "bocl_utils.h"
//:
// \file
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_sstream.h>
#include <vcl_cstdio.h>
#include <vcl_cstring.h>


bocl_device::bocl_device(cl_device_id& device)
{
  device_ = new cl_device_id[1];
  device_[0] = device;
  info_ = bocl_device_info(device_);

  //vcl_cout<<"Device specs: "<<info_<<vcl_endl;

  //Create a context from the device ID
  int status = 1;
  context_ = clCreateContext(0, 1, device_, NULL, NULL, &status);
  if (!check_val(status,CL_SUCCESS,"clCreateContextFromType failed: " + error_to_string(status))) {
    return;
  }
}

vcl_string bocl_device::device_identifier()
{
    vcl_stringstream outstr;
    outstr<<(long)(device_[0]);
    return info_.device_name_+outstr.str();
}
//destructor
bocl_device::~bocl_device()
{
  if (context_) clReleaseContext(context_);
  if (device_) delete[] device_;
}


vcl_ostream& operator <<(vcl_ostream &s, bocl_device& dev)
{
  s << dev.info() << vcl_endl;
  return s;
}


//: Binary save bocl_device  from stream.
void vsl_b_write(vsl_b_ostream& os, bocl_device const& scene){}
void vsl_b_write(vsl_b_ostream& os, const bocl_device* &p){}
void vsl_b_write(vsl_b_ostream& os, bocl_device_sptr& sptr){}
void vsl_b_write(vsl_b_ostream& os, bocl_device_sptr const& sptr){}

//: Binary load bocl_device  from stream.
void vsl_b_read(vsl_b_istream& is, bocl_device &scene){}
void vsl_b_read(vsl_b_istream& is, bocl_device* p){}
void vsl_b_read(vsl_b_istream& is, bocl_device_sptr& sptr){}
void vsl_b_read(vsl_b_istream& is, bocl_device_sptr const& sptr){}
