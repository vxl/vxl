#ifndef vgl_homg_point_3d_h
#define  vgl_homg_point_3d_h
#ifdef __GNUC__
#pragma interface
#endif

// Author: Don Hamilton, Peter Tu
// Copyright:
// Created: Feb 15 2000
// Modifications:
//  Peter Vanroose, 29 Feb 2000: several minor fixes

#include <vcl/vcl_iostream.h>
#include <vcl/vcl_algorithm.h>
#include <vcl/vcl_cmath.h> // for fabs()

template <class Type>
class vgl_point_3d;

//: Represents a homogeneous 3D point.
template <class Type>
class vgl_homg_point_3d {

  // PUBLIC INTERFACE--------------------------------------------------------  
public:
 
  // Constructors/Initializers/Destructors-----------------------------------

  // Default constructor  
  // vgl_homg_point_3d () {}
  
  // Default copy constructor  
  // vgl_homg_point_3d (const vgl_homg_point_3d<Type>& that) {
  //   set(that.x(),that.y(),that.z(),that.w());
  // }

  vgl_homg_point_3d<Type> (vgl_point_3d<Type> const& p);

  // -- Construct from four Types.
  vgl_homg_point_3d (Type px, Type py, Type pz, Type pw) { set(px,py,pz,pw); }

  // -- Construct from three Types.
  vgl_homg_point_3d (Type px, Type py, Type pz) { set(px,py,pz,1.0); }

  // -- Construct from 4-vector.
  vgl_homg_point_3d (const Type v[4]) { set(v[0],v[1],v[2],v[3]); }

  // Default destructor
  // ~vgl_homg_point_3d () {}

  // Default assignment operator
  // vgl_homg_point_3d<Type>& operator=(const vgl_homg_point_3d<Type>& that) {
  //   set(that.x(),that.y(),that.z(),that.w());
  //   return *this;
  // }

  // Data Access-------------------------------------------------------------

  inline Type x() const {return data_[0];}
  inline Type y() const {return data_[1];}
  inline Type z() const {return data_[2];}
  inline Type w() const {return data_[3];}

  // -- Set x,y,z,w
  inline void set (Type px, Type py, Type pz, Type pw) {
    data_[0] = px,
    data_[1] = py,
    data_[2] = pz,
    data_[3] = pw;
  }

  // test for point at infinity  
  // Return true when |w| < tol * min(|x|, |y|, |z|)
  bool ideal(Type tol) {
    return fabs(w()) < tol * vcl_min(vcl_min(fabs(x()),fabs(y())),fabs(z()));
  }

  // INTERNALS---------------------------------------------------------------

protected:
  // the data associated with this point 
  Type data_[4];
};

  // stream operators 
template <class Type>
ostream&  operator<<(ostream& s, const vgl_homg_point_3d<Type>& p) {
  return s << " <vgl_homg_point_3d ("
           << p->data_[0] << "," << p->data_[1] << "," 
           << p->data_[2] << "," << p->data_[3] << ") >";  
}

template <class Type>
istream&  operator>>(istream& is,  vgl_homg_point_3d<Type>& p) {
  return is >> p->data_[0] >> p->data_[1] >> p->data_[2] >> p->data_[3]; 
}

#endif




