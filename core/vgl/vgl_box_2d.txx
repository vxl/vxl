// This is core/vgl/vgl_box_2d.txx
#ifndef vgl_box_2d_txx_
#define vgl_box_2d_txx_
//:
// \file
// \brief Represents a cartesian 2D box.
// \author Don Hamilton, Peter Tu
// \date   Feb 15 2000

#include "vgl_box_2d.h"
#include <vgl/vgl_point_2d.h>
#include <vcl_iostream.h>
#include <vcl_algorithm.h>
#include <vcl_deprecated.h>
#include <vcl_cassert.h>
#include <vcl_cmath.h>

// Constructors/Destructor---------------------------------------------------

template <class Type>
vgl_box_2d<Type>::vgl_box_2d()
{
  min_pos_[0]=min_pos_[1]=(Type)1;
  max_pos_[0]=max_pos_[1]=(Type)0; // empty box
}

template <class Type>
vgl_box_2d<Type>::vgl_box_2d(const Type min_position[2],
                             const Type max_position[2])
{
  min_pos_[0]=min_position[0];
  min_pos_[1]=min_position[1];
  max_pos_[0]=max_position[0];
  max_pos_[1]=max_position[1];
}

template <class Type>
vgl_box_2d<Type>::vgl_box_2d(const vgl_point_2d<Type>& min_position,
                             const vgl_point_2d<Type>& max_position)
{
  min_pos_[0]=min_position.x();
  min_pos_[1]=min_position.y();
  max_pos_[0]=max_position.x();
  max_pos_[1]=max_position.y();
}

template <class Type>
vgl_box_2d<Type>::vgl_box_2d(Type xmin, Type xmax, Type ymin, Type ymax)
{
  min_pos_[0]=xmin;
  min_pos_[1]=ymin;
  max_pos_[0]=xmax;
  max_pos_[1]=ymax;
}

template <class Type>
vgl_box_2d<Type>::vgl_box_2d(const Type ref_point[2],
                             Type width, Type height,
//                           vgl_box_2d<Type>::point_type t) // fails for C++.NET 2003
                             enum point_type t) // enum necessary for C++.NET 2003
{
  if (t == vgl_box_2d<Type>::centre)
  {
    min_pos_[0]=Type(ref_point[0]-0.5*width);
    min_pos_[1]=Type(ref_point[1]-0.5*height);
    max_pos_[0]=Type(ref_point[0]+0.5*width);
    max_pos_[1]=Type(ref_point[1]+0.5*height);
  }
  else if (t == vgl_box_2d<Type>::min_pos)
  {
    min_pos_[0]=ref_point[0];
    min_pos_[1]=ref_point[1];
    max_pos_[0]=ref_point[0]+width;
    max_pos_[1]=ref_point[1]+height;
  }
  else if (t == vgl_box_2d<Type>::max_pos)
  {
    min_pos_[0]=ref_point[0]-width;
    min_pos_[1]=ref_point[1]-height;
    max_pos_[0]=ref_point[0];
    max_pos_[1]=ref_point[1];
  }
  else
    assert(!"point_type should be one of: centre, min_pos, max_pos");
}

template <class Type>
vgl_box_2d<Type>::vgl_box_2d(const Type minpoint[2],
                             Type width, Type height)
{
  VXL_DEPRECATED("vgl_box_2d constructor from min_pos; you should use an explicit 4th argument");
  min_pos_[0]=minpoint[0];
  min_pos_[1]=minpoint[1];
  max_pos_[0]=minpoint[0]+width;
  max_pos_[1]=minpoint[1]+height;
}

template <class Type>
vgl_box_2d<Type>::vgl_box_2d(const vgl_point_2d<Type>& ref_point,
                             Type width, Type height,
                             point_type t)
{
  if (t == vgl_box_2d<Type>::centre)
  {
    min_pos_[0]=Type(ref_point.x()-0.5*width);
    min_pos_[1]=Type(ref_point.y()-0.5*height);
    max_pos_[0]=Type(ref_point.x()+0.5*width);
    max_pos_[1]=Type(ref_point.y()+0.5*height);
  }
  else if (t == vgl_box_2d<Type>::min_pos)
  {
    min_pos_[0]=ref_point.x();
    min_pos_[1]=ref_point.y();
    max_pos_[0]=ref_point.x()+width;
    max_pos_[1]=ref_point.y()+height;
  }
  else if (t == vgl_box_2d<Type>::max_pos)
  {
    min_pos_[0]=ref_point.x()-width;
    min_pos_[1]=ref_point.y()-height;
    max_pos_[0]=ref_point.x();
    max_pos_[1]=ref_point.y();
  }
  else
    assert(!"point_type should be one of: centre, min_pos, max_pos");
}

template <class Type>
vgl_box_2d<Type>::vgl_box_2d(const vgl_point_2d<Type>& minpoint,
                             Type width, Type height)
{
  VXL_DEPRECATED("vgl_box_2d constructor from min_pos; you should use an explicit 4th argument");
  min_pos_[0]=minpoint.x();
  min_pos_[1]=minpoint.y();
  max_pos_[0]=minpoint.x()+width;
  max_pos_[1]=minpoint.y()+height;
}

template <class Type>
Type vgl_box_2d<Type>::centroid_x() const
{
  return Type(0.5*(min_pos_[0] + max_pos_[0]));
}

template <class Type>
Type vgl_box_2d<Type>::centroid_y() const
{
  return Type(0.5*(min_pos_[1] + max_pos_[1]));
}

template <class Type>
Type vgl_box_2d<Type>::width() const
{
  return (max_pos_[0] > min_pos_[0]) ? max_pos_[0] - min_pos_[0] : 0;
}

template <class Type>
Type vgl_box_2d<Type>::height() const
{
  return (max_pos_[1] > min_pos_[1]) ? max_pos_[1] - min_pos_[1] : 0;
}

template <class Type>
vgl_point_2d<Type> vgl_box_2d<Type>::min_point() const
{
  return vgl_point_2d<Type>(min_pos_[0],min_pos_[1]);
}

template <class Type>
vgl_point_2d<Type> vgl_box_2d<Type>::max_point() const
{
  return vgl_point_2d<Type>(max_pos_[0],max_pos_[1]);
}

template <class Type>
vgl_point_2d<Type> vgl_box_2d<Type>::centroid() const
{
  return vgl_point_2d<Type>(centroid_x(),centroid_y());
}

template <class Type>
void vgl_box_2d<Type>::set_centroid_x(Type cent_x)
{
  Type delta = cent_x - centroid_x();
  min_pos_[0]= min_pos_[0] + delta;
  max_pos_[0]= max_pos_[0] + delta;
}

template <class Type>
void vgl_box_2d<Type>::set_centroid_y(Type cent_y)
{
  Type delta = cent_y - centroid_y();
  min_pos_[1]= min_pos_[1] + delta;
  max_pos_[1]= max_pos_[1] + delta;
}


template <class T>
inline void set_dim(T & minv, T& maxv, T spread);


// All this code is to avoid drift in the centroid.
VCL_DEFINE_SPECIALIZATION 
inline void set_dim(int & minv, int& maxv, int spread)
{
  int sum = minv + maxv;
  sum = sum | (spread&1); //if width is odd, then make sum odd
  minv = int(vcl_floor((sum-spread)/2.0));
  maxv = minv+spread;

}

template <class T>
inline void set_dim(T & minv, T& maxv, T spread)
{
  T x = minv + maxv;
  minv = T( (x-spread)*0.5 );
  maxv = minv + spread;
}

//: Modify width, retaining centroid at current position
// For integer types, centroid might change slightly, but
// repeat calls to set_height will not cause centroid drift.
template <class Type>
void vgl_box_2d<Type>::set_width(Type width)
{
  set_dim(min_pos_[0], max_pos_[0], width);
}


//: Modify height, retaining centroid at current position
// For integer types, centroid might change slightly, but
// repeat calls to set_height will not cause centroid drift.
template <class Type>
void vgl_box_2d<Type>::set_height(Type height)
{
  set_dim(min_pos_[1], max_pos_[1], height);
}

template <class Type>
void vgl_box_2d<Type>::setmin_position(Type const min_position[2])
{
  min_pos_[0]=min_position[0];
  min_pos_[1]=min_position[1];
  if (max_pos_[0] < min_pos_[0]){
    max_pos_[0]=min_pos_[0];
  }
  if (max_pos_[1] < min_pos_[1]){
    max_pos_[1]=min_pos_[1];
  }
}

template <class Type>
void vgl_box_2d<Type>::setmax_position(Type const max_position[2])
{
  max_pos_[0]=max_position[0];
  max_pos_[1]=max_position[1];
  if (max_pos_[0] < min_pos_[0])
    min_pos_[0]=max_pos_[0];
  if (max_pos_[1] < min_pos_[1])
    min_pos_[1]=max_pos_[1];
}

template <class Type>
void vgl_box_2d<Type>::set_min_point(vgl_point_2d<Type> const& min_pt)
{
  min_pos_[0]=min_pt.x(); if (max_pos_[0]<min_pos_[0]) max_pos_[0]=min_pos_[0];
  min_pos_[1]=min_pt.y(); if (max_pos_[1]<min_pos_[1]) max_pos_[1]=min_pos_[1];
}


template <class Type>
void vgl_box_2d<Type>::set_max_point(vgl_point_2d<Type> const& max_pt)
{
  max_pos_[0]=max_pt.x(); if (max_pos_[0]<min_pos_[0]) min_pos_[0]=max_pos_[0];
  max_pos_[1]=max_pt.y(); if (max_pos_[1]<min_pos_[1]) min_pos_[1]=max_pos_[1];
}

template <class Type>
void vgl_box_2d<Type>::set_centroid(Type const centroid[2])
{
  set_centroid_x(centroid[0]);
  set_centroid_y(centroid[1]);
}

template <class Type>
void vgl_box_2d<Type>::set_centroid(vgl_point_2d<Type> const& centroid)
{
  set_centroid_x(centroid.x());
  set_centroid_y(centroid.y());
}

template <class Type>
vcl_ostream& vgl_box_2d<Type>::print(vcl_ostream& s) const
{
  if (is_empty())
    return s << "<vgl_box_2d (empty)>";
  else
    return s << "<vgl_box_2d "
             << min_pos_[0] << ',' << min_pos_[1] << " to "
             << max_pos_[0] << ',' << max_pos_[1] << '>';
}

template <class Type>
vcl_ostream& vgl_box_2d<Type>::write(vcl_ostream& s) const
{
  return s << min_pos_[0] << ' ' << min_pos_[1] << ' '
           << max_pos_[0] << ' ' << max_pos_[1] << '\n';
}

template <class Type>
vcl_istream& vgl_box_2d<Type>::read(vcl_istream& s)
{
  return s >> min_pos_[0] >> min_pos_[1]
           >> max_pos_[0] >> max_pos_[1];
}

template <class Type>
vgl_box_2d<Type> intersect(vgl_box_2d<Type> const& a, vgl_box_2d<Type> const& b)
{
  Type x0 = vcl_max(a.min_x(), b.min_x());
  Type y0 = vcl_max(a.min_y(), b.min_y());
  Type x1 = vcl_min(a.max_x(), b.max_x());
  Type y1 = vcl_min(a.max_y(), b.max_y());

  if (x1 > x0 && y1 > y0)
    return vgl_box_2d<Type> (x0, x1, y0, y1);
  else
    return vgl_box_2d<Type> (1,0,1,0);
}

//: Add a point to this box.
// Do this by possibly enlarging the box so that the point just falls within the box.
// Adding a point to an empty box makes it a size zero box only containing p.
template <class Type>
void vgl_box_2d<Type>::add(vgl_point_2d<Type> const& p)
{
  if (is_empty())
  {
    min_pos_[0] = max_pos_[0] = p.x();
    min_pos_[1] = max_pos_[1] = p.y();
  }
  else
  {
    if (p.x() > max_pos_[0]) max_pos_[0] = p.x();
    if (p.x() < min_pos_[0]) min_pos_[0] = p.x();
    if (p.y() > max_pos_[1]) max_pos_[1] = p.y();
    if (p.y() < min_pos_[1]) min_pos_[1] = p.y();
  }
}

//: Make the convex union of two boxes
// Do this by possibly enlarging this box so that the corner points of the
// given box just fall within the box.
// Adding an empty box does not change the current box.
template <class Type>
void vgl_box_2d<Type>::add(vgl_box_2d<Type> const& b)
{
  if (b.is_empty()) return;
  add(b.min_point());
  add(b.max_point());
}

//: Return true iff the point p is inside this box
template <class Type>
bool vgl_box_2d<Type>::contains(vgl_point_2d<Type> const& p) const
{
  return contains(p.x(), p.y());
}

//: Return true iff the corner points of b are inside this box
template <class Type>
bool vgl_box_2d<Type>::contains(vgl_box_2d<Type> const& b) const
{
  return
    contains(b.min_x(), b.min_y()) &&
    contains(b.max_x(), b.max_y());
}

//: Make the box empty
template <class Type>
void vgl_box_2d<Type>::empty()
{
  min_pos_[0]=min_pos_[1]=(Type)1;
  max_pos_[0]=max_pos_[1]=(Type)0;
}

//: Print to stream
template <class Type>
vcl_ostream&  operator<<(vcl_ostream& s, const vgl_box_2d<Type>& p) {
  return p.print(s);
}

//: Read from stream
template <class Type>
vcl_istream&  operator>>(vcl_istream& is,  vgl_box_2d<Type>& p) {
  return p.read(is);
}

//: Return box which bounds p1 and p2 (ie p1,p2 are any two of the corners)
template <class Type>
vgl_box_2d<Type> vgl_bounding_box_2d(const vgl_point_2d<Type>& p1,
                                     const vgl_point_2d<Type>& p2)
{
  return vgl_box_2d<Type>(vcl_min(p1.x(),p2.x()), vcl_max(p1.x(),p2.x()),
                          vcl_min(p1.y(),p2.y()), vcl_max(p1.y(),p2.y()) );
}

#undef VGL_BOX_2D_INSTANTIATE
#define VGL_BOX_2D_INSTANTIATE(Type) \
template class vgl_box_2d<Type >;\
template vcl_istream& operator>>(vcl_istream&, vgl_box_2d<Type >&);\
template vcl_ostream& operator<<(vcl_ostream&, vgl_box_2d<Type > const&);\
template vgl_box_2d<Type > intersect(vgl_box_2d<Type > const&, vgl_box_2d<Type > const&);\
template vgl_box_2d<Type > vgl_bounding_box_2d(const vgl_point_2d<Type >& p1,\
                                               const vgl_point_2d<Type >& p2)

#endif // vgl_box_2d_txx_
