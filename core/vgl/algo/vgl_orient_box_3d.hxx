// This is core/vgl/algo/vgl_orient_box_3d.hxx
#ifndef vgl_orient_box_3d_hxx_
#define vgl_orient_box_3d_hxx_
//:
// \file

#include <iostream>
#include "vgl_orient_box_3d.h"
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_tolerance.h>
#include <vnl/vnl_det.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: constructor from four corner points.
//  The three directions from the first of these to the three other points must be mutually orthogonal.
template <class Type>
vgl_orient_box_3d<Type>::vgl_orient_box_3d(vgl_point_3d<Type> const& p0,
                                           vgl_point_3d<Type> const& px,
                                           vgl_point_3d<Type> const& py,
                                           vgl_point_3d<Type> const& pz)
{
  // The normalized main axes of the box:
  vgl_vector_3d<Type> vx = px - p0, vy = py - p0, vz = pz - p0;
  assert(vx.x() * vy.x() + vx.y() * vy.y() + vx.z() * vy.z() < 0.001  );
  assert(vz.x() * vy.x() + vz.y() * vy.y() + vz.z() * vy.z() < 0.001  );

  double lx = vx.length(), ly = vy.length(), lz = vz.length();
  // the quaternion rotation matrix:
  vnl_matrix_fixed<double,3,3> rotation;
  // first row: coordinates of first orthonormal basis vector (etc):
  rotation(0,0) = vx.x()/lx; rotation(0,1) = vx.y()/lx; rotation(0,2) = vx.z()/lx;
  rotation(1,0) = vy.x()/ly; rotation(1,1) = vy.y()/ly; rotation(1,2) = vy.z()/ly;
  rotation(2,0) = vz.x()/lz; rotation(2,1) = vz.y()/lz; rotation(2,2) = vz.z()/lz;
  // If the transformation matrix has negative determinant, invert px and py:
  double det = vnl_det(rotation);
  if (det < 0) {
    double t = rotation(0,0); rotation(0,0) = rotation(1,0); rotation(1,0) = t;
           t = rotation(0,1); rotation(0,1) = rotation(1,1); rotation(1,1) = t;
           t = rotation(0,2); rotation(0,2) = rotation(1,2); rotation(1,2) = t;
  }
  orient_ = vnl_quaternion<double>(rotation);

  // And finally the vgl_box_3d: it is the inversely rotated version of the given 4 points around the centroid:
  vnl_vector_fixed<double,3> centroid(-0.5*p0.x()+0.5*px.x()+0.5*py.x()+0.5*pz.x(),
                                      -0.5*p0.y()+0.5*px.y()+0.5*py.y()+0.5*pz.y(),
                                      -0.5*p0.z()+0.5*px.z()+0.5*py.z()+0.5*pz.z());
  vnl_vector_fixed<double,3> p0_to_centroid = vnl_vector_fixed<double,3>(p0.x(),p0.y(),p0.z()) - centroid;
  vnl_vector_fixed<double,3> corner_0 = centroid + rotation * p0_to_centroid;
  vgl_point_3d<Type> corner0((Type)(corner_0[0]), (Type)(corner_0[1]), (Type)(corner_0[2])); // explicitly cast back from double
  vnl_vector_fixed<double,3> corner_1 = centroid - rotation * p0_to_centroid;
  vgl_point_3d<Type> corner1((Type)(corner_1[0]), (Type)(corner_1[1]), (Type)(corner_1[2])); // explicitly cast back from double
  box_ = vgl_box_3d<Type>(corner0, corner1);
}

//: returns the 8 corner points of the box
template <class Type>
std::vector<vgl_point_3d<Type> > vgl_orient_box_3d<Type>::corners() const
{
  std::vector<vgl_point_3d<Type> > corner_points(8);

  //get the min and max of the aab and find the other corners
  corner_points[0] = box_.min_point();
  corner_points[7] = box_.max_point();

  corner_points[1] = vgl_point_3d<Type> (corner_points[0].x()+width(),
                                         corner_points[0].y(),
                                         corner_points[0].z());
  corner_points[2] = vgl_point_3d<Type> (corner_points[0].x(),
                                         corner_points[0].y(),
                                         corner_points[0].z()+depth());
  corner_points[3] = vgl_point_3d<Type> (corner_points[1].x(),
                                         corner_points[1].y(),
                                         corner_points[1].z()+depth());
  corner_points[4] = vgl_point_3d<Type> (corner_points[0].x(),
                                         corner_points[0].y()+height(),
                                         corner_points[0].z());
  corner_points[5] = vgl_point_3d<Type> (corner_points[1].x(),
                                         corner_points[1].y()+height(),
                                         corner_points[1].z());
  corner_points[6] = vgl_point_3d<Type> (corner_points[2].x(),
                                         corner_points[2].y()+height(),
                                         corner_points[2].z());
  // rotate the corner points
  for (unsigned int i=0; i < corner_points.size(); i++) {
    vnl_vector_fixed<double,3> p;
    p[0] = corner_points[i].x() - box_.centroid_x();
    p[1] = corner_points[i].y() - box_.centroid_y();
    p[2] = corner_points[i].z() - box_.centroid_z();
    p = orient_.rotate(p);
    corner_points[i] = vgl_point_3d<Type> (Type(p[0]) + box_.centroid_x(),
                                           Type(p[1]) + box_.centroid_y(),
                                           Type(p[2]) + box_.centroid_z());
  }
  return corner_points;
}

template <class Type>
vgl_box_3d<Type> vgl_orient_box_3d<Type>::enclosing_box() const{
  std::vector<vgl_point_3d<Type> > crns = this->corners();
  vgl_box_3d<Type> ret;
  for(typename std::vector<vgl_point_3d<Type> >::iterator cit = crns.begin();
      cit != crns.end(); ++cit)
    ret.add(*cit);
  return ret;
}

//: Return true if \a (x,y,z) is inside this box
template <class Type>
bool vgl_orient_box_3d<Type>::contains(Type const& x,
                                       Type const& y,
                                       Type const& z) const
{
  // first transform the point to the coordinate system of AABB
  vnl_quaternion<double> reverse_rot = orient_.inverse();

  vnl_vector_fixed<double,3> p;
  p[0] = x - box_.centroid_x();
  p[1] = y - box_.centroid_y();
  p[2] = z - box_.centroid_z();
  p = reverse_rot.rotate(p);
  Type xt = static_cast<Type>(p[0] + box_.centroid_x());
  Type yt = static_cast<Type>(p[1] + box_.centroid_y());
  Type zt = static_cast<Type>(p[2] + box_.centroid_z());
  return box_.contains(xt, yt, zt);
}

template <class Type>
std::ostream& vgl_orient_box_3d<Type>::print(std::ostream& s) const
{
  return s <<  "<vgl_orient_box_3d " << box_ << " dir=" << orient_  << '>' << std::endl;
}

template <class Type>
std::istream& vgl_orient_box_3d<Type>::read(std::istream& s)
{
  return s >> box_ >> orient_ ;
}

//: Write box to stream
template <class Type>
std::ostream&  operator<<(std::ostream& s, vgl_orient_box_3d<Type> const& p)
{
  return p.print(s);
}

//: Read box from stream
template <class Type>
std::istream&  operator>>(std::istream& is,  vgl_orient_box_3d<Type>& p)
{
  return p.read(is);
}

#undef VGL_ORIENT_BOX_3D_INSTANTIATE
#define VGL_ORIENT_BOX_3D_INSTANTIATE(T) \
template class vgl_orient_box_3d<T >;\
template std::ostream& operator<<(std::ostream&, vgl_orient_box_3d<T > const& p);\
template std::istream& operator>>(std::istream&, vgl_orient_box_3d<T >& p)

#endif // vgl_orient_box_3d_hxx_
