// This is gel/vsol/vsol_box_2d.h
#ifndef vsol_box_2d_h_
#define vsol_box_2d_h_
//:
// \file
// \brief A bounding box
//
// This is a time stamped and refcounted interface to vbl_box<double,2>
//
// \verbatim
//  Modifications
//   2003/01/09 Peter Vanroose deprecated set_min_x() etc. and replaced with
//                             more safe add_point()
//   2004/05/10 Joseph Mundy   added binary I/O methods
//   2004/05/10 Joseph Mundy   changed bounds access methods to const
//   2004/09/27 Peter Vanroose grow_minmax_bounds() now takes smart pointer arg
// \endverbatim

#include <vsl/vsl_binary_io.h>
#include <vul/vul_timestamp.h>
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_bounding_box.h>
#include "vsol_box_2d_sptr.h"

//: A bounding box for 2d spatial objects

class vsol_box_2d : public vbl_ref_count, public vul_timestamp
{
 protected:
  vbl_bounding_box<double,2> box_;

 public:
  //: create an empty box
  vsol_box_2d() {}

  vsol_box_2d(vsol_box_2d const &b)
    : vbl_ref_count(), vul_timestamp(), box_(b.box_) {}

  vsol_box_2d(vbl_bounding_box<double,2> const &b) : box_(b) {}

  ~vsol_box_2d() {}

  // accessors

  double get_min_x() const;
  double get_max_x() const;

  double get_min_y() const;
  double get_max_y() const;

  double width() const { return get_max_x() - get_min_x(); }
  double height()const { return get_max_y() - get_min_y(); }
  double area() const { return width() * height(); }

  //: enlarge the bounding box by adding the point (x,y) and taking convex hull
  void add_point(double x, double y);

  //: Compare this' bounds to comp_box and grow to the maximum bounding box.
  //  I.e., take the convex union of this and comp_box
  void grow_minmax_bounds(vsol_box_2d_sptr const& comp_box);

  //: a<b means a is inside b
  bool operator< (vsol_box_2d& box) const;

  //: is box about the same as this?
  bool near_equal(vsol_box_2d const& box, float tolerance) const;

  //: reset the bounds of the box, i.e., make the box empty
  void reset_bounds();

  // ==== Binary IO methods ======

  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  void b_read(vsl_b_istream &is);

  //: Return IO version number;
  short version() const;

  //: Print an ascii summary to the stream
  void print_summary(vcl_ostream &os) const;

  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const { return vcl_string("vsol_box_2d"); }

  //: Return true if the argument matches the string identifying the class or any parent class
  virtual bool is_class(const vcl_string& cls) const { return cls==is_a(); }
};

//: Stream operator
vcl_ostream&  operator<<(vcl_ostream& s, vsol_box_2d const& p);

//: Binary save vsol_box_2d* to stream.
void vsl_b_write(vsl_b_ostream &os, vsol_box_2d_sptr const& p);

//: Binary load vsol_box_2d* from stream.
void vsl_b_read(vsl_b_istream &is, vsol_box_2d_sptr &p);

//: Print human readable summary of box to a stream
//  (This is needed for the instantiation of vsl_vector_io<vsol_box_2d_sptr>)
inline void vsl_print_summary(vcl_ostream& os, vsol_box_2d_sptr const& b)
{
  os << *b;
}

#endif // vsol_box_2d_h_
