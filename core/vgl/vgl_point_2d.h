#ifndef vgl_point_2d_h
#define vgl_point_2d_h
#ifdef __GNUC__
#pragma interface
#endif

// .NAME vgl_point_2d
// .INCLUDE vgl/vgl_point_2d.h
// .FILE vgl_point_2d.txx
// .SECTION Author
//    Don Hamilton, Peter Tu
// Created: Feb 15 2000
// .SECTION Modifications:
//  Peter Vanroose, Dec  1 2000: moved distance() to vgl_distance.h
//  Peter Vanroose, May  9 2000: implemented intersection constructor
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
  vgl_point_2d () {}

  // Default copy constructor:
  // vgl_point_2d (vgl_point_2d<Type> const& that) {
  //   this->data_[0] = that.data_[0];
  //   this->data_[1] = that.data_[1];
  // }

  vgl_point_2d<Type> (vgl_homg_point_2d<Type> const& p);

  // -- Construct a vgl_point_2d from two Types.
  vgl_point_2d (Type px, Type py) { data_[0]=px; data_[1]=py; }

  // -- Construct from 2-vector.
  vgl_point_2d (Type const v[2]) { data_[0]=v[0]; data_[1]=v[1]; }

  // -- Construct from 2 lines (intersection).
  vgl_point_2d (vgl_line_2d<Type> const& l1,
                vgl_line_2d<Type> const& l2);

  // Default destructor:
  // ~vgl_point_2d () {}

  // Default assignment operator:
  // vgl_point_2d<Type>& operator=(vgl_point_2d<Type> const& that) {
  //   this->data_[0] = that.data_[0];
  //   this->data_[1] = that.data_[1];
  // }

  
  // -- Destructor
  ~vgl_point_2d () {}
  
  // -- Assignment  

  vgl_point_2d<Type>& operator=(const vgl_point_2d<Type>& that){
    //this->data_ = that.data_;
    data_[0] = that.data_[0];
    data_[1] = that.data_[1];
    return *this;
  }

  // some convenience operators 

  vgl_point_2d<Type> operator+(const vgl_point_2d<Type>& that) const;
  vgl_point_2d<Type> operator-(const vgl_point_2d<Type>& that) const;
  
  // the == operator
  bool operator==(const vgl_point_2d &other) const;
  
  
  // Data Access-------------------------------------------------------------

  inline Type x() const {return data_[0];}
  inline Type y() const {return data_[1];}
  inline Type& x() {return data_[0];}
  inline Type& y() {return data_[1];}

  // -- Set x,y.
  inline void set (Type px, Type py){ data_[0] = px; data_[1] = py; }
  inline void set (Type const p[2]) { data_[0] = p[0]; data_[1] = p[1]; }
  inline void set_x (Type px) { data_[0] = px;}
  inline void set_y (Type py) { data_[1] = py;}

  // INTERNALS---------------------------------------------------------------

protected:
  // the data associated with this point 
  Type data_[2];
};

// stream operators

template <class Type>
ostream&  operator<<(ostream& s, const vgl_point_2d<Type>& p) {
  return s << "<vgl_point_2d " << p.x() << "," << p.y() << ">";
}

template <class Type>
istream&  operator>>(istream& is,  vgl_point_2d<Type>& p) {
  return is >> p.x() >> p.y();
}

#endif // vgl_point_2d_h
