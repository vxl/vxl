// This is core/vgl/vgl_orient_box_3d.txx
#ifndef vgl_orient_box_3d_txx_
#define vgl_orient_box_3d_txx_
//:
// \file

#include "vgl_orient_box_3d.h"
#include <vcl_iostream.h>

template <class Type>
vgl_orient_box_3d<Type>::vgl_orient_box_3d(vgl_box_3d<Type> box)
: box_(box)
{
  vnl_vector<double> axis(3);
  axis[0] = 0.0; axis[1] = 0.0; axis[2]=1.0;
  double angle = 0.0;

  orient_ = vnl_quaternion<double> (axis, angle);
}

template <class Type>
vgl_orient_box_3d<Type>::vgl_orient_box_3d(vgl_box_3d<Type> box, vnl_quaternion<double> orient)
: box_(box), orient_(orient)
{
}

//: returns the 8 corner points of the box
template <class Type>
vcl_vector<vgl_point_3d<Type> > vgl_orient_box_3d<Type>::corners()
{
  vcl_vector<vgl_point_3d<Type> > corner_points(8);

  //get the min and max of the aab and find the other corners
  vgl_point_3d<Type> min = box_.min_point();
  corner_points[0] = min;

  vgl_point_3d<Type> max = box_.max_point();
  corner_points[7] = max;

  corner_points[1] = vgl_point_3d<Type> (width()+min.x(), min.y(), min.z());
  corner_points[2] = vgl_point_3d<Type> (min.x(), min.y(), min.z()+depth());
  corner_points[3] = vgl_point_3d<Type> (corner_points[1].x(), corner_points[1].y(), corner_points[1].z()+depth());
  corner_points[4] = vgl_point_3d<Type> (min.x(), min.y()+height(), min.z());
  corner_points[5] = vgl_point_3d<Type> (corner_points[1].x(), corner_points[1].y()+height(), corner_points[1].z());
  corner_points[6] = vgl_point_3d<Type> (corner_points[2].x(), corner_points[2].y()+height(), corner_points[2].z());

  // rotate the corner points
  for (unsigned int i=0; i < corner_points.size(); i++) {
    vnl_vector<double> p(3);
    p[0] = corner_points[i].x() - box_.centroid_x(); 
    p[1] = corner_points[i].y() - box_.centroid_y();
    p[2] = corner_points[i].z() - box_.centroid_z();;
    p = orient_.rotate(p);
    corner_points[i] = vgl_point_3d<Type> (Type(p[0]) + box_.centroid_x(), 
      Type(p[1]) + box_.centroid_y(), Type(p[2])+ box_.centroid_z());
  }
  return corner_points;
}

//: Return true if \a (x,y,z) is inside this box
template <class Type>
bool vgl_orient_box_3d<Type>::contains(Type const& x,
                                       Type const& y,
                                       Type const& z) const
{
  // first transform the point to the coordinate system of AABB
  vnl_quaternion<double> reverse_rot(orient_.axis(), -1.*orient_.angle());

  vnl_vector<double> p(3);
  p[0] = x - box_.centroid_x(); 
  p[1] = y - box_.centroid_y(); 
  p[2] = z - box_.centroid_z();
  vnl_vector<double> p_transf = reverse_rot.rotate(p);
  return box_.contains(p_transf[0] + box_.centroid_x() , 
  p_transf[1] + box_.centroid_y(), p_transf[2] + box_.centroid_z());
}

template <class Type>
vcl_ostream& vgl_orient_box_3d<Type>::print(vcl_ostream& s) const
{
  return s <<  "<vgl_orient_box_3d " << box_ << " dir=" << orient_  << '>' << vcl_endl;
}

template <class Type>
vcl_istream& vgl_orient_box_3d<Type>::read(vcl_istream& s)
{
  return s >> box_ >> orient_ ;
}

//: Write box to stream
template <class Type>
vcl_ostream&  operator<<(vcl_ostream& s, vgl_orient_box_3d<Type> const& p)
{
  return p.print(s);
}

//: Read box from stream
template <class Type>
vcl_istream&  operator>>(vcl_istream& is,  vgl_orient_box_3d<Type>& p)
{
  return p.read(is);
}

#undef VGL_ORIENT_BOX_3D_INSTANTIATE
#define VGL_ORIENT_BOX_3D_INSTANTIATE(T) \
template class vgl_orient_box_3d<T >;\
template vcl_ostream& operator<<(vcl_ostream&, vgl_orient_box_3d<T > const& p);\
template vcl_istream& operator>>(vcl_istream&, vgl_orient_box_3d<T >& p)

#endif // vgl_orient_box_3d_txx_
