#ifndef vgl_point_3d_h
#define vgl_point_3d_h
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
class vgl_homg_point_3d;

template <class Type>
class vgl_plane_3d;

//: Represents a cartesian 3D point
template <class Type>
class vgl_point_3d {

  // PUBLIC INTERFACE--------------------------------------------------------

public:

  // Constructors/Initializers/Destructors-----------------------------------

  // Default constructor:
  // vgl_point_3d () {}

  // Default copy constructor:
  // vgl_point_3d (vgl_point_3d<Type> const& that) {
  //   this->_data[0] = that._data[0];
  //   this->_data[1] = that._data[1];
  //   this->_data[2] = that._data[2];
  // }

  vgl_point_3d<Type> (vgl_homg_point_3d<Type> const& p);

  // -- Construct from three Types.
  vgl_point_3d (Type px, Type py, Type pz) { _data[0]=px; _data[1]=py; _data[2]=pz; }

  // -- Construct from 3-vector.
  vgl_point_3d (Type const v[3]) { _data[0]=v[0]; _data[1]=v[1]; _data[2]=v[2]; }

  // -- Construct from 3 planes (intersection).
  vgl_point_3d (vgl_plane_3d<Type> const& pl1,
                vgl_plane_3d<Type> const& pl2,
                vgl_plane_3d<Type> const& pl3);

  // Default destructor:
  // ~vgl_point_3d () {}

  // Default assignment operator:
  // vgl_point_3d<Type>& operator=(vgl_point_3d<Type> const& that) {
  //   this->_data[0] = that._data[0];
  //   this->_data[1] = that._data[1];
  //   this->_data[2] = that._data[2];
  // }

  // Data Access-------------------------------------------------------------

  inline Type x() const {return _data[0];}
  inline Type y() const {return _data[1];}
  inline Type z() const {return _data[2];}

  // -- Set x,y,z.
  inline void set (Type px, Type py, Type pz){ _data[0]=px; _data[1]=py; _data[2]=pz; }
  inline void set (Type const p[3]) { _data[0]=p[0]; _data[1]=p[1]; _data[2]=p[2]; }

  // INTERNALS---------------------------------------------------------------

protected:
  // the data associated with this point
  Type _data[3];
};

// stream operators

template <class Type>
ostream&  operator<<(ostream& s, vgl_point_3d<Type> const& p) {
  return s << " <vgl_point_3d ("
           << p->_data[0] << "," << p->_data[1] << "," << p->_data[2] << ") >";
}

template <class Type>
istream&  operator>>(istream& is,  vgl_point_3d<Type>& p) {
  return is >> p->_data[0] >> p->_data[1] >> p->_data[2];
}

#endif
