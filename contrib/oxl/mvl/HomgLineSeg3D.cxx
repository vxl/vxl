// This is oxl/mvl/HomgLineSeg3D.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
//  \file

#include "HomgLineSeg3D.h"
#include <mvl/HomgLine3D.h>
#include <mvl/HomgOperator3D.h>
#include <vcl_iostream.h>

//--------------------------------------------------------------
//
//: Default constructor
HomgLineSeg3D::HomgLineSeg3D ()
{
}

//--------------------------------------------------------------
//
//: Constructor forming line segment from start and end points
HomgLineSeg3D::HomgLineSeg3D (const HomgPoint3D& point1, const HomgPoint3D& point2):
  HomgLine3D(point1, point2)
{
  _point1 = point1;
  _point2 = point2;
}


//--------------------------------------------------------------
//
//: Destructor
HomgLineSeg3D::~HomgLineSeg3D()
{
}


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
  return _point1;
}

//--------------------------------------------------------------
//
//: Return the second point of the line segment
const HomgPoint3D& HomgLineSeg3D::get_point2 () const
{
  return _point2;
}


//--------------------------------------------------------------
//
// Set the line segment given two points
void HomgLineSeg3D::set (const HomgPoint3D& point1, const HomgPoint3D& point2)
{
  _point1 = point1;
  _point2 = point2;
}

vcl_ostream& operator << (vcl_ostream& s, const HomgLineSeg3D& l)
{
  return s << l.get_point1().get_vector() << ", " << l.get_point2().get_vector();
}
