#ifndef vgl_point_3d_h
#define vgl_point_3d_h
#ifdef __GNUC__
#pragma interface
#endif

// This is vxl/vgl/vgl_point_3d.h

//:
// \file
// \brief a point in 3D nonhomogeneous space
// \author Don Hamilton, Peter Tu

#include <vcl_iostream.h>

template <class Type>
class vgl_plane_3d;

//: Represents a cartesian 3D point
template <class Type>
class vgl_point_3d {

  // PUBLIC INTERFACE--------------------------------------------------------

public:

  // Constructors/Initializers/Destructors-----------------------------------

  //: Default constructor
  vgl_point_3d () {}

  //: Copy constructor
  vgl_point_3d (const vgl_point_3d<Type>& that) { *this = that; }

  //: Construct from three Types.
  vgl_point_3d (Type px, Type py, Type pz) { set(px,py,pz); }

  //: Construct from 3-vector.
  vgl_point_3d (const Type v[3]) { set(v[0],v[1],v[2]); }

  //: Construct from 3 planes
  vgl_point_3d (const vgl_plane_3d<Type>& pl1,
                const vgl_plane_3d<Type>& pl2,
                const vgl_plane_3d<Type>& pl3); /* TODO */

  //: Destructor
  ~vgl_point_3d () {}

  //: Assignment
  vgl_point_3d<Type>& operator=(const vgl_point_3d<Type>& that){
    //this->data_ = that.data_;
    data_[0] = that.data_[0];
    data_[1] = that.data_[1];
    data_[2] = that.data_[2];
    return *this;
  }

  //: Return this+that
  vgl_point_3d<Type> operator+(const vgl_point_3d<Type>& that) const;
  //: Return this-that
  vgl_point_3d<Type> operator-(const vgl_point_3d<Type>& that) const;

  //: Test for equality
  bool operator==(const vgl_point_3d<Type> &other) const;

  // Data Access-------------------------------------------------------------

  inline Type x() const {return data_[0];}
  inline Type y() const {return data_[1];}
  inline Type z() const {return data_[2];}
  inline Type& x() {return data_[0];}
  inline Type& y() {return data_[1];}
  inline Type& z() {return data_[2];}

  //: Set x,y and z
  inline void set (Type px, Type py, Type pz){
    data_[0] = px;
    data_[1] = py;
    data_[2] = pz;
  }
  inline void set_x (Type px) { data_[0] = px;}
  inline void set_y (Type py) { data_[1] = py;}
  inline void set_z (Type pz) { data_[2] = pz;}

  //: Write "x y z" to stream s
  vcl_ostream& write(vcl_ostream& s) const {
    return s << x() << " " << y() << " " << z();
  }

  // INTERNALS---------------------------------------------------------------

protected:
  // the data associated with this point

  Type data_[3];
};

// stream operators

//: Write "<vgl_point_3d x,y,z>" to stream
template <class Type>
vcl_ostream&  operator<<(vcl_ostream& s, const vgl_point_3d<Type>& p) {
  return s << "<vgl_point_3d "
           << p.x() << "," << p.y() << "," << p.z() << ">";
}

//: Read x y z from stream
template <class Type>
vcl_istream&  operator>>(vcl_istream& is,  vgl_point_3d<Type>& p) {
  return is >> p.x() >> p.y() >> p.z();
}

#endif // vgl_point_3d_h
