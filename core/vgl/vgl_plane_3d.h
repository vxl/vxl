#ifndef vgl_plane_3d_h
#define vgl_plane_3d_h
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
#include <vgl/vgl_point_3d.h> // necessary for some inline functions

template <class Type>
class vgl_homg_plane_3d;

//: Represents a Euclidian 3D plane
//  the equation of the plane is (nx * x) + (ny * y) + (nz * z) + d = 0
template <class Type>
class vgl_plane_3d {
  
  // PUBLIC INTERFACE--------------------------------------------------------
  
public:
  
  // Constructors/Initializers/Destructors-----------------------------------
  
  // Default constructor  
  // vgl_plane_3d () {}
  
  // Default copy constructor  
  // vgl_plane_3d (const vgl_plane_3d<Type>& that) {
  //   _data[0]=that._data[0];
  //   _data[1]=that._data[1];
  //   _data[2]=that._data[2];
  //   _data[3]=that._data[3];
  // }
  
  vgl_plane_3d<Type> (vgl_homg_plane_3d<Type> const& p);

  // -- Construct a vgl_plane_3d from its equation, four Types.
  vgl_plane_3d (Type nx, Type ny, Type nz, Type d) { set(nx,ny,nz,d); }
  
  // -- Construct from its equation, a 4-vector.
  vgl_plane_3d (const Type v[4]) { set(v[0],v[1],v[2],v[3]); }
  
  // -- Construct from Normal and d
  vgl_plane_3d (const Type normal[3], Type d) {
    set(normal[0],normal[1],normal[2],d);
  }
  
  // -- Construct from Normal and a point
  vgl_plane_3d (const Type normal[3], const vgl_point_3d<Type>& p) {
    set(normal[0],normal[1],normal[2],
        -(normal[0]*p.x() + normal[1]*p.y() + normal[2]*p.z()));
  }
  
  // -- Construct from three non-collinear points
  vgl_plane_3d (vgl_point_3d<Type> const& p1,
                vgl_point_3d<Type> const& p2,
                vgl_point_3d<Type> const& p3);
    
  // Default destructor
  // ~vgl_plane_3d () {}
  
  // Default assignment operator
  // vgl_plane_3d<Type>& operator=(vgl_plane_3d<Type> const& that) {
  //   this->_data[0] = that._data[0];
  //   this->_data[1] = that._data[1];
  //   this->_data[2] = that._data[2];
  //   this->_data[3] = that._data[3];
  //   return *this;
  // }
  
  // Data Access-------------------------------------------------------------
  
  inline Type nx() const {return _data[0];}
  inline Type ny() const {return _data[1];}
  inline Type nz() const {return _data[2];}
  inline Type d()  const {return _data[3];}
  
  //: -- Set nx ny nz d
  inline void set (Type nx, Type ny, Type nz, Type d){
    _data[0] = nx;
    _data[1] = ny;
    _data[2] = nz;
    _data[3] = d;
  }
  
  // INTERNALS---------------------------------------------------------------

protected:
  // the data associated with this plane 
  Type _data[4];
};


// stream operators 

template <class Type>
inline ostream&  operator<<(ostream& s, const vgl_plane_3d<Type>& p) {
  return s << " <vgl_plane_3d "
           << p._data[0] << " x + "
           << p._data[1] << " y + "
           << p._data[2] << " z + "
           << p._data[3] << " = 0 >";
}

template <class Type>
istream&  operator>>(istream& is, vgl_plane_3d<Type>& p) {
  return is >> p._data[0] >> p._data[1] >> p._data[2] >> p._data[3];
}

#endif
