#ifndef vgl_homg_line_3d_2_points_h_
#define vgl_homg_line_3d_2_points_h_

// This is vxl/vgl/vgl_homg_line_3d_2_points.h

//:
// \file
// \author Don HAMILTON Peter TU François BERTEL
//

#ifdef __GNUC__
#pragma interface
#endif

#include <vcl_iostream.h>
#include <vgl/vgl_homg_point_3d.h>

//:Represents a homogeneous 3D line using two points
// A class to hold a homogeneous representation of a 3D Line.  The line is
// stored as a pair of homogeneous 3d points.
template <class Type>
class vgl_homg_line_3d_2_points
{
  //+**************************************************************************
  // Initialization
  //+**************************************************************************
public:
  //: Default constructor with (0,0,0,1) and (1,0,0,0)
  explicit vgl_homg_line_3d_2_points(void)
  : point_finite_(0,0,0,1), point_infinite_(1,0,0,0) {}

  //: Copy constructor
  vgl_homg_line_3d_2_points(const vgl_homg_line_3d_2_points<Type> &that)
  : point_finite_(that.point_finite_), point_infinite_(that.point_infinite_) {}

  //: Construct from two points
  vgl_homg_line_3d_2_points(vgl_homg_point_3d<Type> const& point_finite,
                            vgl_homg_point_3d<Type> const& point_infinite);

  //: Destructor (does nothing)
  ~vgl_homg_line_3d_2_points() {}

  // Data access

  //: Finite point
  vgl_homg_point_3d<Type>const& get_point_finite() const {return point_finite_;}
  //: Finite point
  vgl_homg_point_3d<Type>     & get_point_finite()       {return point_finite_;}
  //: Infinite point
  vgl_homg_point_3d<Type>const& get_point_infinite()const{return point_infinite_;}
  //: Infinite point
  vgl_homg_point_3d<Type>     & get_point_infinite()     {return point_infinite_;}

  // Utility methods

  //: force the point point_infinite_ to infinity, without changing the line
  void force_point2_infinite(void);

  // Internals
protected:
  // Data Members------------------------------------------------------------

  //: Any finite point on the line
  vgl_homg_point_3d<Type> point_finite_;
  //: the (unique) point at infinity
  vgl_homg_point_3d<Type> point_infinite_;
};

//+****************************************************************************
// stream operators
//+****************************************************************************

//: Write to stream (verbose)
template <class Type>
vcl_ostream &operator<<(vcl_ostream &s,
                    const vgl_homg_line_3d_2_points<Type> &p);

//: Read parameters from stream
template <class Type>
vcl_istream &operator>>(vcl_istream &is,
                    vgl_homg_line_3d_2_points<Type> &p);

#endif // vgl_homg_line_3d_2_points_h_
