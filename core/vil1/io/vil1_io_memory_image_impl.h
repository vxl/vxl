// This is core/vil1/io/vil1_io_memory_image_impl.h
#ifndef vil1_io_memory_image_impl_h
#define vil1_io_memory_image_impl_h
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Nick Costen and Ian Scott (Manchester)
// \date 21-Mar-2001

#include <vsl/vsl_binary_io.h>
#include <vil1/io/vil1_io_image_impl.h>

class vil1_memory_image_impl;

//: Provide IO for vil1_memory_image
class vil1_io_memory_image_impl : public vil1_io_image_impl
{
 public:
  //: Constructor
  vil1_io_memory_image_impl();

  //: Destructor
  virtual ~vil1_io_memory_image_impl();

  //: Create new object of type vil1_memory_image_impl on heap
  virtual vil1_image_impl* new_object() const;

  //: Write derived class to os using baseclass reference
  virtual void b_write_by_base(vsl_b_ostream& os, const vil1_image_impl& base) const;

  //: Write derived class to os using baseclass reference
  virtual void b_read_by_base(vsl_b_istream& is, vil1_image_impl& base) const;

  //: Copy this object onto the heap and return a pointer
  virtual vil1_io_image_impl* clone() const;

  //: Return name of class for which this object provides IO
  virtual vcl_string target_classname() const {return "vil1_memory_image_impl";}

  //: Return true if b is of class vil1_memory_image_impl
  virtual bool is_io_for(const vil1_image_impl& b) const { return b.is_a()==target_classname(); }
};


//: Binary save vil1_memory_image_impl to stream.
void vsl_b_write(vsl_b_ostream &os, const vil1_memory_image_impl & v);

//: Binary load vil1_memory_image_impl from stream.
void vsl_b_read(vsl_b_istream &is, vil1_memory_image_impl & v);

//: Print human readable summary of vil1_memory_image_impl to a stream
void vsl_print_summary(vcl_ostream& os, const vil1_memory_image_impl & b);


#endif // vil1_io_memory_image_impl_h
