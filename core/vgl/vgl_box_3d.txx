// This is core/vgl/vgl_box_3d.txx
#ifndef vgl_box_3d_txx_
#define vgl_box_3d_txx_
//:
// \file

#include "vgl_box_3d.h"
#include <vgl/vgl_point_3d.h>
#include <vcl_iostream.h>
#include <vcl_deprecated.h>
#include <vcl_cassert.h>
#include <vcl_cmath.h>

// Constructors/Destructor---------------------------------------------------

template <class Type>
vgl_box_3d<Type>::vgl_box_3d()
{
  min_pos_[0]=min_pos_[1]=min_pos_[2]=(Type)1;
  max_pos_[0]=max_pos_[1]=max_pos_[2]=(Type)0; // empty box
}

template <class Type>
vgl_box_3d<Type>::vgl_box_3d(Type const min_position[3],
                             Type const max_position[3] )
{
  min_pos_[0]=min_position[0];
  min_pos_[1]=min_position[1];
  min_pos_[2]=min_position[2];

  max_pos_[0]=max_position[0];
  max_pos_[1]=max_position[1];
  max_pos_[2]=max_position[2];
}

template <class Type>
vgl_box_3d<Type>::vgl_box_3d(vgl_point_3d<Type> const& min_pos,
                             vgl_point_3d<Type> const& max_pos)
{
  min_pos_[0]=min_pos.x();
  min_pos_[1]=min_pos.y();
  min_pos_[2]=min_pos.z();

  max_pos_[0]=max_pos.x();
  max_pos_[1]=max_pos.y();
  max_pos_[2]=max_pos.z();
}

template <class Type>
vgl_box_3d<Type>::vgl_box_3d(Type xmin, Type ymin, Type zmin,
                             Type xmax, Type ymax, Type zmax)
{
  min_pos_[0]=xmin;
  min_pos_[1]=ymin;
  min_pos_[2]=zmin;

  max_pos_[0]=xmax;
  max_pos_[1]=ymax;
  max_pos_[2]=zmax;
}

template <class Type>
vgl_box_3d<Type>::vgl_box_3d(const Type ref_point[3],
                             Type width, Type height, Type depth,
//                             vgl_box_3d<Type>::point_type t) // fails for C++.NET 2003
                             enum point_type t)  // necessary for C++.NET 2003
{
  if (t == vgl_box_3d<Type>::centre)
  {
    min_pos_[0]=Type(ref_point[0]-0.5*width);
    min_pos_[1]=Type(ref_point[1]-0.5*height);
    min_pos_[2]=Type(ref_point[2]-0.5*depth);
    max_pos_[0]=Type(ref_point[0]+0.5*width);
    max_pos_[1]=Type(ref_point[1]+0.5*height);
    max_pos_[2]=Type(ref_point[2]+0.5*depth);
  }
  else if (t == vgl_box_3d<Type>::min_pos)
  {
    min_pos_[0]=ref_point[0];
    min_pos_[1]=ref_point[1];
    min_pos_[2]=ref_point[2];
    max_pos_[0]=ref_point[0]+width;
    max_pos_[1]=ref_point[1]+height;
    max_pos_[2]=ref_point[2]+depth;
  }
  else if (t == vgl_box_3d<Type>::max_pos)
  {
    min_pos_[0]=ref_point[0]-width;
    min_pos_[1]=ref_point[1]-height;
    min_pos_[2]=ref_point[2]-depth;
    max_pos_[0]=ref_point[0];
    max_pos_[1]=ref_point[1];
    max_pos_[2]=ref_point[2];
  }
  else
    assert(!"point_type should be one of: centre, min_pos, max_pos");
}

template <class Type>
vgl_box_3d<Type>::vgl_box_3d(const Type centroid[3],
                             Type width, Type height, Type depth)
{
  VXL_DEPRECATED("vgl_box_3d constructor from centroid; you should use an explicit 5th argument");
  min_pos_[0]=Type(centroid[0]-0.5*width);
  min_pos_[1]=Type(centroid[1]-0.5*height);
  min_pos_[2]=Type(centroid[2]-0.5*depth);
  max_pos_[0]=Type(centroid[0]+0.5*width);
  max_pos_[1]=Type(centroid[1]+0.5*height);
  max_pos_[2]=Type(centroid[2]+0.5*depth);
}

template <class Type>
vgl_box_3d<Type>::vgl_box_3d(const vgl_point_3d<Type>& ref_point,
                             Type width, Type height, Type depth,
#if 1
                             typename vgl_box_3d<Type>::point_type t)
#else
                             point_type t)
#endif                             
{
  if (t == vgl_box_3d<Type>::centre)
  {
    min_pos_[0]=Type(ref_point.x()-0.5*width);
    min_pos_[1]=Type(ref_point.y()-0.5*height);
    min_pos_[2]=Type(ref_point.z()-0.5*depth);
    max_pos_[0]=Type(ref_point.x()+0.5*width);
    max_pos_[1]=Type(ref_point.y()+0.5*height);
    max_pos_[2]=Type(ref_point.z()+0.5*depth);
  }
  else if (t == vgl_box_3d<Type>::min_pos)
  {
    min_pos_[0]=ref_point.x();
    min_pos_[1]=ref_point.y();
    min_pos_[2]=ref_point.z();
    max_pos_[0]=ref_point.x()+width;
    max_pos_[1]=ref_point.y()+height;
    max_pos_[2]=ref_point.z()+depth;
  }
  else if (t == vgl_box_3d<Type>::max_pos)
  {
    min_pos_[0]=ref_point.x()-width;
    min_pos_[1]=ref_point.y()-height;
    min_pos_[2]=ref_point.z()-depth;
    max_pos_[0]=ref_point.x();
    max_pos_[1]=ref_point.y();
    max_pos_[2]=ref_point.z();
  }
  else
    assert(!"point_type should be one of: centre, min_pos, max_pos");
}

template <class Type>
vgl_box_3d<Type>::vgl_box_3d(const vgl_point_3d<Type>& centroid,
                             Type width, Type height, Type depth)
{
  VXL_DEPRECATED("vgl_box_3d constructor from centroid; you should use an explicit 5th argument");
  min_pos_[0]=Type(centroid.x()-0.5*width);
  min_pos_[1]=Type(centroid.y()-0.5*height);
  min_pos_[2]=Type(centroid.z()-0.5*depth);
  max_pos_[0]=Type(centroid.x()+0.5*width);
  max_pos_[1]=Type(centroid.y()+0.5*height);
  max_pos_[2]=Type(centroid.z()+0.5*depth);
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
void vgl_box_3d<Type>::set_centroid_x(Type cx)
{
  Type delta = cx - centroid_x();
  min_pos_[0]= min_pos_[0] + delta;
  max_pos_[0]= max_pos_[0] + delta;
}

template <class Type>
void vgl_box_3d<Type>::set_centroid_y(Type cy)
{
  Type delta = cy - centroid_y();
  min_pos_[1]= min_pos_[1] + delta;
  max_pos_[1]= max_pos_[1] + delta;
}

template <class Type>
void vgl_box_3d<Type>::set_centroid_z(Type cz)
{
  Type delta = cz - centroid_z();
  min_pos_[2]= min_pos_[2] + delta;
  max_pos_[2]= max_pos_[2] + delta;
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



template <class Type>
void vgl_box_3d<Type>::set_width(const Type width)
{
  set_dim(min_pos_[0], max_pos_[0], width);
}

template <class Type>
void vgl_box_3d<Type>::set_height(const Type height)
{
  set_dim(min_pos_[1], max_pos_[1], height);
}

template <class Type>
void vgl_box_3d<Type>::set_depth(const Type depth)
{
  set_dim(min_pos_[2], max_pos_[2], depth);
}

template <class Type>
void vgl_box_3d<Type>::set_min_point(vgl_point_3d<Type> const& min_pt)
{
  min_pos_[0]=min_pt.x();
  min_pos_[1]=min_pt.y();
  min_pos_[2]=min_pt.z();

  if (max_pos_[0] < min_pos_[0])
    max_pos_[0]=min_pos_[0];
  if (max_pos_[1] < min_pos_[1])
    max_pos_[1]=min_pos_[1];
  if (max_pos_[2] < min_pos_[2])
    max_pos_[2]=min_pos_[2];
}

template <class Type>
void vgl_box_3d<Type>::set_max_point(vgl_point_3d<Type> const& max_pt)
{
  max_pos_[0]=max_pt.x();
  max_pos_[1]=max_pt.y();
  max_pos_[2]=max_pt.z();

  if (max_pos_[0] < min_pos_[0])
    min_pos_[0]=max_pos_[0];
  if (max_pos_[1] < min_pos_[1])
    min_pos_[1]=max_pos_[1];
  if (max_pos_[2] < min_pos_[2])
    min_pos_[2]=max_pos_[2];
}

template <class Type>
void vgl_box_3d<Type>::set_centroid(vgl_point_3d<Type> const& centroid)
{
  set_centroid_x(centroid.x());
  set_centroid_y(centroid.y());
  set_centroid_z(centroid.z());
}

template <class Type>
vcl_ostream& vgl_box_3d<Type>::print(vcl_ostream& s) const
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
  return vgl_point_3d<Type>(min_pos_[0],min_pos_[1],min_pos_[2]);
}

template <class Type>
vgl_point_3d<Type> vgl_box_3d<Type>::max_point() const
{
  return vgl_point_3d<Type>(max_pos_[0],max_pos_[1],max_pos_[2]);
}

template <class Type>
vcl_ostream& vgl_box_3d<Type>::write(vcl_ostream& s) const
{
  return s << min_pos_[0] << ' ' << min_pos_[1] << ' ' << min_pos_[2] << ' '
           << max_pos_[0] << ' ' << max_pos_[1] << ' ' << max_pos_[2] << '\n';
}

template <class Type>
vcl_istream& vgl_box_3d<Type>::read(vcl_istream& s)
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
vcl_ostream&  operator<<(vcl_ostream& s, vgl_box_3d<Type> const& p)
{
  return p.print(s);
}

//: Read box from stream
template <class Type>
vcl_istream&  operator>>(vcl_istream& is,  vgl_box_3d<Type>& p)
{
  return p.read(is);
}

#undef VGL_BOX_3D_INSTANTIATE
#define VGL_BOX_3D_INSTANTIATE(Type) \
template class vgl_box_3d<Type >;\
template vcl_ostream& operator<<(vcl_ostream& s, vgl_box_3d<Type > const& p);\
template vcl_istream& operator>>(vcl_istream& is,  vgl_box_3d<Type >& p)

#endif // vgl_box_3d_txx_
