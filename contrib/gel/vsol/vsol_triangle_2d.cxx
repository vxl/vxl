// This is gel/vsol/vsol_triangle_2d.cxx
#include "vsol_triangle_2d.h"
//:
// \file

//*****************************************************************************
// External declarations for implementation
//*****************************************************************************
#include <vsol/vsol_point_2d.h>
#include <vcl_iostream.h>

//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
//: Constructor from its 3 vertices
//---------------------------------------------------------------------------
vsol_triangle_2d::vsol_triangle_2d(const vsol_point_2d_sptr &new_p0,
                                   const vsol_point_2d_sptr &new_p1,
                                   const vsol_point_2d_sptr &new_p2)
{
  storage_=new vcl_vector<vsol_point_2d_sptr>(3);
  (*storage_)[0]=new_p0;
  (*storage_)[1]=new_p1;
  (*storage_)[2]=new_p2;
}

//---------------------------------------------------------------------------
// Copy constructor
//---------------------------------------------------------------------------
vsol_triangle_2d::vsol_triangle_2d(const vsol_triangle_2d &other)
  : vsol_polygon_2d(other)
{
}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vsol_triangle_2d::~vsol_triangle_2d()
{
}

//---------------------------------------------------------------------------
//: Clone `this': creation of a new object and initialization
// See Prototype pattern
//---------------------------------------------------------------------------
vsol_spatial_object_2d* vsol_triangle_2d::clone(void) const
{
  return new vsol_triangle_2d(*this);
}

//***************************************************************************
// Access
//***************************************************************************

//---------------------------------------------------------------------------
//: Return the first vertex
//---------------------------------------------------------------------------
vsol_point_2d_sptr vsol_triangle_2d::p0(void) const
{
  return (*storage_)[0];
}

//---------------------------------------------------------------------------
//: Return the second vertex
//---------------------------------------------------------------------------
vsol_point_2d_sptr vsol_triangle_2d::p1(void) const
{
  return (*storage_)[1];
}

//---------------------------------------------------------------------------
//: Return the last vertex
//---------------------------------------------------------------------------
vsol_point_2d_sptr vsol_triangle_2d::p2(void) const
{
  return (*storage_)[2];
}

//***************************************************************************
// Comparison
//***************************************************************************

//---------------------------------------------------------------------------
//: Has `this' the same points than `other' in the same order ?
//---------------------------------------------------------------------------
bool vsol_triangle_2d::operator==(const vsol_triangle_2d &other) const
{
  return vsol_polygon_2d::operator==(other);
}

bool vsol_triangle_2d::operator==(const vsol_polygon_2d &other) const
{
  return vsol_polygon_2d::operator==(other);
}

//: spatial object equality

bool vsol_triangle_2d::operator==(const vsol_spatial_object_2d& obj) const
{
  return
   obj.spatial_type() == vsol_spatial_object_2d::REGION &&
   ((vsol_region_2d const&)obj).region_type() == vsol_region_2d::POLYGON
  ? *this == (vsol_polygon_2d const&) (vsol_region_2d const&) obj
  : false;
}

//***************************************************************************
// Status report
//***************************************************************************

//---------------------------------------------------------------------------
//: Return the area of `this'
//---------------------------------------------------------------------------
double vsol_triangle_2d::area(void) const
{
  double result;

  result=(((*storage_)[0]->x()-(*storage_)[1]->x())
          *((*storage_)[1]->y()-(*storage_)[2]->y())
          -((*storage_)[1]->x()-(*storage_)[2]->x())
          *((*storage_)[0]->y()-(*storage_)[1]->y()))/2;

  return result;
}

//***************************************************************************
// Element change
//***************************************************************************

//---------------------------------------------------------------------------
//: Set the first vertex
//---------------------------------------------------------------------------
void vsol_triangle_2d::set_p0(const vsol_point_2d_sptr &new_p0)
{
  (*storage_)[0]=new_p0;
}

//---------------------------------------------------------------------------
//: Set the second vertex
//---------------------------------------------------------------------------
void vsol_triangle_2d::set_p1(const vsol_point_2d_sptr &new_p1)
{
  (*storage_)[1]=new_p1;
}

//---------------------------------------------------------------------------
//: Set the last vertex
//---------------------------------------------------------------------------
void vsol_triangle_2d::set_p2(const vsol_point_2d_sptr &new_p2)
{
  (*storage_)[2]=new_p2;
}

inline void vsol_triangle_2d::describe(vcl_ostream &strm, int blanking) const
{
  if (blanking < 0) blanking = 0; while (blanking--) strm << ' ';
  strm << "<vsol_triangle_2d with corners";
  for (unsigned int i=0; i<size(); ++i)
    strm << ' ' << *(vertex(i));
  strm << '>' << vcl_endl;
}
