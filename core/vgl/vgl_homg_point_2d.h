#ifndef vgl_homg_point_2d_h
#define vgl_homg_point_2d_h
#ifdef __GNUC__
#pragma interface
#endif

// This is vxl/vgl/vgl_homg_point_2d.h

//:
// \file
// \brief point in projective 2D space
// \author Don HAMILTON, Peter TU
//
// \verbatim
// Modifications
// Peter Vanroose -  1 July 2001 - Renamed data to x_ y_ w_, inlined constructors
// Peter Vanroose - 27 June 2001 - Added operator==
// \endverbatim

#include <vcl_cmath.h> // for vcl_abs(double) etc
#include <vcl_cstdlib.h> // for vcl_abs(int) etc
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_fwd.h> // forward declare vgl_homg_line_2d
#include <vcl_iosfwd.h>

//: Represents a homogeneous 2D point.
template <class Type>
class vgl_homg_point_2d {

  // PUBLIC INTERFACE--------------------------------------------------------

public:

  // Constructors/Initializers/Destructors-----------------------------------

  //: Default constructor
  vgl_homg_point_2d () : x_(0), y_(0), w_(Type(1)) {}

  explicit vgl_homg_point_2d<Type> (vgl_point_2d<Type> const& p)
    : x_(p.x()), y_(p.y()), w_(Type(1)) {}

  //: Construct from two (nonhomogeneous) or three (homogeneous) Types.
  vgl_homg_point_2d (Type px, Type py, Type pw = Type(1)) : x_(px), y_(py), w_(pw) {}

  //: Construct from homogeneous 3-vector.
  vgl_homg_point_2d (const Type v[3]) : x_(v[0]), y_(v[1]), w_(v[2]) {}

  //: Construct from 2 lines (intersection).
  vgl_homg_point_2d (vgl_homg_line_2d<Type> const& l1,
                     vgl_homg_line_2d<Type> const& l2);

#if 0
  //: Default copy constructor
  vgl_homg_point_2d (const vgl_homg_point_2d<Type>& that)
    : x_(that.x()), y_(that.y()), w_(that.w()) {}

  // Default destructor
  ~vgl_homg_point_2d () {}

  // Default assignment operator
  vgl_homg_point_2d<Type>& operator=(const vgl_homg_point_2d<Type>& that) {
    set(that.x(),that.y(),that.w());
    return *this;
  }
#endif

  //: the equality operator
  bool operator==(vgl_homg_point_2d<Type> const& other) const;
  bool operator!=(vgl_homg_point_2d<Type> const& other) const { return ! operator==(other); }

  // Data Access-------------------------------------------------------------

  inline Type x() const {return x_;}
  inline Type y() const {return y_;}
  inline Type w() const {return w_;}

  //: Set x,y,w.
  void set (Type px, Type py, Type pw = Type(1)) {
    x_ = px, y_ = py, w_ = pw;
  }

  //: Return true iff the point is at infinity (an ideal point).
  // The method checks that |w| < tol * max(|x|,|y|)
  bool ideal(Type tol = Type(0)) {
    return vcl_abs(w()) <= tol * vcl_abs(x()) ||
           vcl_abs(w()) <= tol * vcl_abs(y());
  }

  // INTERNALS---------------------------------------------------------------

protected:
  // the data associated with this point
  Type x_;
  Type y_;
  Type w_;
};

// stream operators

template <class Type>
vcl_ostream&  operator<<(vcl_ostream& s, const vgl_homg_point_2d<Type>& p);

template <class Type>
vcl_istream&  operator>>(vcl_istream& is,  vgl_homg_point_2d<Type>& p);

#endif // vgl_homg_point_2d_h
