#include <vsol/vsol_rectangle_3d.h>

//*****************************************************************************
// External declarations for implementation
//*****************************************************************************
#include <vcl_cassert.h>
#include <vnl/vnl_vector_fixed.h>
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
  vsol_point_3d_sptr result;
  vnl_vector_fixed<double,3> *v;

  result=new vsol_point_3d(*(*storage_)[0]);
  v=(*storage_)[1]->to_vector(*(*storage_)[2]);
  result->add_vector(*v);
  delete v;

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
  vsol_point_3d_sptr tp3;

  double xmin;
  double xmax;
  double ymin;
  double ymax;
  double zmin;
  double zmax;


  tp3=p3();

  xmin=(*storage_)[0]->x();
  ymin=(*storage_)[0]->y();
  zmin=(*storage_)[0]->z();
  xmax=xmin;
  ymax=ymin;
  zmax=zmin;

  if((*storage_)[1]->x()<xmin)
    xmin=(*storage_)[1]->x();
  else if((*storage_)[1]->x()>xmax)
    xmax=(*storage_)[1]->x();
  if((*storage_)[1]->y()<ymin)
    ymin=(*storage_)[1]->y();
  else if((*storage_)[1]->y()>ymax)
    ymax=(*storage_)[1]->y();
  if((*storage_)[1]->z()<zmin)
    zmin=(*storage_)[1]->z();
  else if((*storage_)[1]->z()>zmax)
    zmax=(*storage_)[1]->z();

  if((*storage_)[2]->x()<xmin)
    xmin=(*storage_)[2]->x();
  else if((*storage_)[2]->x()>xmax)
    xmax=(*storage_)[2]->x();
  if((*storage_)[2]->y()<ymin)
    ymin=(*storage_)[2]->y();
  else if((*storage_)[2]->y()>ymax)
    ymax=(*storage_)[2]->y();
  if((*storage_)[2]->z()<zmin)
    zmin=(*storage_)[2]->z();
  else if((*storage_)[2]->z()>zmax)
    zmax=(*storage_)[2]->z();

  if(tp3->x()<xmin)
    xmin=tp3->x();
  else if(tp3->x()>xmax)
    xmax=tp3->x();
  if(tp3->y()<ymin)
    ymin=tp3->y();
  else if(tp3->y()>ymax)
    ymax=tp3->y();
  if(tp3->z()<zmin)
    zmin=tp3->z();
  else if(tp3->z()>zmax)
    zmax=tp3->z();

  if(_bounding_box==0)
    _bounding_box=new vsol_box_3d();
  _bounding_box->set_min_x(xmin);
  _bounding_box->set_max_x(xmax);
  _bounding_box->set_min_y(ymin);
  _bounding_box->set_max_y(ymax);
  _bounding_box->set_min_z(zmin);
  _bounding_box->set_max_z(zmax);
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
  vnl_vector_fixed<double,3>* a=new_vertices[0]->to_vector(*(new_vertices[1]));
  vnl_vector_fixed<double,3>* b=new_vertices[1]->to_vector(*(new_vertices[2]));
  bool result=dot_product(*a,*b)==0;
  delete b;
  delete a;
  return result;
}

//***************************************************************************
// Basic operations
//***************************************************************************

//---------------------------------------------------------------------------
//: Is `p' in `this' ?
//---------------------------------------------------------------------------
bool vsol_rectangle_3d::in(const vsol_point_3d_sptr &p) const
{
  // TO DO
  vcl_cerr << "Warning: vsol_rectangle_3d::in() has not been implemented yet\n";
  return false;
}

//---------------------------------------------------------------------------
//: Return the unit normal vector at point `p'. Have to be deleted manually
// Require: in(p)
//---------------------------------------------------------------------------
vnl_vector_fixed<double,3> *
vsol_rectangle_3d::normal_at_point(const vsol_point_3d_sptr &p) const
{
  // require
  assert(in(p));

  vnl_vector_fixed<double,3> *result;
  vnl_vector_fixed<double,3> v1((*storage_)[1]->x()-(*storage_)[0]->x(),
                                (*storage_)[1]->y()-(*storage_)[0]->y(),
                                (*storage_)[1]->z()-(*storage_)[0]->z());
  vnl_vector_fixed<double,3> v2((*storage_)[2]->x()-(*storage_)[0]->x(),
                                (*storage_)[2]->y()-(*storage_)[0]->y(),
                                (*storage_)[2]->z()-(*storage_)[0]->z());

  result=new vnl_vector_fixed<double,3>(cross_3d(v1,v2));
  if((*result)[0]!=0||(*result)[1]!=0||(*result)[2]!=0)
    result->normalize();
  return result;
}
