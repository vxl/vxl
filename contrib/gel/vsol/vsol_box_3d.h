// This is gel/vsol/vsol_box_3d.h
#ifndef vsol_box_3d_h_
#define vsol_box_3d_h_
//:
// \file
// \brief A bounding box
//
//  Note that the definition of width, depth and
//  height are such that the X-Y plane is considered
//  as a Rectangle with a "width" and "height"
//  according to the usual definition. The figure
//  shows a right-handed coordinate system, but there
//  is no commitment to that in the definitions
// \verbatim
//                       |<--width-->|
//                 Z     O-----------O  ---
//                    | /           /|   ^
//                    |/           / |   |
//                    O-----------O  | depth
//                    |           |  |   |
//                    |  centroid |  |   v
//                    |  Y  o     |  O  ---
//                    | /         | /   /_____height
//                    |/          |/   /
//                    O-----------O  --- X
// \endverbatim
//
// \verbatim
//  Modifications
//   2003/01/09 Peter Vanroose deprecated set_min_x() etc. and replaced with
//                             more safe add_point()
// \endverbatim

#include <vul/vul_timestamp.h>
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_bounding_box.h>

//: A bounding box for 3d spatial objects

class vsol_box_3d : public vbl_ref_count , public vul_timestamp
{
 protected:
  vbl_bounding_box<double,3> box_;

 public:
  //: create an empty box
  vsol_box_3d() {}

  vsol_box_3d(vsol_box_3d const& b);

  vsol_box_3d(vbl_bounding_box<double,3> const &b){box_=b;}

  ~vsol_box_3d() {}

  // accessors

  double get_min_x();
  double get_max_x();

  double get_min_y();
  double get_max_y();

  double get_min_z();
  double get_max_z();

  //: enlarge the bounding box by adding the point (x,y,z) & taking convex hull
  void add_point(double x, double y, double z);

  //: Compare this' bounds to comp_box and grow to the maximum bounding box
  void grow_minmax_bounds(vsol_box_3d & comp_box);

  double width() { return get_max_x() - get_min_x(); }
  double height() { return get_max_y() - get_min_y(); }
  double depth() { return get_max_z() - get_min_z(); }
  double volume() { return width() * height() * depth(); }

  bool operator< (vsol_box_3d& box);  // a<b = a is inside b

  bool near_equal(vsol_box_3d& box, float tolerance); // is box about the same as this?
  //: reset the bounds of the box
  void reset_bounds();
};

#endif // vsol_box_3d_h_
