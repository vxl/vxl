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
// Peter Vanroose - 27 June 2001 - Added operator==
// \endverbatim

#include <vcl_iostream.h>
#include <vcl_cmath.h> // for vcl_abs(double) etc
#include <vcl_cstdlib.h> // for vcl_abs(int) etc

template <class Type>
class vgl_point_2d;

template <class Type>
class vgl_homg_line_2d;

//: Represents a homogeneous 2D point.
template <class Type>
class vgl_homg_point_2d {

  // PUBLIC INTERFACE--------------------------------------------------------

public:

  // Constructors/Initializers/Destructors-----------------------------------

  //: Default constructor
  vgl_homg_point_2d () {}

#if 0
  //: Default copy constructor
  vgl_homg_point_2d (const vgl_homg_point_2d<Type>& that) {
    set(that.x(),that.y(),that.w());
  }
#endif

#if 0 // unimp
  vgl_homg_point_2d<Type> (vgl_point_2d<Type> const& p);
#endif

  //: Construct from two Types (nonhomogeneous interface)
  vgl_homg_point_2d (Type px, Type py) { set(px,py); }

  //: Construct from three Types.
  vgl_homg_point_2d (Type px, Type py, Type pw) { set(px,py,pw); }

  //: Construct from homogeneous 3-vector.
  vgl_homg_point_2d (const Type v[3]) { set(v[0],v[1],v[2]); }

  //: Construct from 2 lines (intersection).
  vgl_homg_point_2d (vgl_homg_line_2d<Type> const& l1,
                     vgl_homg_line_2d<Type> const& l2);

#if 0
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

  inline Type x() const {return data_[0];}
  inline Type y() const {return data_[1];}
  inline Type w() const {return data_[2];}
  inline Type& x() {return data_[0];}
  inline Type& y() {return data_[1];}
  inline Type& w() {return data_[2];}

  //: Set x,y,w.
  void set (Type px, Type py, Type pw = (Type)1) {
    data_[0] = px,
    data_[1] = py,
    data_[2] = pw;
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
  Type data_[3];
};

// stream operators
template <class Type>
vcl_ostream&  operator<<(vcl_ostream& s, const vgl_homg_point_2d<Type>& p) {
  return s << " <vgl_homg_point_2d ("
           << p.x() << "," << p.y() << "," << p.w() << ") >";
}

template <class Type>
vcl_istream&  operator>>(vcl_istream& is,  vgl_homg_point_2d<Type>& p) {
  return is >> p.x() >> p.y() >> p.w() ;
}

#endif // vgl_homg_point_2d_h
