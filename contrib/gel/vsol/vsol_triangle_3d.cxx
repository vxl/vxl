// This is gel/vsol/vsol_triangle_3d.cxx
#include "vsol_triangle_3d.h"
//:
// \file

//*****************************************************************************
// External declarations for implementation
//*****************************************************************************
#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <vsol/vsol_point_3d.h>
#include <vnl/vnl_math.h>

//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
//: Constructor from its 3 vertices
//---------------------------------------------------------------------------
vsol_triangle_3d::vsol_triangle_3d(const vsol_point_3d_sptr &new_p0,
                                   const vsol_point_3d_sptr &new_p1,
                                   const vsol_point_3d_sptr &new_p2)
{
  storage_=new vcl_vector<vsol_point_3d_sptr>(3);
  (*storage_)[0]=new_p0;
  (*storage_)[1]=new_p1;
  (*storage_)[2]=new_p2;
}

//---------------------------------------------------------------------------
// Copy constructor
//---------------------------------------------------------------------------
vsol_triangle_3d::vsol_triangle_3d(const vsol_triangle_3d &other)
  : vsol_polygon_3d(other)
{
}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vsol_triangle_3d::~vsol_triangle_3d()
{
}

//---------------------------------------------------------------------------
//: Clone `this': creation of a new object and initialization
// See Prototype pattern
//---------------------------------------------------------------------------
vsol_spatial_object_3d_sptr vsol_triangle_3d::clone(void) const
{
  return new vsol_triangle_3d(*this);
}

//***************************************************************************
// Access
//***************************************************************************

//---------------------------------------------------------------------------
//: Return the first vertex
//---------------------------------------------------------------------------
vsol_point_3d_sptr vsol_triangle_3d::p0(void) const
{
  return (*storage_)[0];
}

//---------------------------------------------------------------------------
//: Return the second vertex
//---------------------------------------------------------------------------
vsol_point_3d_sptr vsol_triangle_3d::p1(void) const
{
  return (*storage_)[1];
}

//---------------------------------------------------------------------------
//: Return the last vertex
//---------------------------------------------------------------------------
vsol_point_3d_sptr vsol_triangle_3d::p2(void) const
{
  return (*storage_)[2];
}

//***************************************************************************
// Comparison
//***************************************************************************

//---------------------------------------------------------------------------
//: Has `this' the same points than `other' in the same order ?
//---------------------------------------------------------------------------
bool vsol_triangle_3d::operator==(const vsol_triangle_3d &other) const
{
  return vsol_polygon_3d::operator==(other);
}

bool vsol_triangle_3d::operator==(const vsol_polygon_3d &other) const
{
  return vsol_polygon_3d::operator==(other);
}

//: spatial object equality

bool vsol_triangle_3d::operator==(const vsol_spatial_object_3d& obj) const
{
  return
   obj.spatial_type() == vsol_spatial_object_3d::REGION &&
   ((vsol_region_3d const&)obj).region_type() == vsol_region_3d::POLYGON
  ? *this == (vsol_polygon_3d const&) (vsol_region_3d const&) obj
  : false;
}

//***************************************************************************
// Status report
//***************************************************************************

//---------------------------------------------------------------------------
//: Return the area of `this'
//---------------------------------------------------------------------------
double vsol_triangle_3d::area(void) const
{
  double dx02=(*storage_)[0]->x()-(*storage_)[2]->x();
  double dy02=(*storage_)[0]->y()-(*storage_)[2]->y();
  double dz02=(*storage_)[0]->z()-(*storage_)[2]->z();
  double dx12=(*storage_)[1]->x()-(*storage_)[2]->x();
  double dy12=(*storage_)[1]->y()-(*storage_)[2]->y();
  double dz12=(*storage_)[1]->z()-(*storage_)[2]->z();
  return vcl_sqrt( vnl_math_sqr(dy02*dz12-dy12*dz02)
                  +vnl_math_sqr(dz02*dx12-dz12*dx02)
                  +vnl_math_sqr(dx02*dy12-dx12*dy02))/2;
}
//***************************************************************************
// Element change
//***************************************************************************

//---------------------------------------------------------------------------
//: Set the first vertex
//---------------------------------------------------------------------------
void vsol_triangle_3d::set_p0(const vsol_point_3d_sptr &new_p0)
{
  (*storage_)[0]=new_p0;
}

//---------------------------------------------------------------------------
//: Set the second vertex
//---------------------------------------------------------------------------
void vsol_triangle_3d::set_p1(const vsol_point_3d_sptr &new_p1)
{
  (*storage_)[1]=new_p1;
}

//---------------------------------------------------------------------------
//: Set the last vertex
//---------------------------------------------------------------------------
void vsol_triangle_3d::set_p2(const vsol_point_3d_sptr &new_p2)
{
  (*storage_)[2]=new_p2;
}

//***************************************************************************
// Basic operations
//***************************************************************************

//---------------------------------------------------------------------------
//: Is `p' in `this' ?
//---------------------------------------------------------------------------
bool vsol_triangle_3d::in(const vsol_point_3d_sptr& ) const
{
  // TODO
  vcl_cerr << "Warning: vsol_triangle_3d::in() has not been implemented yet\n";
  return true;
}

//---------------------------------------------------------------------------
//: Return the unit normal vector at point `p'.
// This is the normal to the place in which the triangle lies.
// If the vertices are collinear, the normal is the null vector.
// Require: in(p)
//---------------------------------------------------------------------------
vgl_vector_3d<double>
vsol_triangle_3d::normal_at_point(const vsol_point_3d_sptr &p) const
{
  // require
  assert(in(p));

  // Since a rectangle is planar, the answer is independent of p:
  vgl_vector_3d<double> v1((*storage_)[1]->x()-(*storage_)[0]->x(),
                           (*storage_)[1]->y()-(*storage_)[0]->y(),
                           (*storage_)[1]->z()-(*storage_)[0]->z());
  vgl_vector_3d<double> v2((*storage_)[2]->x()-(*storage_)[0]->x(),
                           (*storage_)[2]->y()-(*storage_)[0]->y(),
                           (*storage_)[2]->z()-(*storage_)[0]->z());

  return normalized(cross_product(v1,v2));
}

inline void vsol_triangle_3d::describe(vcl_ostream &strm, int blanking) const
{
  if (blanking < 0) blanking = 0; while (blanking--) strm << ' ';
  strm << "<vsol_triangle_3d with corners";
  for (unsigned int i=0; i<size(); ++i)
    strm << ' ' << *(vertex(i));
  strm << '>' << vcl_endl;
}
