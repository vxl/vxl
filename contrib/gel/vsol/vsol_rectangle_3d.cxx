// This is gel/vsol/vsol_rectangle_3d.cxx
#include "vsol_rectangle_3d.h"
//:
// \file

//*****************************************************************************
// External declarations for implementation
//*****************************************************************************
#include <vcl_cassert.h>
#include <vgl/vgl_vector_3d.h>
#include <vsol/vsol_point_3d.h>
#include <vcl_iostream.h>

//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
//: Constructor from 3 points.
// Description: `new_p0' is the origin of the rectangle. `new_p1' defines
//              the abscissa axis and the width. `new_p2' defines the
//              ordinate axis and the height.
// Require: valid_vertices(new_p0,new_p1,new_p2)
//---------------------------------------------------------------------------
vsol_rectangle_3d::vsol_rectangle_3d(const vsol_point_3d_sptr &new_p0,
                                     const vsol_point_3d_sptr &new_p1,
                                     const vsol_point_3d_sptr &new_p2)
{
  storage_=new vcl_vector<vsol_point_3d_sptr>(3);
  (*storage_)[0]=new_p0;
  (*storage_)[1]=new_p1;
  (*storage_)[2]=new_p2;
  // require
  assert(valid_vertices(*storage_));
}

//---------------------------------------------------------------------------
// Copy constructor
//---------------------------------------------------------------------------
vsol_rectangle_3d::vsol_rectangle_3d(const vsol_rectangle_3d &other)
  : vsol_polygon_3d(other)
{
}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vsol_rectangle_3d::~vsol_rectangle_3d()
{
}

//---------------------------------------------------------------------------
//: Clone `this': creation of a new object and initialization
// See Prototype pattern
//---------------------------------------------------------------------------
vsol_spatial_object_3d_sptr vsol_rectangle_3d::clone(void) const
{
  return new vsol_rectangle_3d(*this);
}

//***************************************************************************
// Access
//***************************************************************************

//---------------------------------------------------------------------------
//: Return the first vertex
//---------------------------------------------------------------------------
vsol_point_3d_sptr vsol_rectangle_3d::p0(void) const
{
  return (*storage_)[0];
}

//---------------------------------------------------------------------------
//: Return the second vertex
//---------------------------------------------------------------------------
vsol_point_3d_sptr vsol_rectangle_3d::p1(void) const
{
  return (*storage_)[1];
}

//---------------------------------------------------------------------------
//: Return the third vertex
//---------------------------------------------------------------------------
vsol_point_3d_sptr vsol_rectangle_3d::p2(void) const
{
  return (*storage_)[2];
}

//---------------------------------------------------------------------------
//: Return the last vertex
//---------------------------------------------------------------------------
vsol_point_3d_sptr vsol_rectangle_3d::p3(void) const
{
  vsol_point_3d_sptr result=new vsol_point_3d(*(*storage_)[0]);
  vgl_vector_3d<double> v=(*storage_)[1]->to_vector(*(*storage_)[2]);
  result->add_vector(v);
  return result;
}

//***************************************************************************
// Comparison
//***************************************************************************

//---------------------------------------------------------------------------
//: Has `this' the same points than `other' in the same order ?
//---------------------------------------------------------------------------
bool vsol_rectangle_3d::operator==(const vsol_rectangle_3d &other) const
{
  return vsol_polygon_3d::operator==(other);
}

bool vsol_rectangle_3d::operator==(const vsol_polygon_3d &other) const
{
  return vsol_polygon_3d::operator==(other);
}

//: spatial object equality

bool vsol_rectangle_3d::operator==(const vsol_spatial_object_3d& obj) const
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
//: Compute the bounding box of `this'
//---------------------------------------------------------------------------
void vsol_rectangle_3d::compute_bounding_box(void)
{
  double xmin=(*storage_)[0]->x();
  double ymin=(*storage_)[0]->y();
  double zmin=(*storage_)[0]->z();
  double xmax=xmin;
  double ymax=ymin;
  double zmax=zmin;

  for (unsigned int i=1;i<3;++i)
  {
    double x=(*storage_)[i]->x();
    if      (x<xmin) xmin=x;
    else if (x>xmax) xmax=x;
    double y=(*storage_)[i]->y();
    if      (y<ymin) ymin=y;
    else if (y>ymax) ymax=y;
    double z=(*storage_)[i]->z();
    if      (z<zmin) zmin=z;
    else if (z>zmax) zmax=z;
  }
  double x=p3()->x();
  if      (x<xmin) xmin=x;
  else if (x>xmax) xmax=x;
  double y=p3()->y();
  if      (y<ymin) ymin=y;
  else if (y>ymax) ymax=y;
  double z=p3()->z();
  if      (z<zmin) zmin=z;
  else if (z>zmax) zmax=z;

  if (bounding_box_==0)
    bounding_box_=new vsol_box_3d;
  bounding_box_->set_min_x(xmin);
  bounding_box_->set_max_x(xmax);
  bounding_box_->set_min_y(ymin);
  bounding_box_->set_max_y(ymax);
  bounding_box_->set_min_z(zmin);
  bounding_box_->set_max_z(zmax);
}

//---------------------------------------------------------------------------
//: Return the width
//---------------------------------------------------------------------------
double vsol_rectangle_3d::width(void) const
{
  return (*storage_)[0]->distance((*storage_)[1]);
}

//---------------------------------------------------------------------------
//: Return the height
//---------------------------------------------------------------------------
double vsol_rectangle_3d::height(void) const
{
  return (*storage_)[1]->distance((*storage_)[2]);
}

//---------------------------------------------------------------------------
//: Return the area of `this'
//---------------------------------------------------------------------------
double vsol_rectangle_3d::area(void) const
{
  double result;
  result=width()*height();
  return result;
}

//---------------------------------------------------------------------------
//: Are `new_vertices' valid to build a rectangle ?
//---------------------------------------------------------------------------
bool vsol_rectangle_3d::valid_vertices(const vcl_vector<vsol_point_3d_sptr> new_vertices) const
{
  vgl_vector_3d<double> a=new_vertices[0]->to_vector(*(new_vertices[1]));
  vgl_vector_3d<double> b=new_vertices[1]->to_vector(*(new_vertices[2]));
  // the two vectors should be orthogonal:
  return dot_product(a,b)==0;
}

//***************************************************************************
// Basic operations
//***************************************************************************

//---------------------------------------------------------------------------
//: Is `p' in `this' ?
//---------------------------------------------------------------------------
bool vsol_rectangle_3d::in(const vsol_point_3d_sptr &p) const
{
  // TODO
  vcl_cerr << "Warning: vsol_rectangle_3d::in() has not been implemented yet\n";
  return true;
}

//---------------------------------------------------------------------------
//: Return the unit normal vector at point `p'.
// Require: in(p)
//---------------------------------------------------------------------------
vgl_vector_3d<double>
vsol_rectangle_3d::normal_at_point(const vsol_point_3d_sptr &p) const
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
