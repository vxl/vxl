#ifndef vgl_homg_plane_3d_h
#define vgl_homg_plane_3d_h
#ifdef __GNUC__
#pragma interface
#endif

// Author: Don Hamilton, Peter Tu
// Copyright:
// Created: Feb 15 2000
//: Represents a homogeneous 3D plane

#include <vcl/vcl_iostream.h>

template <class Type>
class vgl_homg_point_3d;

template <class Type>
class vgl_homg_plane_3d {
  
  // PUBLIC INTERFACE--------------------------------------------------------
  
public:
  
  // Constructors/Initializers/Destructors-----------------------------------
  
  // -- Default constructor  
  vgl_homg_plane_3d () {}
  
  // -- Copy constructor  
  vgl_homg_plane_3d (const vgl_homg_plane_3d<Type>& that) { *this = that; }
  
  // -- Construct from four Types.
  vgl_homg_plane_3d (Type nx, Type ny, Type nz, Type d) { set(nx,ny,nw,d); }
  
  // -- Construct from 4-vector.
  vgl_homg_plane_3d (const Type v[4]) { set(v[0],v[1],v[2],v[3]); }
  
  // -- Construct from Normal and d
  vgl_homg_plane_3d (const Type normal[3], Type d) {
    set(normal[0],normal[1],normal[2],d);
  }

  // -- Construct from Normal and a point
  vgl_homg_plane_3d (const Type normal[3], const vgl_homg_point_3d<Type>& p);

    
  // -- Destructor
  ~vgl_homg_plane_3d () {}
  
  // -- Assignment  
  vgl_homg_plane_3d<Type>& operator=(const vgl_homg_plane_3d<Type>& that){
    this->data_ = that->data_;
    return *this;
  }
  
  // Data Access-------------------------------------------------------------
  
  inline Type nx() const {return data_[0];}
  inline Type ny() const {return data_[1];}
  inline Type nz() const {return data_[2];}
  inline Type d() const {return data_[3];}

  // iterators. suggest spinning off a vgl_homg4 base class for mixing
  // into homg_plane and homg_point. fsm.
  typedef Type *iterator;
  typedef Type *const_iterator;
  iterator begin() { return data_; }
  iterator end() { return data_+4; }
  const_iterator begin() const { return data_; }
  const_iterator end() const { return data_+4; }
  
  // -- Set nx ny nz d
  inline void set (Type nx, Type ny, Type nz, Type d){
    data_[0] = nx;
    data_[1] = ny;
    data_[2] = nz;
    data_[3] = d;
  }

  // test for plane at infinity
  bool ideal(Type tol) { /* TODO */ }
  
  // INTERNALS---------------------------------------------------------------

protected:
  // the data associated with this point 

  Type data_[4];
};

// stream operators 

template <class Type>
inline ostream&  operator<<(ostream& s, const vgl_homg_plane_3d<Type>& p) {
  return s << "<vgl_homg_plane_3d "
           << p.data_[0] << " "
           << p.data_[1] << " "
           << p.data_[2] << " "
           << p.data_[3] << " "
           << ">";
}

template <class Type>
istream&  operator>>(istream& is, vgl_homg_plane_3d<Type>& p) {
  return is >> p.data_[0] >> p.data_[1] >> p.data_[2] >> p.data_[3];
}

#endif
