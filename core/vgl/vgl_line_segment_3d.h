// This is vxl/vgl/vgl_line_segment_3d.h

//-*- c++ -*-------------------------------------------------------------------
#ifndef vgl_line_segment_3d_h_
#define vgl_line_segment_3d_h_
#ifdef __GNUC__
#pragma interface
#endif
//:
// \file
// \brief line segment in 3D nonhomogeneous space
// \author  awf@robots.ox.ac.uk
//
// \verbatim
// Modifications
// Peter Vanroose - 27 June 2001 - Added operator==
// \endverbatim

#include <vcl_iosfwd.h>
#include <vgl/vgl_point_3d.h> // data member of this class

//: Represents a 3D line segment using two points.
template <class Type>
class vgl_line_segment_3d {
public:

  vgl_line_segment_3d() {}
  vgl_line_segment_3d(vgl_line_segment_3d<Type> const& that);
  vgl_line_segment_3d(vgl_point_3d<Type> const &, vgl_point_3d<Type> const &);
 ~vgl_line_segment_3d();

  //: the equality operator
  bool operator==(vgl_line_segment_3d<Type> const& other) const;
  bool operator!=(vgl_line_segment_3d<Type> const& other) const { return ! operator==(other); }

  vgl_point_3d<Type> const & get_point1() const { return point1_; }
  vgl_point_3d<Type>       & get_point1() { return point1_; }
  vgl_point_3d<Type> const & get_point2() const { return point2_; }
  vgl_point_3d<Type>       & get_point2() { return point2_; }

private:
  vgl_point_3d<Type> point1_;
  vgl_point_3d<Type> point2_;
};

// stream operators
template <class Type>
vcl_ostream&  operator<<(vcl_ostream& s, const vgl_line_segment_3d<Type>& p);

template <class Type>
vcl_istream&  operator>>(vcl_istream& is,  vgl_line_segment_3d<Type>& p);

#define VGL_LINE_SEGMENT_3D_INSTANTIATE(T) extern "please include vgl/vgl_line_segment_3d.txx first"

#endif // vgl_line_segment_3d_h_
