#ifndef vgl_point_3d_h
#define vgl_point_3d_h
#ifdef __GNUC__
#pragma interface
#endif

// .NAME vgl_point_3d
// .INCLUDE vgl/vgl_point_3d.h
// .FILE vgl_point_3d.txx
// .SECTION Author
//    Don Hamilton, Peter Tu
// Created: Feb 15 2000

#include <vcl/vcl_iostream.h>

template <class Type>
class vgl_plane_3d;

//: Represents a cartesian 3D point
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
                const vgl_plane_3d<Type>& pl3); /* TODO */ 

  // -- Destructor
  ~vgl_point_3d () {}
  
  // -- Assignment  
  vgl_point_3d<Type>& operator=(const vgl_point_3d<Type>& that){
    //this->data_ = that.data_;
    data_[0] = that.data_[0];
    data_[1] = that.data_[1];
    data_[2] = that.data_[2];
    return *this;
  }
  
  
  // some convenience operators 

  vgl_point_3d<Type> operator+(const vgl_point_3d<Type>& that) const;
  vgl_point_3d<Type> operator-(const vgl_point_3d<Type>& that) const;
  
  Type distance(const vgl_point_3d<Type>& that) const;

  
  // the == operator
  bool operator==(const vgl_point_3d &other) const;
  
  


  // Data Access-------------------------------------------------------------
  
  inline Type x() const {return data_[0];}
  inline Type y() const {return data_[1];}
  inline Type z() const {return data_[2];}
  
  // -- Set x,y.
  inline void set (Type px, Type py, Type pz){
    data_[0] = px;
    data_[1] = py;
    data_[2] = pz;
  }
  inline void set_x (Type px) { data_[0] = px;}
  inline void set_y (Type py) { data_[1] = py;}
  inline void set_z (Type pz) { data_[2] = pz;}


  ostream& write(ostream& s) const {
    return s << this->data_[0] << " " << this->data_[1] << " " << this->data_[2];
  }
  
  // INTERNALS---------------------------------------------------------------
  
protected:
  // the data associated with this point 
  
  Type data_[3];
};

// stream operators 

template <class Type>
ostream&  operator<<(ostream& s, const vgl_point_3d<Type>& p) {
  return s << "<vgl_point_3d "
           << p->data_[0] << " " << p->data_[1] << " " << p->data_[2] << ">";
}

template <class Type>
istream&  operator>>(istream& is,  vgl_point_3d<Type>& p) {
  return is >> p->data_[0] >> p->data_[1] >> p->data_[2]; 
}

#endif
