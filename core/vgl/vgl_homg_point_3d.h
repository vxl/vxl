#ifndef vgl_homg_point_3d_h_
#define vgl_homg_point_3d_h_

// This is vxl/vgl/vgl_homg_point_3d.h

//:
// \file
// \author Don HAMILTON Peter TU
//

template <class Type>
class vgl_point_3d;

#include <vcl_iostream.h>
#include <vcl_algorithm.h>
#include <vcl_cmath.h> // for vcl_abs()


//: Represents a homogenious 3D point
template <class Type>
class vgl_homg_point_3d
{
  //***************************************************************************
  // Initialization
  //***************************************************************************
public:

  //: Default constructor with (0,0,0,1)
  explicit vgl_homg_point_3d(void);

  // Default copy constructor
  // vgl_homg_point_3d (const vgl_homg_point_3d<Type>& that) {
  //   set(that.x(),that.y(),that.z(),that.w());
  // }

//unimp  vgl_homg_point_3d(vgl_point_3d<Type> const& p);

  //: Constructor from four Types
  vgl_homg_point_3d(Type px,
                    Type py,
                    Type pz,
                    Type pw)
  {
    set(px,py,pz,pw);
  }

  //: Constructor from three Types
  vgl_homg_point_3d(Type px,
                    Type py,
                    Type pz)
  {
    set(px,py,pz);
  }

  //: Construct from 4-vector.
  vgl_homg_point_3d(const Type v[4])
  {
    set(v[0],v[1],v[2],v[3]);
  }

  // Default destructor
  // ~vgl_homg_point_3d () {}

  // Default assignment operator
  // vgl_homg_point_3d<Type>& operator=(const vgl_homg_point_3d<Type>& that) {
  //   set(that.x(),that.y(),that.z(),that.w());
  //   return *this;
  // }

  //***************************************************************************
  // Data Access
  //***************************************************************************

  inline Type x() const { return data_[0]; }
  inline Type y() const { return data_[1]; }
  inline Type z() const { return data_[2]; }
  inline Type w() const { return data_[3]; } 
  inline Type& x() { return data_[0]; }
  inline Type& y() { return data_[1]; }
  inline Type& z() { return data_[2]; }
  inline Type& w() { return data_[3]; } 

  //: Set x,y,z,w
  inline void set(Type px,
                  Type py,
                  Type pz,
                  Type pw = (Type)1)
  {
    data_[0]=px;
    data_[1]=py;
    data_[2]=pz;
    data_[3]=pw;
  }

  //: the equality operator
  bool operator==(const vgl_homg_point_3d<Type> &other) const;


  //: Test for point at infinity
  // Return true when |w| < tol * min(|x|, |y|, |z|)
  bool ideal(Type tol)
  {
    return vcl_abs(w()) < tol * vcl_min(vcl_min(vcl_abs(x()),vcl_abs(y())),vcl_abs(z()));
  }

  //***************************************************************************
  // Internals
  //***************************************************************************
protected:
  // the data associated with this point
  Type data_[4];
};

//*****************************************************************************
// Stream operators
//*****************************************************************************
template <class Type>
vcl_ostream &operator<<(vcl_ostream &s,
                    const vgl_homg_point_3d<Type> &p)
{
  return s << " <vgl_homg_point_3d ("
           << p.x() << "," << p.y() << ","
           << p.z() << "," << p.w() << ") >";
}

template <class Type>
vcl_istream &operator>>(vcl_istream &is,
                    vgl_homg_point_3d<Type> &p)
{
  return is >> p.x() >> p.y() >> p.z() >> p.w();
}

#endif // #ifndef vgl_homg_point_3d_h_
