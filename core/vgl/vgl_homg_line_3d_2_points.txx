// Author: Don Hamilton. Peter Tu
// Copyright:
// Created: Feb 15 2000
//: Represents a homogeneous 3d line.


//--------------------------------------------------------------
//
// Class : vgl_homg_line_3d_2_points
//
//--------------------------------------------------------------

#include <vgl/vgl_homg_line_3d_2_points.h> 

// -- Constructor
template <class Type>
vgl_homg_line_3d_2_points<Type>::vgl_homg_line_3d_2_points()
{
}

// -- Constructor
template <class Type>
vgl_homg_line_3d_2_points<Type>::vgl_homg_line_3d_2_points( const vgl_homg_line_3d_2_points<Type> &that)
  : point_finite_(that.point_finite_)
  , point_infinite_(that.point_infinite_)
{
}

// -- Constructor, initialise using the specified distinct points
// on the line.
template <class Type>
vgl_homg_line_3d_2_points<Type>::vgl_homg_line_3d_2_points (const vgl_homg_point_3d<Type>& start,
                                                            const vgl_homg_point_3d<Type>& end)
{
  // ho_quadvecstd_points2_to_line
  bool start_finite = start.w() != 0;
  bool end_finite = end.w() != 0;
  
  if (start_finite && end_finite) {
    point_finite_ = start;

    Type dx = end.x() - start.x();
    Type dy = end.y() - start.y();
    Type dz = end.z() - start.z();
    
    point_infinite_.set(dx,dy,dz, 0.0);
  } else if (end_finite) {
    // Start infinite
    point_finite_ = end;
    point_infinite_ = start;
  } else {
    // End infinite -- just assign
    point_finite_ = start;
    point_infinite_ = end;
  }
}

// -- Destructor
template <class Type>
vgl_homg_line_3d_2_points<Type>::~vgl_homg_line_3d_2_points() {}

//-----------------------------------------------------------------------------
//

// -- Push point2 off to infinity
template <class Type>
void vgl_homg_line_3d_2_points<Type>::force_point2_infinite()
{
}

// TODO need to create the structure coresponding to Double3

// -- Return line direction as a 3-vector
// vgl_homg_line_3d_2_points::dir() const
// {
//   const IUE_vector<double>& d = point_infinite_.get_vector();
//  if (d[3] != 0) { 
//    cerr << *this;
//    cerr << "*** vgl_homg_line_3d_2_points: Infinite point not at infinity!! ***\n";
//  }
//  return Double3(d[0], d[1], d[2])
// }


template <class Type>
ostream& operator<<(ostream& s, const vgl_homg_line_3d_2_points<Type>& p)
{
  return s << "<vgl_homg_line_3d_2_points  finite_point: " 
           << p.get_point_finite()
           << " infinite_point: " << p.get_point_infinite_() << ">";
}


