#ifndef vgl_plane_3d_h
#define vgl_plane_3d_h
#ifdef __GNUC__
#pragma interface
#endif


// Author: Don Hamilton, Peter Tu
// Copyright:
// Created: Feb 15 2000
//: Represents a euclidian 3D plane
//  the equation of the plane is (nx * x) + (ny * y) + (nz * z) + d = 0

#include <vcl/vcl_iostream.h>

template <class Type>
class vgl_point_3d;

template <class Type>
class vgl_plane_3d {
  
  // PUBLIC INTERFACE--------------------------------------------------------
  
public:
  
  // Constructors/Initializers/Destructors-----------------------------------
  
  // -- Default constructor  
  vgl_plane_3d () {}
  
  // -- Copy constructor  
  vgl_plane_3d (const vgl_plane_3d<Type>& that) { *this = that; }
  
  // -- Construct a vgl_plane_3d from four Types.
  vgl_plane_3d (Type nx, Type ny, Type nz, Type d) { set(nx,ny,nw,d); }
  
  // -- Construct from 4-vector.
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
    
  // -- Destructor
  ~vgl_plane_3d () {}
  
  // -- Assignment  
  inline vgl_plane_3d<Type>& operator=(const vgl_plane_3d<Type>& that) {
    this->_data[0] = that->_data[0];
    this->_data[1] = that->_data[1];
    this->_data[2] = that->_data[2];
    this->_data[3] = that->_data[3];
    return *this;
  }
  
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
  return s << "<vgl_plane_3d "
           << p._data[0] << " "
           << p._data[1] << " "
           << p._data[2] << " "
           << p._data[3] << " "
           << ">";
}

template <class Type>
istream&  operator>>(istream& is, vgl_plane_3d<Type>& p) {
  return is >> p._data[0] >> p._data[1] >> p._data[2] >> p._data[3];
}

#endif
