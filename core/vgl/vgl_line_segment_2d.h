#ifndef vgl_line_segment_2d_h_
#define vgl_line_segment_2d_h_
#ifdef __GNUC__
#pragma interface
#endif

// This is vxl/vgl/vgl_line_segment_2d.h

//:
// \file
// \author mccane@cs.otago.ac.nz: but copied from vgl_line_segment_3d
//
// \verbatim
// Modifications
// Peter Vanroose - 27 June 2001 - Added operator==
// \endverbatim

#include <vcl_iosfwd.h>
#include <vgl/vgl_point_2d.h> // data member of this class

//: Represents a 2D line segment using two points.
template <class Type>
class vgl_line_segment_2d {
public:

  //: Default constructor
  vgl_line_segment_2d() {}

  //: Copy constructor
  vgl_line_segment_2d(vgl_line_segment_2d<Type> const& that);
  vgl_line_segment_2d(vgl_point_2d<Type> const &, vgl_point_2d<Type> const &);
 ~vgl_line_segment_2d();

  //: the equality operator
  bool operator==(vgl_line_segment_2d<Type> const& other) const;
  bool operator!=(vgl_line_segment_2d<Type> const& other) const { return ! operator==(other); }

  vgl_point_2d<Type> const & get_point1() const { return point1_; }
  vgl_point_2d<Type>       & get_point1() { return point1_; }
  vgl_point_2d<Type> const & get_point2() const { return point2_; }
  vgl_point_2d<Type>       & get_point2() { return point2_; }

private:
  //: One end of line segment
  vgl_point_2d<Type> point1_;
  //: The other end of the line segment
  vgl_point_2d<Type> point2_;
};

//: Write to stream
template <class Type>
vcl_ostream&  operator<<(vcl_ostream& s, const vgl_line_segment_2d<Type>& p);

//: Read from stream
template <class Type>
vcl_istream&  operator>>(vcl_istream& is,  vgl_line_segment_2d<Type>& p);

#endif // vgl_line_segment_2d_h_
