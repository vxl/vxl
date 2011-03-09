#ifndef bocl_device_h_
#define bocl_device_h_
//:
// \file
// \brief  A wrapper for an cl_device_id, and 
// \author Andrew Miller acm@computervisiongroup.com
// \date  March 8, 2011
//
#include "bocl_cl.h"
#include "bocl_utils.h"
#include "bocl_device_info.h"
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_cstddef.h> // for std::size_t
#include <vcl_iosfwd.h>

//:  High level wrapper OpenCL Device
//   Currently the device creates it's own context
class bocl_device
{
  public:
    bocl_device() : device_(0) {}
    bocl_device(cl_device_id& device); 
    virtual ~bocl_device();
  
    //: accessors for context/device
    cl_device_id*     device_id() { return device_; }
    cl_context&       context() { return context_; }
    bocl_device_info& info() { return info_; }
  private: 
  
    //:Store a pointer to the cl_device_id
    cl_device_id* device_; 
  
    //: create and store a context
    cl_context context_; 
  
    //: bocl_device_info for this device
    bocl_device_info info_; 
};

vcl_ostream& operator <<(vcl_ostream &s, bocl_device& dev);


#endif
