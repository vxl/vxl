#ifndef vgl_homg_point_2d_h
#define vgl_homg_point_2d_h
#ifdef __GNUC__
#pragma interface
#endif

// Author: Don Hamilton, Peter Tu
// Copyright:
// Created: Feb 15 2000
// Modifications:
//  Peter Vanroose, 29 Feb 2000: several minor fixes and additions

#include <vcl/vcl_iostream.h>
#include <vcl/vcl_algorithm.h>
#include <vcl/vcl_cmath.h> // for fabs()

template <class Type>
class vgl_point_2d;

//: Represents a homogeneous 2D point.
template <class Type>
class vgl_homg_point_2d {

  // PUBLIC INTERFACE--------------------------------------------------------
  
public:
 
  // Constructors/Initializers/Destructors-----------------------------------

  // Default constructor  
  // vgl_homg_point_2d () {}
  
  // Default copy constructor  
  // vgl_homg_point_2d (const vgl_homg_point_2d<Type>& that) {
  //   set(that.x(),that.y(),that.w());
  // }

  vgl_homg_point_2d<Type> (vgl_point_2d<Type> const& p);

  // -- Construct from two Types (nonhomogeneous interface)
  vgl_homg_point_2d (Type px, Type py) { set(px,py,1.0); }

  // -- Construct from three Types.
  vgl_homg_point_2d (Type px, Type py, Type pw) { set(px,py,pw); }

  // -- Construct from homogeneous 3-vector.
  vgl_homg_point_2d (const Type v[3]) { set(v[0],v[1],v[3]); }

  // Default destructor
  // ~vgl_homg_point_2d () {}

  // Default assignment operator
  // vgl_homg_point_2d<Type>& operator=(const vgl_homg_point_2d<Type>& that) {
  //   set(that.x(),that.y(),that.w());
  //   return *this;
  // }

  // Data Access-------------------------------------------------------------

  inline Type x() const {return data_[0];}
  inline Type y() const {return data_[1];}
  inline Type w() const {return data_[2];}

  // -- Set x,y,w.
  inline void set (Type px, Type py, Type pw) {
    data_[0] = px,
    data_[1] = py,
    data_[2] = pw;
  }

  // -- Return true iff the point is at infinity (an ideal point).
  // The method checks that |w| < tol * min(|x|,|y|)
  inline bool ideal(Type tol) {
    return fabs(w()) < tol * vcl_min(fabs(x()),fabs(y()));
  }

  // INTERNALS---------------------------------------------------------------

protected:
  // the data associated with this point 
  Type data_[3];
};

// stream operators 
template <class Type>
ostream&  operator<<(ostream& s, const vgl_homg_point_2d<Type>& p) {
  return s << " <vgl_homg_point_2d ("
           << p->data_[0] << "," << p->data_[1] << "," << p->data_[2] << ") >";
}

template <class Type>
istream&  operator>>(istream& is,  vgl_homg_point_2d<Type>& p) {
  return is >> p->data_[0] >> p->data_[1] >> p->data_[2] ;
}

#endif
