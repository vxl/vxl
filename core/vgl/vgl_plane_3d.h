#ifndef vgl_plane_3d_h
#define vgl_plane_3d_h
#ifdef __GNUC__
#pragma interface
#endif

// This is vxl/vgl/vgl_plane_3d.h

//:
//  \file
//  \brief a plane in 3D nonhomogeneous space
//  \author Don Hamilton, Peter Tu
//  \date   Feb 15 2000

#include <vcl_iosfwd.h>
#include <vgl/vgl_fwd.h> // forward declare vgl_homg_plane_3d
#include <vgl/vgl_point_3d.h> // necessary for inline functions x(), y() and z()

//: Represents a Euclidian 3D plane
//  the equation of the plane is (nx * x) + (ny * y) + (nz * z) + d = 0
template <class Type>
class vgl_plane_3d {

  // PUBLIC INTERFACE--------------------------------------------------------

public:

  // Constructors/Initializers/Destructors-----------------------------------

#if 0
  // Default constructor
  vgl_plane_3d () {}

  // Default copy constructor
  vgl_plane_3d (const vgl_plane_3d<Type>& that) {
    data_[0]=that.data_[0];
    data_[1]=that.data_[1];
    data_[2]=that.data_[2];
    data_[3]=that.data_[3];
  }
#endif

  vgl_plane_3d<Type> (vgl_homg_plane_3d<Type> const& p);

  //: Construct a vgl_plane_3d from its equation, four Types.
  vgl_plane_3d (Type nx, Type ny, Type nz, Type d) { set(nx,ny,nz,d); }

  //: Construct from its equation, a 4-vector.
  vgl_plane_3d (const Type v[4]) { set(v[0],v[1],v[2],v[3]); }

  //: Construct from Normal and d
  vgl_plane_3d (const Type normal[3], Type d) {
    set(normal[0],normal[1],normal[2],d);
  }

  //: Construct from Normal and a point
  vgl_plane_3d (const Type normal[3], const vgl_point_3d<Type>& p) {
    set(normal[0],normal[1],normal[2],
        -(normal[0]*p.x() + normal[1]*p.y() + normal[2]*p.z()));
  }

  //: Construct from three non-collinear points
  vgl_plane_3d (vgl_point_3d<Type> const& p1,
                vgl_point_3d<Type> const& p2,
                vgl_point_3d<Type> const& p3);

#if 0
  // Default destructor
  ~vgl_plane_3d () {}

  // Default assignment operator
  vgl_plane_3d<Type>& operator=(vgl_plane_3d<Type> const& that) {
    this->data_[0] = that.data_[0];
    this->data_[1] = that.data_[1];
    this->data_[2] = that.data_[2];
    this->data_[3] = that.data_[3];
    return *this;
  }
#endif

  // Data Access-------------------------------------------------------------

  inline Type nx() const {return data_[0];}
  inline Type ny() const {return data_[1];}
  inline Type nz() const {return data_[2];}
  inline Type d()  const {return data_[3];}

  //: -- Set nx ny nz d
  inline void set (Type nx, Type ny, Type nz, Type d){
    data_[0] = nx;
    data_[1] = ny;
    data_[2] = nz;
    data_[3] = d;
  }

  // INTERNALS---------------------------------------------------------------

protected:
  // the data associated with this plane
  Type data_[4];
};


// stream operators

template <class Type>
vcl_ostream&  operator<<(vcl_ostream& s, const vgl_plane_3d<Type>& p);

template <class Type>
vcl_istream&  operator>>(vcl_istream& is, vgl_plane_3d<Type>& p);

#endif // vgl_plane_3d_h
