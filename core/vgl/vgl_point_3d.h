#ifndef vgl_point_3d_h
#define vgl_point_3d_h
#ifdef __GNUC__
#pragma interface
#endif

// Author: Don Hamilton, Peter Tu
// Copyright:
// Created: Feb 15 2000
//: Represents a cartesian 3D point

#include <vcl/vcl_iostream.h>

template <class Type>
class vgl_plane_3d;

template <class Type>
class vgl_point_3d {
  
  // PUBLIC INTERFACE--------------------------------------------------------
  
public:
  
  // Constructors/Initializers/Destructors-----------------------------------
  
  // -- Default constructor  
  vgl_point_3d () {}
  
  // -- Copy constructor  
  vgl_point_3d (const vgl_point_3d<Type>& that) { *this = that; }
  
  // -- Construct from three Types.
  vgl_point_3d (Type px, Type py, Type pz) { set(px,py,pz); }
  
  // -- Construct from 3-vector.
  vgl_point_3d (const Type v[3]) { set(v[0],v[1],v[2]); }
  
  // -- Construct from 3 planes
  vgl_point_3d (const vgl_plane_3d<Type>& pl1,
                const vgl_plane_3d<Type>& pl2,
                const vgl_plane_3d<Type>& pl3) { /* TODO */ }

  // -- Destructor
  ~vgl_point_3d () {}
  
  // -- Assignment  
  vgl_point_3d<Type>& operator=(const vgl_point_3d<Type>& that){
    this->_data = that->_data;
    return *this;
  }
  
  // Data Access-------------------------------------------------------------
  
  inline Type x() const {return _data[0];}
  inline Type y() const {return _data[1];}
  inline Type z() const {return _data[2];}
  
  // -- Set x,y.
  inline void set (Type px, Type py, Type pz){
    _data[0] = px;
    _data[1] = py;
    _data[2] = pz;
  }
  
  // INTERNALS---------------------------------------------------------------
  
protected:
  // the data associated with this point 
  
  Type _data[3];
};

// stream operators 

template <class Type>
ostream&  operator<<(ostream& s, const vgl_point_3d<Type>& p) {
  return s << "<vgl_point_3d "
           << p->_data[0] << " " << p->_data[1] << " " << p->_data[2] << ">";
}

template <class Type>
istream&  operator>>(istream& is,  vgl_point_3d<Type>& p) {
  return is >> p->_data[0] >> p->_data[1] >> p->_data[2]; 
}

#endif
