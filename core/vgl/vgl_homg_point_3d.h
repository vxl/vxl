#ifndef vgl_homg_point_3d_h
#define  vgl_homg_point_3d_h
#ifdef __GNUC__
#pragma interface
#endif

// Author: Don Hamilton, Peter Tu
// Copyright:
// Created: Feb 15 2000
//: Represents a homogeneous 3D point.

#include <vcl/vcl_iostream.h>

template <class Type>
class vgl_homg_point_3d {

  // PUBLIC INTERFACE--------------------------------------------------------  
public:
 
  // Constructors/Initializers/Destructors-----------------------------------

  // -- Default constructor  
  vgl_homg_point_3d () {}
  
  // -- Copy constructor  
  vgl_homg_point_3d (const vgl_homg_point_3d<Type>& that) { *this = that; }

  // -- Construct from three Types.
  vgl_homg_point_3d (Type px, Type py, Type pz, Type pw) { set(px,py,pw,pz); }

  // -- Construct from 4-vector.
  vgl_homg_point_3d (const Type v[4]) { set(v[0],v[1],v[2],v[3]); }

  // -- Destructor
  ~vgl_homg_point_3d () {}

  // -- Assignment  
  vgl_homg_point_3d<Type>& operator=(const vgl_homg_point_3d<Type>& that) {
    this->_data = that->_data;
    return *this;
  }

  // Data Access-------------------------------------------------------------

  inline Type x() const {return _data[0];}
  inline Type y() const {return _data[1];}
  inline Type z() const {return _data[2];}
  inline Type w() const {return _data[3];}

  // -- Set x,y,z,w
  inline void set (Type px, Type py, Type pw){
    _data[0] = px;
    _data[1] = py;
    _data[2] = pz;
    _data[3] = pw;
  }

  // test for point at infinity  
  // Return true when max(|x|, |y|, |z|) < tol * |w|  
  bool ideal(Type tol) {
    return max(max(abs(x()),abs(y())),abs(z())) < (tol * abs(w()));
  }

  // INTERNALS---------------------------------------------------------------

protected:
  // the data associated with this point 

  Type _data[4];
};

  // stream operators 
template <class Type>
ostream&  operator<<(ostream& s, const vgl_homg_point_3d<Type>& p) {
  return s << "<vgl_homg_point_3d "
           << p->_data[0] << " " << p->_data[1] << " " 
           << p->_data[2] << " " << p->_data[3] << ">";  
}

template <class Type>
istream&  operator>>(istream& is,  vgl_homg_point_3d<Type>& p) {
  return is >> p->_data[0] >> p->_data[1] >> p->_data[2] >> p->_data[3]; 
}

#endif




