#ifndef vsol_box_3d_H
#define vsol_box_3d_H
//:
//  \file
// \brief A bounding box
//
//   Note that the definition of width, depth and 
//   height are such that the X-Y plane is considered
//   as a Rectangle with a "width" and "height" 
//   according to the usual definition. The figure 
//   shows a right-handed coordinate system, but there
//   is no commitment to that in the definitions
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

#include <vul/vul_timestamp.h>
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_bounding_box.h>

//: \brief A bounding box for 3d spatial objects

class vsol_box_3d : public vbl_ref_count , public vul_timestamp
{
 public:

  vsol_box_3d();
  vsol_box_3d(vsol_box_3d const& b);

  ~vsol_box_3d();

  //: \brief accessors

  double get_min_x();
  double get_max_x();

  double get_min_y();
  double get_max_y();

  double get_min_z();
  double get_max_z();


  void set_min_x(const double& v);
  void set_max_x(const double& v);

  void set_min_y(const double& v);
  void set_max_y(const double& v);

  void set_min_z(const double& v);
  void set_max_z(const double& v);

  //: Compare this' bounds to comp_box and grow to the maximum bounding box
  void grow_minmax_bounds(vsol_box_3d & comp_box);

  double width();
  double height();

  bool operator< (vsol_box_3d& box);  // a<b = a is inside b

  bool near_equal(vsol_box_3d& box, const float& tolerance); // is box about the same as this?

 protected:

  vbl_bounding_box<double,3> _box;
};

#endif // vsol_box_3d_H
