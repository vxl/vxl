// This is brl/bbas/bvgl/bvgl_triangle_3d.h
#ifndef bvgl_triangle_h_
#define bvgl_triangle_h_
//:
// \file
// \brief simple 3d triangle
// \author Jan 30, 2012 Andrew Miller
//

#include <vector>
#include <iostream>
#include <iosfwd>
#include <vgl/vgl_point_3d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class Type>
class bvgl_triangle_3d
{
 public:

  //: Default constructor (creates empty triangle)
  bvgl_triangle_3d();

  //: Construct using two corner points
  bvgl_triangle_3d( vgl_point_3d<Type> const& corner1,
                    vgl_point_3d<Type> const& corner2,
                    vgl_point_3d<Type> const& corner3);

  //: access points
  vgl_point_3d<Type>        operator[](int index) { return points_[index]; }
  //: access points
  vgl_point_3d<Type> const& operator[](int index) const { return points_[index]; }

  // ----- IO --------

  //: Write "<vgl_box_3d x0,y0,z0 to x1,y1,z1>" to stream
  std::ostream& print(std::ostream&) const;

  //: Write "x0 y0 z0 x1 y1 z1(endl)" to stream
  std::ostream& write(std::ostream&) const;

  //: Read x0,y0,z0,x1,y1,z1 from stream
  std::istream& read(std::istream&);

 private:
  vgl_point_3d<Type> points_[3];
};

template <class Type>
std::ostream&  operator<<(std::ostream& s, bvgl_triangle_3d<Type> const& p);

//: Read box from stream
template <class Type>
std::istream&  operator>>(std::istream& is,  bvgl_triangle_3d<Type>& p);

#define BVGL_TRIANGLE_3D(T) extern "please include bbas/bvgl/bvgl_triangle_3d.txx first"

#endif // bvgl_triangle_h_
