// This is vpgl/ihog/ihog_transform_2d.h
#ifndef ihog_transform_2d_h_
#define ihog_transform_2d_h_
//:
// \file
// \brief A vimt_transform_2d with ref_counting
// \author Matt Leotta
// \date 4/27/04
//
// \verbatim
//  Modifications
//   None
// \endverbatim

#include <vbl/vbl_ref_count.h>
#include <vimt/vimt_transform_2d.h>
#include <vsl/vsl_binary_io.h>

//: A quadrilateral region of an image
class ihog_transform_2d : public vimt_transform_2d, public vbl_ref_count
{
public:
  //: Default Constructor
  ihog_transform_2d()
    : vimt_transform_2d() {}

  //: Constructor
  ihog_transform_2d(const vimt_transform_2d& xform)
    : vimt_transform_2d(xform) {}

  //: Destructor
  ~ihog_transform_2d(){}
};




//: Binary save ihog_transform_2d* to stream.
inline void vsl_b_write(vsl_b_ostream &os, const ihog_transform_2d* t)
{
  if (t==0) {
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else{
    vsl_b_write(os,true); // Indicate non-null pointer stored
    t->b_write(os);
  }
}


//: Binary load ihog_transform_2d* from stream.
inline void vsl_b_read(vsl_b_istream &is, ihog_transform_2d* &t)
{
  delete t;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr) {
    t = new ihog_transform_2d();
    t->b_read(is);
  }
  else
    t = 0;
}


//: Print an ASCII summary to the stream
inline void vsl_print_summary(vcl_ostream &os, const ihog_transform_2d* t)
{
  os << *t;
}

#endif // ihog_transform_2d_h_

