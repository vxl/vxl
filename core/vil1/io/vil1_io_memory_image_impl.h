// This is vxl/vil/io/vil_io_memory_image_impl.h
#ifndef vil_io_memory_image_impl_h
#define vil_io_memory_image_impl_h
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Nick Costen and Ian Scott (Manchester)
// \date 21-Mar-2001

#include <vsl/vsl_binary_io.h>
#include <vil/io/vil_io_image_impl.h>

class vil_memory_image_impl;

//: Provide IO for vil_memory_image
class vil_io_memory_image_impl : public vil_io_image_impl
{
 public:
  //: Constructor
  vil_io_memory_image_impl();

  //: Destructor
  virtual ~vil_io_memory_image_impl();

  //: Create new object of type vil_memory_image_impl on heap
  virtual vil_image_impl* new_object() const;

  //: Write derived class to os using baseclass reference
  virtual void b_write_by_base(vsl_b_ostream& os, const vil_image_impl& base) const;

  //: Write derived class to os using baseclass reference
  virtual void b_read_by_base(vsl_b_istream& is, vil_image_impl& base) const;

  //: Copy this object onto the heap and return a pointer
  virtual vil_io_image_impl* clone() const;

  //: Return name of class for which this object provides IO
  virtual vcl_string target_classname() const;

  //: Return true if b is of class vil_memory_image_impl
  virtual bool is_io_for(const vil_image_impl& b) const;
};


//: Binary save vil_memory_image_impl to stream.
void vsl_b_write(vsl_b_ostream &os, const vil_memory_image_impl & v);

//: Binary load vil_memory_image_impl from stream.
void vsl_b_read(vsl_b_istream &is, vil_memory_image_impl & v);

//: Print human readable summary of vil_memory_image_impl to a stream
void vsl_print_summary(vcl_ostream& os, const vil_memory_image_impl & b);


#endif // vil_io_memory_image_impl_h
