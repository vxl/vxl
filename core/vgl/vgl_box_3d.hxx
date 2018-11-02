// This is core/vgl/vgl_box_3d.hxx
#ifndef vgl_box_3d_hxx_
#define vgl_box_3d_hxx_
//:
// \file

#include <iostream>
#include <algorithm>
#include <cmath>
#include "vgl_box_3d.h"
#include <vgl/vgl_point_3d.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// Constructors/Destructor---------------------------------------------------

template <class Type>
vgl_box_3d<Type>::vgl_box_3d()
{
  min_pos_[0]=min_pos_[1]=min_pos_[2]=(Type)1;
  max_pos_[0]=max_pos_[1]=max_pos_[2]=(Type)0; // empty box
}

template <class Type>
vgl_box_3d<Type>::vgl_box_3d(Type const corner1[3],
                             Type const corner2[3])
{
  min_pos_[0]=max_pos_[0]=corner1[0];
  min_pos_[1]=max_pos_[1]=corner1[1];
  min_pos_[2]=max_pos_[2]=corner1[2];
  this->add(corner2);
}

template <class Type>
vgl_box_3d<Type>::vgl_box_3d(vgl_point_3d<Type> const& corner1,
                             vgl_point_3d<Type> const& corner2)
{
  min_pos_[0]=max_pos_[0]=corner1.x();
  min_pos_[1]=max_pos_[1]=corner1.y();
  min_pos_[2]=max_pos_[2]=corner1.z();
  this->add(corner2);
}

template <class Type>
vgl_box_3d<Type>::vgl_box_3d(Type xmin, Type ymin, Type zmin,
                             Type xmax, Type ymax, Type zmax)
{
  min_pos_[0]=max_pos_[0]=xmin;
  min_pos_[1]=max_pos_[1]=ymin;
  min_pos_[2]=max_pos_[2]=zmin;
  this->add(vgl_point_3d<Type>(xmax,ymax,zmax));
  if (xmin > xmax || ymin > ymax || zmin > zmax) this->empty();
}

template <class Type>
vgl_box_3d<Type>::vgl_box_3d(Type const ref_point[3],
                             Type w, Type h, Type d,
                             typename vgl_box_3d<Type>::point_type t)
{
  if (t == vgl_box_3d<Type>::centre)
  {
    min_pos_[0]=Type(ref_point[0]-0.5*w);
    min_pos_[1]=Type(ref_point[1]-0.5*h);
    min_pos_[2]=Type(ref_point[2]-0.5*d);
    max_pos_[0]=Type(ref_point[0]+0.5*w);
    max_pos_[1]=Type(ref_point[1]+0.5*h);
    max_pos_[2]=Type(ref_point[2]+0.5*d);
  }
  else if (t == vgl_box_3d<Type>::min_pos)
  {
    min_pos_[0]=ref_point[0];
    min_pos_[1]=ref_point[1];
    min_pos_[2]=ref_point[2];
    max_pos_[0]=ref_point[0]+w;
    max_pos_[1]=ref_point[1]+h;
    max_pos_[2]=ref_point[2]+d;
  }
  else if (t == vgl_box_3d<Type>::max_pos)
  {
    min_pos_[0]=ref_point[0]-w;
    min_pos_[1]=ref_point[1]-h;
    min_pos_[2]=ref_point[2]-d;
    max_pos_[0]=ref_point[0];
    max_pos_[1]=ref_point[1];
    max_pos_[2]=ref_point[2];
  }
  else
    assert(!"point_type should be one of: centre, min_pos, max_pos");
}

template <class Type>
vgl_box_3d<Type>::vgl_box_3d(vgl_point_3d<Type> const& ref_point,
                             Type w, Type h, Type d,
                             typename vgl_box_3d<Type>::point_type t)
{
  if (t == vgl_box_3d<Type>::centre)
  {
    min_pos_[0]=Type(ref_point.x()-0.5*w);
    min_pos_[1]=Type(ref_point.y()-0.5*h);
    min_pos_[2]=Type(ref_point.z()-0.5*d);
    max_pos_[0]=Type(ref_point.x()+0.5*w);
    max_pos_[1]=Type(ref_point.y()+0.5*h);
    max_pos_[2]=Type(ref_point.z()+0.5*d);
  }
  else if (t == vgl_box_3d<Type>::min_pos)
  {
    min_pos_[0]=ref_point.x();
    min_pos_[1]=ref_point.y();
    min_pos_[2]=ref_point.z();
    max_pos_[0]=ref_point.x()+w;
    max_pos_[1]=ref_point.y()+h;
    max_pos_[2]=ref_point.z()+d;
  }
  else if (t == vgl_box_3d<Type>::max_pos)
  {
    min_pos_[0]=ref_point.x()-w;
    min_pos_[1]=ref_point.y()-h;
    min_pos_[2]=ref_point.z()-d;
    max_pos_[0]=ref_point.x();
    max_pos_[1]=ref_point.y();
    max_pos_[2]=ref_point.z();
  }
  else
    assert(!"point_type should be one of: centre, min_pos, max_pos");
}

template <class Type>
Type vgl_box_3d<Type>::width() const
{
  return (max_pos_[0] > min_pos_[0]) ? max_pos_[0] - min_pos_[0] : 0;
}

template <class Type>
Type vgl_box_3d<Type>::height() const
{
  return (max_pos_[1] > min_pos_[1]) ? max_pos_[1] - min_pos_[1] : 0;
}

template <class Type>
Type vgl_box_3d<Type>::depth() const
{
  return (max_pos_[2] > min_pos_[2]) ? max_pos_[2] - min_pos_[2] : 0;
}

template <class Type>
vgl_point_3d<Type> vgl_box_3d<Type>::centroid() const
{
  return vgl_point_3d<Type>(centroid_x(),centroid_y(),centroid_z());
}

template <class Type>
Type vgl_box_3d<Type>::centroid_x() const
{
  assert(!is_empty());
  return Type(0.5*(min_pos_[0]+max_pos_[0]));
}

template <class Type>
Type vgl_box_3d<Type>::centroid_y() const
{
  assert(!is_empty());
  return Type(0.5*(min_pos_[1]+max_pos_[1]));
}

template <class Type>
Type vgl_box_3d<Type>::centroid_z() const
{
  assert(!is_empty());
  return Type(0.5*(min_pos_[2]+max_pos_[2]));
}

template <class Type>
void vgl_box_3d<Type>::set_centroid_x(Type cx)
{
  assert(!is_empty());
  Type delta = cx - centroid_x();
  min_pos_[0]= min_pos_[0] + delta;
  max_pos_[0]= max_pos_[0] + delta;
}

template <class Type>
void vgl_box_3d<Type>::set_centroid_y(Type cy)
{
  assert(!is_empty());
  Type delta = cy - centroid_y();
  min_pos_[1]= min_pos_[1] + delta;
  max_pos_[1]= max_pos_[1] + delta;
}

template <class Type>
void vgl_box_3d<Type>::set_centroid_z(Type cz)
{
  assert(!is_empty());
  Type delta = cz - centroid_z();
  min_pos_[2]= min_pos_[2] + delta;
  max_pos_[2]= max_pos_[2] + delta;
}

template <class T>
inline void set_dim_3d(T& minv, T& maxv, T spread);

// All this code is to avoid drift in the centroid.
template <>
inline void set_dim_3d(int& minv, int& maxv, int spread)
{
  int sum = minv + maxv;
  sum = sum | (spread&1); // if width is odd, then make sum odd
  minv = int(std::floor((sum-spread)/2.0));
  maxv = minv+spread;
}

template <class T>
inline void set_dim_3d(T& minv, T& maxv, T spread)
{
  T x = minv + maxv;
  minv = T( (x-spread)*0.5 );
  maxv = minv + spread;
}

template <class Type>
void vgl_box_3d<Type>::set_width(Type w)
{
  assert(!is_empty());
  set_dim_3d(min_pos_[0], max_pos_[0], w);
}

template <class Type>
void vgl_box_3d<Type>::set_height(Type h)
{
  assert(!is_empty());
  set_dim_3d(min_pos_[1], max_pos_[1], h);
}

template <class Type>
void vgl_box_3d<Type>::set_depth(Type d)
{
  assert(!is_empty());
  set_dim_3d(min_pos_[2], max_pos_[2], d);
}

//: Add to width and height, centroid unchanged.
// Will move each side by \p expand / 2.
template <class Type>
void vgl_box_3d<Type>::expand_about_centroid(Type expand)
{
  assert(!is_empty());
  set_dim_3d(min_pos_[0], max_pos_[0], width() + expand );
  set_dim_3d(min_pos_[1], max_pos_[1], height() + expand );
  set_dim_3d(min_pos_[2], max_pos_[2], depth() + expand );
}

//: Scale width, height and depth, centroid unchanged.
template <class Type>
void vgl_box_3d<Type>::scale_about_centroid(double s)
{
  assert(!is_empty());
  set_dim_3d(min_pos_[0], max_pos_[0], static_cast<Type>(width()*s));
  set_dim_3d(min_pos_[1], max_pos_[1], static_cast<Type>(height()*s));
  set_dim_3d(min_pos_[2], max_pos_[2], static_cast<Type>(depth()*s));
}

//: Scale width, height and depth, keeping scaled position of origin unchanged.
template <class Type>
void vgl_box_3d<Type>::scale_about_origin(double s)
{
  min_pos_[0] = static_cast<Type>(min_pos_[0] * s);
  min_pos_[1] = static_cast<Type>(min_pos_[1] * s);
  min_pos_[2] = static_cast<Type>(min_pos_[2] * s);
  max_pos_[0] = static_cast<Type>(max_pos_[0] * s);
  max_pos_[1] = static_cast<Type>(max_pos_[1] * s);
  max_pos_[2] = static_cast<Type>(max_pos_[2] * s);
}

template <class Type>
void vgl_box_3d<Type>::set_min_position(Type const min_position[3])
{
  min_pos_[0]=min_position[0];
  min_pos_[1]=min_position[1];
  min_pos_[2]=min_position[2];
  if (max_pos_[0] < min_pos_[0]) max_pos_[0]=min_pos_[0];
  if (max_pos_[1] < min_pos_[1]) max_pos_[1]=min_pos_[1];
  if (max_pos_[2] < min_pos_[2]) max_pos_[2]=min_pos_[2];
}

template <class Type>
void vgl_box_3d<Type>::set_max_position(Type const max_position[3])
{
  max_pos_[0]=max_position[0];
  max_pos_[1]=max_position[1];
  max_pos_[2]=max_position[2];
  if (max_pos_[0] < min_pos_[0]) min_pos_[0]=max_pos_[0];
  if (max_pos_[1] < min_pos_[1]) min_pos_[1]=max_pos_[1];
  if (max_pos_[2] < min_pos_[2]) min_pos_[2]=max_pos_[2];
}

template <class Type>
void vgl_box_3d<Type>::set_min_point(vgl_point_3d<Type> const& min_pt)
{
  min_pos_[0]=min_pt.x(); if (max_pos_[0]<min_pos_[0]) max_pos_[0]=min_pos_[0];
  min_pos_[1]=min_pt.y(); if (max_pos_[1]<min_pos_[1]) max_pos_[1]=min_pos_[1];
  min_pos_[2]=min_pt.z(); if (max_pos_[2]<min_pos_[2]) max_pos_[2]=min_pos_[2];
}

template <class Type>
void vgl_box_3d<Type>::set_max_point(vgl_point_3d<Type> const& max_pt)
{
  max_pos_[0]=max_pt.x(); if (max_pos_[0]<min_pos_[0]) min_pos_[0]=max_pos_[0];
  max_pos_[1]=max_pt.y(); if (max_pos_[1]<min_pos_[1]) min_pos_[1]=max_pos_[1];
  max_pos_[2]=max_pt.z(); if (max_pos_[2]<min_pos_[2]) min_pos_[2]=max_pos_[2];
}

template <class Type>
std::ostream& vgl_box_3d<Type>::print(std::ostream& s) const
{
  if (is_empty())
    return s << "<vgl_box_3d (empty)>";
  else
    return s<< "<vgl_box_3d "
            << min_pos_[0] << ',' << min_pos_[1] << ',' << min_pos_[2] << " to "
            << max_pos_[0] << ',' << max_pos_[1] << ',' << max_pos_[2] << '>';
}

template <class Type>
vgl_point_3d<Type> vgl_box_3d<Type>::min_point() const
{
  assert(!is_empty());
  return vgl_point_3d<Type>(min_pos_[0],min_pos_[1],min_pos_[2]);
}

template <class Type>
vgl_point_3d<Type> vgl_box_3d<Type>::max_point() const
{
  assert(!is_empty());
  return vgl_point_3d<Type>(max_pos_[0],max_pos_[1],max_pos_[2]);
}

template <class Type>
std::vector<vgl_point_3d<Type> > vgl_box_3d<Type>::vertices() const
{
  assert(!is_empty());
  std::vector<vgl_point_3d<Type> > vertices;
  vertices.push_back(vgl_point_3d<Type>(min_pos_[0], min_pos_[1], min_pos_[2]));
  vertices.push_back(vgl_point_3d<Type>(max_pos_[0], min_pos_[1], min_pos_[2]));
  vertices.push_back(vgl_point_3d<Type>(max_pos_[0], max_pos_[1], min_pos_[2]));
  vertices.push_back(vgl_point_3d<Type>(min_pos_[0], max_pos_[1], min_pos_[2]));
  vertices.push_back(vgl_point_3d<Type>(min_pos_[0], min_pos_[1], max_pos_[2]));
  vertices.push_back(vgl_point_3d<Type>(max_pos_[0], min_pos_[1], max_pos_[2]));
  vertices.push_back(vgl_point_3d<Type>(max_pos_[0], max_pos_[1], max_pos_[2]));
  vertices.push_back(vgl_point_3d<Type>(min_pos_[0], max_pos_[1], max_pos_[2]));
  return vertices;
}

template <class Type>
std::ostream& vgl_box_3d<Type>::write(std::ostream& s) const
{
  return s << min_pos_[0] << ' ' << min_pos_[1] << ' ' << min_pos_[2] << ' '
           << max_pos_[0] << ' ' << max_pos_[1] << ' ' << max_pos_[2] << '\n';
}

template <class Type>
std::istream& vgl_box_3d<Type>::read(std::istream& s)
{
  return s >> min_pos_[0] >> min_pos_[1] >> min_pos_[2]
           >> max_pos_[0] >> max_pos_[1] >> max_pos_[2];
}

//: Add a point to this box.
// Do this by possibly enlarging the box so that the point just falls within the box.
// Adding a point to an empty box makes it a size zero box only containing p.
template <class Type>
void vgl_box_3d<Type>::add(vgl_point_3d<Type> const& p)
{
  if (is_empty())
  {
    min_pos_[0] = max_pos_[0] = p.x();
    min_pos_[1] = max_pos_[1] = p.y();
    min_pos_[2] = max_pos_[2] = p.z();
  }
  else
  {
    if (p.x() > max_pos_[0]) max_pos_[0] = p.x();
    if (p.x() < min_pos_[0]) min_pos_[0] = p.x();
    if (p.y() > max_pos_[1]) max_pos_[1] = p.y();
    if (p.y() < min_pos_[1]) min_pos_[1] = p.y();
    if (p.z() > max_pos_[2]) max_pos_[2] = p.z();
    if (p.z() < min_pos_[2]) min_pos_[2] = p.z();
  }
}

//: Make the convex union of two boxes
// Do this by possibly enlarging this box so that the corner points of the
// given box just fall within the box.
// Adding an empty box does not change the current box.
template <class Type>
void vgl_box_3d<Type>::add(vgl_box_3d<Type> const& b)
{
  if (b.is_empty()) return;
  add(b.min_point());
  add(b.max_point());
}

//: Return true iff the point p is inside this box
template <class Type>
bool vgl_box_3d<Type>::contains(vgl_point_3d<Type> const& p) const
{
    return contains(p.x(), p.y(), p.z());
}

//: Return true iff the corner points of b are inside this box
template <class Type>
bool vgl_box_3d<Type>::contains(vgl_box_3d<Type> const& b) const
{
  return
    contains(b.min_x(), b.min_y(), b.min_z()) &&
    contains(b.max_x(), b.max_y(), b.max_z());
}

//: Make the box empty
template <class Type>
void vgl_box_3d<Type>::empty()
{
  min_pos_[0]=min_pos_[1]=min_pos_[2]=(Type)1;
  max_pos_[0]=max_pos_[1]=max_pos_[2]=(Type)0;
}

//: Write box to stream
template <class Type>
std::ostream&  operator<<(std::ostream& s, vgl_box_3d<Type> const& p)
{
  return p.print(s);
}

//: Read box from stream
template <class Type>
std::istream&  operator>>(std::istream& is,  vgl_box_3d<Type>& p)
{
  return p.read(is);
}

#undef VGL_BOX_3D_INSTANTIATE
#define VGL_BOX_3D_INSTANTIATE(Type) \
template class vgl_box_3d<Type >;\
template std::ostream& operator<<(std::ostream&, vgl_box_3d<Type > const& p);\
template std::istream& operator>>(std::istream&, vgl_box_3d<Type >& p)

#endif // vgl_box_3d_hxx_
