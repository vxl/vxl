#ifndef vgl_point_2d_h
#define vgl_point_2d_h
#ifdef __GNUC__
#pragma interface
#endif

// This is vxl/vgl/vgl_point_2d.h

//:
// \file
// \brief a point in 2D nonhomogeneous space
// \author Don Hamilton, Peter Tu

#include <vcl_iostream.h>

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

  //:Default constructor:
  vgl_point_2d () {}

  //: Construct from homogeneous point
  vgl_point_2d<Type> (vgl_homg_point_2d<Type> const& p);

  //: Construct a vgl_point_2d from two Types.
  vgl_point_2d (Type px, Type py) { data_[0]=px; data_[1]=py; }

  //: Construct from 2-vector.
  vgl_point_2d (Type const v[2]) { data_[0]=v[0]; data_[1]=v[1]; }

  //: Construct from 2 lines (intersection).
  vgl_point_2d (vgl_line_2d<Type> const& l1,
                vgl_line_2d<Type> const& l2);

  //: Destructor
  ~vgl_point_2d () {}

  //: Assignment
  vgl_point_2d<Type>& operator=(const vgl_point_2d<Type>& that){
    //this->data_ = that.data_;
    data_[0] = that.data_[0];
    data_[1] = that.data_[1];
    return *this;
  }

  //: Return this+that
  vgl_point_2d<Type> operator+(const vgl_point_2d<Type>& that) const;

  //: Return this-that
  vgl_point_2d<Type> operator-(const vgl_point_2d<Type>& that) const;

  //: Return this * d
  vgl_point_2d<Type> operator*(Type d) const;

  //: Return this/d
  vgl_point_2d<Type> operator/(Type d) const;

  //: Add that to this and return result
  vgl_point_2d<Type>& operator+=(const vgl_point_2d<Type>& that);

  //: Subtract that from this and return result
  vgl_point_2d<Type>& operator-=(const vgl_point_2d<Type>& that);

  //: Multiply this by d and return result
  vgl_point_2d<Type>& operator*=(Type d);

  //: Divide this by d and return result
  vgl_point_2d<Type>& operator/=(Type d);

  // the == operator
  bool operator==(const vgl_point_2d<Type> &other) const;
  bool operator!=(const vgl_point_2d<Type> &other) const { return ! operator==(other); }


  // Data Access-------------------------------------------------------------

  inline Type x() const {return data_[0];}
  inline Type y() const {return data_[1];}

  inline Type& x() {return data_[0];}
  inline Type& y() {return data_[1];}

  //: Set x,y.
  inline void set (Type px, Type py){ data_[0] = px; data_[1] = py; }
  //: Set x,y.
  inline void set (Type const p[2]) { data_[0] = p[0]; data_[1] = p[1]; }
  inline void set_x (Type px) { data_[0] = px;}
  inline void set_y (Type py) { data_[1] = py;}

  // INTERNALS---------------------------------------------------------------

protected:
  // the data associated with this point
  Type data_[2];
};


//: Return d * that
// This function assumes that scalar multiplication for this Type is commutative.
template <class Type>
vgl_point_2d<Type> operator*(Type d, const vgl_point_2d<Type>& that)
{
  return that*d;
}


//: Write <vgl_point_2d x,y> to stream
template <class Type>
vcl_ostream&  operator<<(vcl_ostream& s, const vgl_point_2d<Type>& p) {
  return s << "<vgl_point_2d " << p.x() << "," << p.y() << ">";
}

//: Read x y from stream
template <class Type>
vcl_istream&  operator>>(vcl_istream& is,  vgl_point_2d<Type>& p) {
  return is >> p.x() >> p.y();
}


#endif // vgl_point_2d_h
