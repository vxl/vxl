#ifndef vgl_homg_line_2d_h
#define  vgl_homg_line_2d_h
#ifdef __GNUC__
#pragma interface
#endif

// This is vxl/vgl/vgl_homg_line_2d.h

//:
// \file
// \brief line in projective 2D space
// \author Don Hamilton, Peter Tu
//
// \verbatim
// Modifications
// Peter Vanroose - 27 June 2001 - Added operator==
// \endverbatim

#include <vcl_iosfwd.h>
#include <vcl_cmath.h> // for vcl_abs(double) etc
#include <vcl_cstdlib.h> // for vcl_abs(int) etc
#include <vgl/vgl_fwd.h> // forward declare vgl_homg_point_2d and vgl_line_2d

//: Represents a homogeneous 2D line.
template <class Type>
class vgl_homg_line_2d {

  // PUBLIC INTERFACE--------------------------------------------------------

public:

  // Constructors/Initializers/Destructors-----------------------------------

  //: Default constructor (leaves line undefined)
  vgl_homg_line_2d () {}

  // Default copy constructor
  // vgl_homg_line_2d (const vgl_homg_line_2d<Type>& that) { *this = that; }

#if 0 // unimp
  vgl_homg_line_2d<Type> (vgl_line_2d<Type> const& p);
#endif

  //: Construct from three Types.
  vgl_homg_line_2d (Type const& a_, Type const& b_, Type const& c_) { set(a_,b_,c_); }

  //: Construct from 3-vector.
  vgl_homg_line_2d (const Type v[3]) { set(v[0],v[1],v[2]); }

  //: Construct from two points (join)
  vgl_homg_line_2d (vgl_homg_point_2d<Type> const& p1, vgl_homg_point_2d<Type> const& p2);

#if 0
  // Default destructor
  ~vgl_homg_line_2d () {}

  // Default assignment operator
  vgl_homg_line_2d<Type>& operator=(const vgl_homg_line_2d<Type>& that){
    set(that.a(),that.b(),that.c());
    return *this;
  }
#endif

  //: the equality operator
  bool operator==(vgl_homg_line_2d<Type> const& other) const;
  bool operator!=(vgl_homg_line_2d<Type> const& other) const { return ! operator==(other); }

  // Data Access-------------------------------------------------------------

#if 0 // TODO
  vcl_vector<Type> get_direction() const;
  vcl_vector<Type> get_normal() const;
#endif

  Type dirx() const { return a(); }  // TODO
  Type diry() const { return b(); }  // TODO
  Type nx() const { return -b(); } // TODO
  Type ny() const { return a(); } // TODO

  Type a() const {return pos_[0];}
  Type b() const {return pos_[1];}
  Type c() const {return pos_[2];}

  //: Set a b c.
  void set (Type const& a_, Type const& b_, Type const& c_){
    pos_[0] = a_;
    pos_[1] = b_;
    pos_[2] = c_;
  }

  //: Return true iff this line is the line at infinity
  //  This version checks (max(|a|,|b|) < tol * c
  bool ideal(Type tol = Type(0)) const {
    return vcl_abs(a()) <= tol*vcl_abs(c()) &&
           vcl_abs(b()) <= tol*vcl_abs(c());
  }

  //:get two points on the line
  // These two points are normally the intersections
  // with the Y axis and X axis, respectively.  When the line is parallel to one
  // of these, the point with y=1 or x=1, resp. are taken.  When the line goes
  // through the origin, the second point is (b, -a, 1).  Finally, when the line
  // is the line at infinity, the returned points are (1,0,0) and (0,1,0).
  // Thus, whenever possible, the returned points are not at infinity.
  void get_two_points(vgl_homg_point_2d<Type> &p1, vgl_homg_point_2d<Type> &p2);

  // INTERNALS---------------------------------------------------------------

protected:
  //: the data associated with this line
  Type pos_[3];
};

//: Print line equation to stream
template <class Type>
vcl_ostream&  operator<<(vcl_ostream& s, const vgl_homg_line_2d<Type>& p);

//: Load in line parameters from stream
template <class Type>
vcl_istream&  operator>>(vcl_istream& is,  vgl_homg_line_2d<Type>& p);

#endif //  vgl_homg_line_2d_h
