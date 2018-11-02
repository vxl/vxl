#ifndef bocl_device_h_
#define bocl_device_h_
//:
// \file
// \brief  A wrapper for an cl_device_id, and
// \author Andrew Miller acm@computervisiongroup.com
// \date  March 8, 2011
//
#include <string>
#include <vector>
#include <iostream>
#include <cstddef>
#include <iosfwd>
#include "bocl_cl.h"
#include "bocl_utils.h"
#include "bocl_device_info.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vbl/vbl_ref_count.h>
#include <vsl/vsl_binary_io.h>
#include <vbl/vbl_smart_ptr.h>

//:  High level wrapper OpenCL Device
//   Currently the device creates its own context
class bocl_device: public vbl_ref_count
{
  public:
    bocl_device() = default;
    bocl_device(cl_device_id& device);
    ~bocl_device() override;

    //: accessors for context/device
    cl_device_id*     device_id() { return &device_; }
    cl_context&       context() { return context_; }
    bocl_device_info& info() { return info_; }
    //: function which returns a unique string for a device.
    std::string device_identifier();
  private:
    //:Store a pointer to the cl_device_id
    cl_device_id device_;

    //: create and store a context
    cl_context context_;

    //: bocl_device_info for this device
    bocl_device_info info_;
};


typedef vbl_smart_ptr<bocl_device> bocl_device_sptr;
std::ostream& operator <<(std::ostream &s, bocl_device& dev);

//: Binary write bocl_device  to stream
void vsl_b_write(vsl_b_ostream& os, bocl_device const& scene);
void vsl_b_write(vsl_b_ostream& os, const bocl_device* &p);
void vsl_b_write(vsl_b_ostream& os, bocl_device_sptr& sptr);
void vsl_b_write(vsl_b_ostream& os, bocl_device_sptr const& sptr);

//: Binary load bocl_device from stream.
void vsl_b_read(vsl_b_istream& is, bocl_device &scene);
void vsl_b_read(vsl_b_istream& is, bocl_device* p);
void vsl_b_read(vsl_b_istream& is, bocl_device_sptr& sptr);
void vsl_b_read(vsl_b_istream& is, bocl_device_sptr const& sptr);

#endif
