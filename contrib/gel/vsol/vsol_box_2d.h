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
// \endverbatim

#include <vul/vul_timestamp.h>
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_bounding_box.h>

//: a bounding box for spatial objects

class vsol_box_2d : public vbl_ref_count, public vul_timestamp
{
 protected:
  vbl_bounding_box<double,2> box_;

 public:
  //: create an empty box
  vsol_box_2d(void) {}

  vsol_box_2d(vsol_box_2d const &b);

  vsol_box_2d(vbl_bounding_box<double,2> const &b){box_=b;}

  ~vsol_box_2d() {}

  // accessors

  double get_min_x(void);
  double get_max_x(void);

  double get_min_y(void);
  double get_max_y(void);

  inline double width() { return get_max_x() - get_min_x(); }
  inline double height() { return get_max_y() - get_min_y(); }
  inline double area() { return width() * height(); }

  //: enlarge the bounding box by adding the point (x,y) and taking convex hull
  void add_point(double x, double y);

  //: Compare this' bounds to comp_box and grow to the maximum bounding box.
  //  I.e., take the convex union of this and comp_box
  void grow_minmax_bounds(vsol_box_2d & comp_box);

  bool operator< (vsol_box_2d& b);  // a<b means a is inside b

  // is box about the same as this?
  bool near_equal(vsol_box_2d& b, float tolerance);

  //: reset the bounds of the box, i.e., make the box empty
  void reset_bounds();
};

#endif // vsol_box_2d_h_
