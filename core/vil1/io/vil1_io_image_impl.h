// This is vxl/vil/io/vil_io_image_impl.h
#ifndef vil_io_image_impl_h
#define vil_io_image_impl_h
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Ian Scott (Manchester)
// \date 21-Mar-2001

#include <vsl/vsl_binary_io.h>
#include <vil/vil_image_impl.h>

//: Base for objects which provide IO for classes derived from vil_image_impl
class vil_io_image_impl
{
 public:
  //: Constructor
  vil_io_image_impl();

  //: Destructor
  virtual ~vil_io_image_impl();

  //: Create new object of type vil_image_impl on heap
  virtual vil_image_impl* new_object() const = 0;

  //: Write derived class to os using vil_image_impl reference
  virtual void b_write_by_base(vsl_b_ostream& os,
    const vil_image_impl& base) const;

  //: Write derived class to os using vil_image_impl reference
  virtual void b_read_by_base(vsl_b_istream& is,
    vil_image_impl& base) const;

  //: Print summary of derived class to os using vil_image_impl reference
  virtual void print_summary_by_base(vcl_ostream& os,
                                     const vil_image_impl& base) const;

  //: Copy this object onto the heap and return a pointer
  virtual vil_io_image_impl* clone() const =0;

  //: Return name of class for which this object provides IO
  virtual vcl_string target_classname() const;

  //: Return true if b is of class target_classname()
  //  Typically this will just be "return b.is_a()==target_classname()"
  //  However, third party libraries may use a different system
  virtual bool is_io_for(const vil_image_impl& b) const;
};

//: Add example object to list of those that can be loaded
//  The vsl_binary_loader must see an example of each derived class
//  before it knows how to deal with them.
//  A clone is taken of b
void vsl_add_to_binary_loader(const vil_io_image_impl& b);

//: Binary save to stream by vil_image_impl pointer
void vsl_b_write(vsl_b_ostream &os, const vil_image_impl * b);

//: Binary read from stream by vil_image_impl pointer
void vsl_b_read(vsl_b_istream &is, vil_image_impl* &b);

//: Print summary to stream by vil_image_impl pointer
void vsl_print_summary(vcl_ostream &os, const vil_image_impl * b);

//: Binary save vil_real_polynomial to stream.
void vsl_b_write(vsl_b_ostream &os, const vil_image_impl & v);

//: Binary load vil_real_polynomial from stream.
void vsl_b_read(vsl_b_istream &is, vil_image_impl & v);

//: Print human readable summary of object to a stream
void vsl_print_summary(vcl_ostream& os,const vil_image_impl & b);

#endif // vil_io_image_impl_h
