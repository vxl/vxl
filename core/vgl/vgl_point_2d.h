#ifndef vgl_point_2d_h
#define vgl_point_2d_h
#ifdef __GNUC__
#pragma interface
#endif


// Author: Don Hamilton, Peter Tu
// Copyright:
// Created: Feb 15 2000
// Modifications:
//  Peter Vanroose, Feb 28 2000: lots of minor corrections

#include <vcl/vcl_iostream.h>

template <class Type>
class vgl_homg_point_2d;

template <class Type>
class vgl_line_2d;

//: Represents a cartesian 2D point
template <class Type>
class vgl_point_2d {

  // PUBLIC INTERFACE--------------------------------------------------------

public:

  // Constructors/Initializers/Destructors-----------------------------------

  // Default constructor:
  // vgl_point_2d () {}

  // Default copy constructor:
  // vgl_point_2d (vgl_point_2d<Type> const& that) {
  //   this->_data[0] = that._data[0];
  //   this->_data[1] = that._data[1];
  // }

  vgl_point_2d<Type> (vgl_homg_point_2d<Type> const& p);

  // -- Construct a vgl_point_2d from two Types.
  vgl_point_2d (Type px, Type py) { _data[0]=px; _data[1]=py; }

  // -- Construct from 2-vector.
  vgl_point_2d (Type const v[2]) { _data[0]=v[0]; _data[1]=v[1]; }

  // -- Construct from 2 lines (intersection).
  vgl_point_2d (vgl_line_2d<Type> const& l1,
                vgl_line_2d<Type> const& l2);

  // Default destructor:
  // ~vgl_point_2d () {}

  // Default assignment operator:
  // vgl_point_2d<Type>& operator=(vgl_point_2d<Type> const& that) {
  //   this->_data[0] = that._data[0];
  //   this->_data[1] = that._data[1];
  // }

  // Data Access-------------------------------------------------------------

  inline Type x() const {return _data[0];}
  inline Type y() const {return _data[1];}

  // -- Set x,y.
  inline void set (Type px, Type py){ _data[0] = px; _data[1] = py; }
  inline void set (Type const p[2]) { _data[0] = p[0]; _data[1] = p[1]; }

  // INTERNALS---------------------------------------------------------------

protected:
  // the data associated with this point
  Type _data[2];
};

// stream operators

template <class Type>
ostream&  operator<<(ostream& s, vgl_point_2d<Type> const& p) {
  return s << " <vgl_point_2d (" << p->_data[0] << "," << p->_data[1] << ") >";
}

template <class Type>
istream&  operator>>(istream& is,  vgl_point_2d<Type>& p) {
  return is >> p->_data[0] >> p->_data[1];
}

#endif
