#ifndef vgl_line_2d_h_
#define vgl_line_2d_h_
#ifdef __GNUC__
#pragma interface
#endif

// This is vxl/vgl/vgl_line_2d.h

//:
// \file
// \author Don Hamilton, Peter Tu, Peter VANROOSE, François BERTEL, Franck Bettinger
//
// \verbatim
// Modifications :
// 2001/06/27 Peter Vanroose         Added operator==
// 2001/03/19 Franck Bettinger       added Manchester binary IO code
// 2000/12/01 Peter Vanroose         moved dist_origin() to vgl_distance.h
// 2000/05/09 Peter VANROOSE         dist_origin() re-implemented
// 2000/05/05 François BERTEL        Several minor bugs fixed
// 2000/02/29 Peter VANROOSE         Several minor fixes
// 2000/02/16 Don HAMILTON, Peter TU Creation
// \endverbatim

template <class Type>
class vgl_line_2d;

#include <vcl_iostream.h>
#include <vcl_cmath.h> // for vcl_sqrt()
#include <vcl_string.h>

template <class Type>
class vgl_point_2d;

template <class Type>
class vgl_homg_line_2d;

//:Represents an euclidian 2D line
// An interface for the line coefficients, [a,b,c], is provided in terms of the
// standard implicit line equation: a*x + b*y + c = 0
template <class Type>
class vgl_line_2d {

// PUBLIC INTERFACE--------------------------------------------------------

public:
  //: Default constructor (Line 1.x==0)
  vgl_line_2d() { data_[0]=1; data_[1]=0; data_[2]=0; }

  //: Construct from homogeneous description of line
  vgl_line_2d<Type> (vgl_homg_line_2d<Type> const& p);

  //: Construct a vgl_line_2d from its equation, three Types.
  vgl_line_2d (Type a, Type b, Type c) { data_[0]=a; data_[1]=b; data_[2]=c; }

  //:  Construct from two distinct points
  vgl_line_2d (vgl_point_2d<Type> const& p1, vgl_point_2d<Type> const& p2) {
    data_[0] = p1.y() - p2.y();
    data_[1] = p2.x() - p1.x();
    data_[2] = p1.x() * p2.y() - p1.y() * p2.x();
  }

  //: Construct from its equation, a 3-vector.
  vgl_line_2d (const Type v[3]) { data_[0]=v[0];data_[1]=v[1];data_[2]=v[2]; }

#if 0
  // Default destructor:
  ~vgl_line_2d () {}

  // Default assignment operator:
  vgl_line_2d<Type>& operator=(const vgl_line_2d<Type>& that){
    data_[0]=that.data_[0];
    data_[1]=that.data_[1];
    data_[2]=that.data_[2];
    return *this;
  }
#endif

  //: the equality operator
  bool operator==(vgl_line_2d<Type> const& other) const;
  bool operator!=(vgl_line_2d<Type> const& other) const { return ! operator==(other); }

  // Data Access-------------------------------------------------------------

  //: Get elements of unit vector describing line direction
  inline void get_direction(Type& dx, Type& dy) const { dx=dir_x(); dy=dir_y(); }

  //: Get elements of unit vector orthogonal to line
  inline void get_normal(Type& nx, Type& ny) const { nx=normal_x(); ny=normal_y(); }

  //: x component of unit vector describing direction of line
  inline Type dir_x() const { return -b()/(vcl_sqrt(a()*a() + b()*b())); }

  //: y component of unit vector describing direction of line
  inline Type dir_y() const { return a()/(vcl_sqrt(a()*a() + b()*b())); }

  //: x component of unit vector orthogonal to line
  inline Type normal_x() const { return -dir_y(); }

  //: y component of unit vector orthogonal to line
  inline Type normal_y() const { return dir_x(); }

  //: Parameter of line a()*x + b()*y + c() = 0
  inline Type a() const {return data_[0];}
  //: Parameter of line a()*x + b()*y + c() = 0
  inline Type b() const {return data_[1];}
  //: Parameter of line a()*x + b()*y + c() = 0
  inline Type c() const {return data_[2];}

  //: Set a b c.
  void set (Type a, Type b, Type c){ data_[0] = a; data_[1] = b; data_[2] = c; }

  //: Get two points on the line; normally the intersection with X and Y axes.
  // When the line is parallel to one
  // of these, the point with y=1 or x=1, resp. are taken.  When the line goes
  // through the origin, the second point is (b, -a).
  void get_two_points(vgl_point_2d<Type> &p1, vgl_point_2d<Type> &p2);

  // INTERNALS---------------------------------------------------------------

protected:
  // the data associated with this point
  Type data_[3];
};

//: Write line description to stream: "<vgl_line_2d ax+by+c>"
template <class Type>
vcl_ostream&  operator<<(vcl_ostream& s, const vgl_line_2d<Type>& p) {
  return s << " <vgl_line_2d " << p.a() << " x + " << p.b()
           << " y + " << p.c() << " = 0>";
}

//: Read in three line parameters from stream
template <class Type>
vcl_istream&  operator>>(vcl_istream& is,  vgl_line_2d<Type>& p) {
  Type a,b,c;
  is >> a >> b >> c;
  p.set(a,b,c);
  return is;
}

#endif // vgl_line_2d_h_
