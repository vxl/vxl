#ifndef vsol_box_2d_H
#define vsol_box_2d_H
//
// .NAME        vsol_box_2d - A bounding box
// .LIBRARY     vsol
// .HEADER      gel package
// .INCLUDE     vsol/vsol_box_2d.h
// .FILE        vsol_box_2d.cxx
//
// .SECTION Description
//                     Note that the definition of width, depth and
//                     height are such that the X-Y plane is considered
//                     as a Rectangle with a "width" and "height"
//                     according to the usual definition.
//

#include <vsol/vsol_box_2d_sptr.h>


#include <vul/vul_timestamp.h>
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_bounding_box.h>

//: \brief a bounding box for spatial objects


class vsol_box_2d
  : public vbl_ref_count,
    public vul_timestamp
{
 public:

  vsol_box_2d(void);
  vsol_box_2d(vsol_box_2d const &b);

  ~vsol_box_2d();

  //: \brief accessors

  double get_min_x(void);
  double get_max_x(void);

  double get_min_y(void);
  double get_max_y(void);

  void set_min_x(const double &v);
  void set_max_x(const double &v);

  void set_min_y(const double &v);
  void set_max_y(const double &v);

  //Compare this' bounds to comp_box and grow to the maximum bounding box
  void grow_minmax_bounds(vsol_box_2d & comp_box);

 protected:

  vbl_bounding_box<double,2> _box;
};

#endif // vsol_box_2d_H
