// This is vxl/vgl/vgl_homg_line_3d_2_points.txx
#ifndef vgl_homg_line_3d_2_points_txx_
#define vgl_homg_line_3d_2_points_txx_

#include "vgl_homg_line_3d_2_points.h"

//***************************************************************************
// Initialization
//***************************************************************************


//: Construct from two points. Initialise using the specified distinct
//       points on the line.
template <class Type>
vgl_homg_line_3d_2_points<Type>::vgl_homg_line_3d_2_points(vgl_homg_point_3d<Type> const& start,
                                                           vgl_homg_point_3d<Type> const& end)
{
  bool start_finite = start.w() != 0;
  bool end_finite = end.w() != 0;

  if (start_finite && end_finite) {
    point_finite_ = start;

    Type dx = end.x() - start.x();
    Type dy = end.y() - start.y();
    Type dz = end.z() - start.z();

    point_infinite_.set(dx,dy,dz, 0.0);
  }
  else if (end_finite) {
    // Start infinite
    point_finite_ = end;
    point_infinite_ = start;
  }
  else { // End infinite
    point_finite_ = start; // possibly also infinite
    point_infinite_ = end;
  }
}

//***************************************************************************
// Utility methods
//***************************************************************************

template <class Type>
bool vgl_homg_line_3d_2_points<Type>::operator==(vgl_homg_line_3d_2_points<Type> const& other) const
{
  if (this==&other)
    return true;
  force_point2_infinite(); other.force_point2_infinite();
  if (get_point_infinite() != other.get_point_infinite())
    return false;
  // Now it suffices to check that the three points are collinear:
  vgl_homg_point_3d<Type> const& p1 = get_point_infinite();
  vgl_homg_point_3d<Type> const& p2 = get_point_finite();
  vgl_homg_point_3d<Type> const& p3 = other.get_point_finite();
  Type d1x = p1.x()*p2.w()*p3.w()-p1.w()*p2.x()*p3.w();
  Type d2x = p1.x()*p2.w()*p3.w()-p1.w()*p2.w()*p3.x();
  Type d1y = p1.y()*p2.w()*p3.w()-p1.w()*p2.y()*p3.w();
  Type d2y = p1.y()*p2.w()*p3.w()-p1.w()*p2.w()*p3.y();
  Type d1z = p1.z()*p2.w()*p3.w()-p1.w()*p2.z()*p3.w();
  Type d2z = p1.z()*p2.w()*p3.w()-p1.w()*p2.w()*p3.z();
  return d1x*d2y == d1y*d2x && d1x*d2z == d1z*d2x;
}

//: force the point point_infinite_ to infinity, without changing the line
template <class Type>
void vgl_homg_line_3d_2_points<Type>::force_point2_infinite(void) const
{
  if (point_infinite_.w() == 0) return; // already OK
  else if (point_finite_.w() == 0) // interchange the points
  {
    vgl_homg_point_3d<Type> t=point_infinite_;
    point_infinite_=point_finite_;
    point_finite_=t;
    return;
  }
  Type a=point_finite_.x(), a1=point_infinite_.x(),
       b=point_finite_.y(), b1=point_infinite_.y(),
       c=point_finite_.z(), c1=point_infinite_.z(),
       d=point_finite_.w(), d1=point_infinite_.w();
  point_infinite_.set(a*d1-a1*d, b*d1-b1*d, c*d1-c1*d, 0);
}

//*****************************************************************************
// stream operators
//*****************************************************************************

template <class Type>
vcl_ostream& operator<<(vcl_ostream &s,
                    const vgl_homg_line_3d_2_points<Type> &p)
{
  return s << "<vgl_homg_line_3d_2_points  finite_point: "
           << p.get_point_finite()
           << " infinite_point: " << p.get_point_infinite_() << ">";
}

#endif // vgl_homg_line_3d_2_points_txx_
