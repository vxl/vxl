// This is core/vgl/vgl_box_2d.txx
#ifndef vgl_box_2d_txx_
#define vgl_box_2d_txx_
//:
// \file

#include "vgl_box_2d.h"
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_line_2d.h>
#include <vcl_iostream.h>
#include <vcl_algorithm.h>
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
vgl_box_2d<Type>::vgl_box_2d(Type const min_pos[2],
                             Type const max_pos[2])
{
  min_pos_[0]=max_pos_[0]=min_pos[0];
  min_pos_[1]=max_pos_[1]=min_pos[1];
  this->add(max_pos);
}

template <class Type>
vgl_box_2d<Type>::vgl_box_2d(vgl_point_2d<Type> const& min_pos,
                             vgl_point_2d<Type> const& max_pos)
{
  min_pos_[0]=max_pos_[0]=min_pos.x();
  min_pos_[1]=max_pos_[1]=min_pos.y();
  this->add(max_pos);
}

template <class Type>
vgl_box_2d<Type>::vgl_box_2d(Type xmin, Type xmax, Type ymin, Type ymax)
{
  min_pos_[0]=max_pos_[0]=xmin;
  min_pos_[1]=max_pos_[1]=ymin;
  this->add(vgl_point_2d<Type>(xmax,ymax));
}

template <class Type>
vgl_box_2d<Type>::vgl_box_2d(Type const ref_point[2],
                             Type width, Type height,
                             typename vgl_box_2d<Type>::point_type t)
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
vgl_box_2d<Type>::vgl_box_2d(vgl_point_2d<Type> const& ref_point,
                             Type width, Type height,
                             typename vgl_box_2d<Type>::point_type t)
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
Type vgl_box_2d<Type>::centroid_x() const
{
  assert(!is_empty());
  return Type(0.5*(min_pos_[0] + max_pos_[0]));
}

template <class Type>
Type vgl_box_2d<Type>::centroid_y() const
{
  assert(!is_empty());
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
  assert(!is_empty());
  return vgl_point_2d<Type>(min_pos_[0],min_pos_[1]);
}

template <class Type>
vgl_point_2d<Type> vgl_box_2d<Type>::max_point() const
{
  assert(!is_empty());
  return vgl_point_2d<Type>(max_pos_[0],max_pos_[1]);
}

template <class Type>
vgl_point_2d<Type> vgl_box_2d<Type>::centroid() const
{
  assert(!is_empty());
  return vgl_point_2d<Type>(centroid_x(),centroid_y());
}

template <class Type>
void vgl_box_2d<Type>::set_centroid_x(Type cent_x)
{
  assert(!is_empty());
  Type delta = cent_x - centroid_x();
  min_pos_[0]= min_pos_[0] + delta;
  max_pos_[0]= max_pos_[0] + delta;
}

template <class Type>
void vgl_box_2d<Type>::set_centroid_y(Type cent_y)
{
  assert(!is_empty());
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
  assert(!is_empty());
  set_dim(min_pos_[0], max_pos_[0], width);
}

//: Modify height, retaining centroid at current position
// For integer types, centroid might change slightly, but
// repeat calls to set_height will not cause centroid drift.
template <class Type>
void vgl_box_2d<Type>::set_height(Type height)
{
  assert(!is_empty());
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

  if (x1 >= x0 && y1 >= y0)
    return vgl_box_2d<Type>(x0, x1, y0, y1);
  else
    return vgl_box_2d<Type>(); // empty box
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

//: compute the intersection of an infinite line with *this box.
//  p0 and p1 are the intersection points
static bool vgl_near_zero(double x){return vcl_fabs(x)<1e-08;}
static bool vgl_near_eq(double x, double y){return vgl_near_zero(x-y);}

template <class Type>
bool vgl_box_2d<Type>::intersect(vgl_line_2d<Type>& line,
                                 vgl_point_2d<Type>& p0,
                                 vgl_point_2d<Type>& p1)
{
  double a = line.a(), b = line.b(), c = line.c();
  double xmin=this->min_x(), xmax=this->max_x();
  double ymin=this->min_y(), ymax=this->max_y();

 //Run through the cases
  //
  if (vgl_near_zero(a))// The line is y = -c/b
  {
    float y0 = -c/b;
    // The box edge is collinear with line?
    if (vgl_near_eq(ymin,y0))
    {
      p0.set(xmin, ymin);
      p1.set(xmax, ymin);
      return true;
    }
    if (vgl_near_eq(ymax,y0))
    {
      p0.set(xmin, ymax);
      p1.set(xmax, ymax);
      return true;
    }

    if ((ymin > y0) || (y0 > ymax)) // The line does not intersect the box
      return false;
    else // The line does intersect
    {
      p0.set(xmin, y0);
      p1.set(xmax, y0);
      return true;
    }
  }

  if (vgl_near_zero(b))// The line is x = -c/a
  {
    float x0 = -c/a;
    // The box edge is collinar with l?
    if (vgl_near_eq(xmin,x0))
    {
      p0.set(xmin, ymin);
      p1.set(xmin, ymax);
      return true;
    }
    if (vgl_near_eq(xmax,x0))
    {
      p0.set(xmax, ymin);
      p1.set(xmax, ymax);
      return true;
    }

    if ((xmin > x0) || (x0 > xmax)) // The line does not intersect the box
      return false;
    else // The line does intersect
    {
      p0.set(x0, ymin);
      p1.set(x0, ymax);
      return true;
    }
  }

  // The normal case with no degeneracies

// There are six possible intersection combinations:
// \verbatim
//
//                C01 /    CY     \ C11
//                   /     |       \           .
//       ymax  -----/------|--------\-----
//            |    /       |         \    |
//            |   /        |          \   |
//            |  /         |           \  | \  .
//            | /          |            \ |  \_ Bounding Box
//            |/           |             \|
//            /            |              \    .
//           /|            |              |\   .
//           ---------------------------------- CX
//          \ |            |              /
//           \|            |             /|
//            \            |            / |
//            |\           |           /  |
//            | \          |          /   |
//            |  \         |         /    |
//       xmin  ---\--------|--------/-----   xmax
//       ymin      \       |       /
//              C00 \             / C10
// \endverbatim

  // Intersection with x = xmin
  float y_xmin_int = -(c + a*xmin)/b;
  bool inside_xmin = (y_xmin_int >= ymin) && (y_xmin_int <= ymax);

  // Intersection with x = xmax
  float y_xmax_int = -(c + a*xmax)/b;
  bool inside_xmax = (y_xmax_int >= ymin) && (y_xmax_int <= ymax);

  // Intersection with y = ymin
  float x_ymin_int = -(c + b*ymin)/a;
  bool inside_ymin = (x_ymin_int >= xmin) && (x_ymin_int <= xmax);

  // Intersection with y = ymax
  float x_ymax_int = -(c + b*ymax)/a;
  bool inside_ymax = (x_ymax_int >= xmin) && (x_ymax_int <= xmax);

  // Case CX
  if (inside_xmin && inside_xmax &&
      !(vgl_near_eq(y_xmin_int,ymin) && vgl_near_eq(y_xmax_int,ymax)))
  {
    p0.set(xmin, y_xmin_int);
    p1.set(xmax, y_xmax_int);
    return true;
  }

  // Case CY
  if (inside_ymin && inside_ymax &&
      !(vgl_near_eq(x_ymin_int,xmin) && vgl_near_eq(x_ymax_int,xmax)))
  {
    p0.set(x_ymin_int, ymin);
    p1.set(x_ymax_int, ymax);
    return true;
  }

  // Case C00
  if (inside_xmin && inside_ymin &&
      !(inside_xmax && inside_ymax))
  {
    p0.set(xmin, y_xmin_int);
    p1.set(x_ymin_int, ymin);
    return true;
  }

  // Case C01
  if (inside_xmin && inside_ymax &&
      !(inside_xmax && inside_ymin))
  {
    p0.set(xmin, y_xmin_int);
    p1.set(x_ymax_int, ymax);
    return true;
  }

  // Case C10
  if (inside_ymin && inside_xmax &&
      !(inside_xmin && inside_ymax))
  {
    p0.set(x_ymin_int, ymin);
    p1.set(xmax, y_xmax_int);
    return true;
  }

  // Case C11
  if (inside_ymax && inside_xmax &&
      !(inside_xmin && inside_ymin))
  {
    p0.set(x_ymax_int, ymax);
    p1.set(xmax, y_xmax_int);
    return true;
  }
  //Exactly p0ssing through diagonal of BB
  if (inside_xmin && inside_xmax && inside_ymin && inside_ymax)
  {
    if (a>0) // 45 degrees
    {
      p0.set(xmin, ymin);
      p1.set(xmax, ymax);
      return true;
    }
    else // 135 degrees
    {
      p0.set(xmin, ymax);
      p1.set(xmax, ymin);
      return true;
    }
  }
  return false;
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
vcl_ostream& operator<<(vcl_ostream& s, vgl_box_2d<Type> const& p) {
  return p.print(s);
}

//: Read from stream
template <class Type>
vcl_istream& operator>>(vcl_istream& is, vgl_box_2d<Type>& p) {
  return p.read(is);
}

#undef VGL_BOX_2D_INSTANTIATE
#define VGL_BOX_2D_INSTANTIATE(Type) \
template class vgl_box_2d<Type >;\
template vcl_istream& operator>>(vcl_istream&, vgl_box_2d<Type >&);\
template vcl_ostream& operator<<(vcl_ostream&, vgl_box_2d<Type > const&);\
template vgl_box_2d<Type > intersect(vgl_box_2d<Type > const&, vgl_box_2d<Type > const&);\
template vgl_box_2d<Type > vgl_bounding_box_2d(vgl_point_2d<Type > const& p1,\
                                               vgl_point_2d<Type > const& p2)

#endif // vgl_box_2d_txx_
