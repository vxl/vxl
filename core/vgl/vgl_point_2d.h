#ifndef vgl_point_2d_h
#define vgl_point_2d_h
#ifdef __GNUC__
#pragma interface
#endif


// Author: Don Hamilton, Peter Tu
// Copyright:
// Created: Feb 15 2000
//: Represents a cartesian 2D point

#include <vcl/vcl_iostream.h>

template <class Type>
class vgl_point_2d {
  
  // PUBLIC INTERFACE--------------------------------------------------------
  
public:
  
  // Constructors/Initializers/Destructors-----------------------------------
  
  // -- Default constructor  
  vgl_point_2d () {}
  
  // -- Copy constructor  
  vgl_point_2d (const vgl_point_2d<Type>& that) { *this = that; }
  
  // -- Construct a vgl_point_2d from two Types.
  vgl_point_2d (Type px, Type py) { set(px,py); }
  
  // -- Construct from 2-vector.
  vgl_point_2d (const Type v[2]) { set(v[0],v[1]); }
  
  // -- Destructor
  ~vgl_point_2d () {}
  
  // -- Assignment  
  vgl_point_2d<Type>& operator=(const vgl_point_2d<Type>& that){
    this->_data = that->_data;
    return *this;
  }
  
  // Data Access-------------------------------------------------------------
  
  inline Type x() const {return _data[0];}
  inline Type y() const {return _data[1];}
  
  // -- Set x,y.
  inline void set (Type px, Type py){
    _data[0] = px;
    _data[1] = py;
  }
  
  // INTERNALS---------------------------------------------------------------
  
protected:
  // the data associated with this point 
  Type _data[2];
};

// stream operators 
template <class Type>
ostream&  operator<<(ostream& s, const vgl_point_2d<Type>& p) {
  return s << "<vgl_point_2d " << p->_data[0] << " " << p->_data[1] << ">";
}

template <class Type>
istream&  operator>>(istream& is,  vgl_point_2d<Type>& p) {
  return is >> p->_data[0] >> p->_data[1];
}

#endif




