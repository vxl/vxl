// This is oxl/mvl/HomgLineSeg3D.cxx
//:
//  \file

#include <iostream>
#include "HomgLineSeg3D.h"
#include <mvl/HomgLine3D.h>
#include <mvl/HomgOperator3D.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//--------------------------------------------------------------
//
//: Default constructor
HomgLineSeg3D::HomgLineSeg3D () = default;

//--------------------------------------------------------------
//
//: Constructor forming line segment from start and end points
HomgLineSeg3D::HomgLineSeg3D (const HomgPoint3D& point1, const HomgPoint3D& point2):
  HomgLine3D(point1, point2)
{
  point1_ = point1;
  point2_ = point2;
}


//--------------------------------------------------------------
//
//: Destructor
HomgLineSeg3D::~HomgLineSeg3D() = default;


//--------------------------------------------------------------
//
//: Return the line through the two points.
const HomgLine3D& HomgLineSeg3D::get_line () const
{
  return *this;
}

//--------------------------------------------------------------
//
//: Return the first point of the line segment
const HomgPoint3D& HomgLineSeg3D::get_point1 () const
{
  return point1_;
}

//--------------------------------------------------------------
//
//: Return the second point of the line segment
const HomgPoint3D& HomgLineSeg3D::get_point2 () const
{
  return point2_;
}


//--------------------------------------------------------------
//
// Set the line segment given two points
void HomgLineSeg3D::set (const HomgPoint3D& point1, const HomgPoint3D& point2)
{
  point1_ = point1;
  point2_ = point2;
}

std::ostream& operator << (std::ostream& s, const HomgLineSeg3D& l)
{
  return s << l.get_point1().get_vector() << ", " << l.get_point2().get_vector();
}
