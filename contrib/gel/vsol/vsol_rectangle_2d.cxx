#include <vsol/vsol_rectangle_2d.h>

//*****************************************************************************
// External declarations for implementation
//*****************************************************************************
#include <vcl_cassert.h>
#include <vnl/vnl_vector_fixed.h>
#include <vsol/vsol_point_2d.h>

//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
// -- Constructor from 3 points.
// Description: `new_p0' is the origin of the rectangle. `new_p1' defines
//              the abscissa axis and the width. `new_p2' defines the
//              ordinate axis and the height.
// Require: valid_vertices(new_p0,new_p1,new_p2)
//---------------------------------------------------------------------------
vsol_rectangle_2d::vsol_rectangle_2d(const vsol_point_2d_sptr &new_p0,
                                     const vsol_point_2d_sptr &new_p1,
                                     const vsol_point_2d_sptr &new_p2)
{
  storage_=new vcl_vector<vsol_point_2d_sptr>(3);
  (*storage_)[0]=new_p0;
  (*storage_)[1]=new_p1;
  (*storage_)[2]=new_p2;
  // require
  assert(valid_vertices(*storage_));
}

//---------------------------------------------------------------------------
// Copy constructor
//---------------------------------------------------------------------------
vsol_rectangle_2d::vsol_rectangle_2d(const vsol_rectangle_2d &other)
  : vsol_polygon_2d(other)
{
}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vsol_rectangle_2d::~vsol_rectangle_2d()
{
}

//---------------------------------------------------------------------------
// -- Clone `this': creation of a new object and initialization
// See Prototype pattern
//---------------------------------------------------------------------------
vsol_spatial_object_2d_sptr vsol_rectangle_2d::clone(void) const
{
  return new vsol_rectangle_2d(*this);
}

//***************************************************************************
// Access
//***************************************************************************

//---------------------------------------------------------------------------
// -- Return the first vertex
//---------------------------------------------------------------------------
vsol_point_2d_sptr vsol_rectangle_2d::p0(void) const
{
  return (*storage_)[0];
}

//---------------------------------------------------------------------------
// -- Return the second vertex
//---------------------------------------------------------------------------
vsol_point_2d_sptr vsol_rectangle_2d::p1(void) const
{
  return (*storage_)[1];
}

//---------------------------------------------------------------------------
// -- Return the third vertex
//---------------------------------------------------------------------------
vsol_point_2d_sptr vsol_rectangle_2d::p2(void) const
{
  return (*storage_)[2];
}

//---------------------------------------------------------------------------
// -- Return the last vertex
//---------------------------------------------------------------------------
vsol_point_2d_sptr vsol_rectangle_2d::p3(void) const
{
  vsol_point_2d_sptr result;
  vnl_vector_fixed<double,2> *v;

  result=new vsol_point_2d(*(*storage_)[0]);
  v=(*storage_)[1]->to_vector(*(*storage_)[2]);
  result->add_vector(*v);
  delete v;

  return result;
}

//***************************************************************************
// Comparison
//***************************************************************************

//---------------------------------------------------------------------------
// -- Has `this' the same points than `other' in the same order ?
//---------------------------------------------------------------------------
bool vsol_rectangle_2d::operator==(const vsol_rectangle_2d &other) const
{
  return vsol_polygon_2d::operator==(other);
}

bool vsol_rectangle_2d::operator==(const vsol_polygon_2d &other) const
{
  return vsol_polygon_2d::operator==(other);
}

// -- spatial object equality

bool vsol_rectangle_2d::operator==(const vsol_spatial_object_2d& obj) const
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
// -- Compute the bounding box of `this'
//---------------------------------------------------------------------------
void vsol_rectangle_2d::compute_bounding_box(void)
{
  double xmin;
  double xmax;
  double ymin;
  double ymax;

  vsol_point_2d_sptr tp3;

  tp3=p3();

  xmin=(*storage_)[0]->x();
  ymin=(*storage_)[0]->y();
  xmax=xmin;
  ymax=ymin;

  if((*storage_)[1]->x()<xmin)
    xmin=(*storage_)[1]->x();
  else if((*storage_)[1]->x()>xmax)
    xmax=(*storage_)[1]->x();
  if((*storage_)[1]->y()<ymin)
    ymin=(*storage_)[1]->y();
  else if((*storage_)[1]->y()>ymax)
    ymax=(*storage_)[1]->y();

  if((*storage_)[2]->x()<xmin)
    xmin=(*storage_)[2]->x();
  else if((*storage_)[2]->x()>xmax)
    xmax=(*storage_)[2]->x();
  if((*storage_)[2]->y()<ymin)
    ymin=(*storage_)[2]->y();
  else if((*storage_)[2]->y()>ymax)
    ymax=(*storage_)[2]->y();

  if(tp3->x()<xmin)
    xmin=tp3->x();
  else if(tp3->x()>xmax)
    xmax=tp3->x();
  if(tp3->y()<ymin)
    ymin=tp3->y();
  else if(tp3->y()>ymax)
    ymax=tp3->y();

  if(_bounding_box==0)
    _bounding_box=new vsol_box_2d();
  _bounding_box->set_min_x(xmin);
  _bounding_box->set_max_x(xmax);
  _bounding_box->set_min_y(ymin);
  _bounding_box->set_max_y(ymax);

}

//---------------------------------------------------------------------------
// -- Return the width
//---------------------------------------------------------------------------
double vsol_rectangle_2d::width(void) const
{
  return (*storage_)[0]->distance((*storage_)[1]);
}

//---------------------------------------------------------------------------
// -- Return the height
//---------------------------------------------------------------------------
double vsol_rectangle_2d::height(void) const
{
  return (*storage_)[1]->distance((*storage_)[2]);
}

//---------------------------------------------------------------------------
// -- Return the area of `this'
//---------------------------------------------------------------------------
double vsol_rectangle_2d::area(void) const
{
  double result;
  result=width()*height();
  return result;
}

//---------------------------------------------------------------------------
// -- Are `new_vertices' valid to build a rectangle ?
//---------------------------------------------------------------------------
bool vsol_rectangle_2d::valid_vertices(const vcl_vector<vsol_point_2d_sptr> new_vertices) const
{
  if (new_vertices.size() != 3) return false;
  vnl_vector_fixed<double,2>* a=new_vertices[0]->to_vector(*(new_vertices[1]));
  vnl_vector_fixed<double,2>* b=new_vertices[1]->to_vector(*(new_vertices[2]));
  bool result=dot_product(*a,*b)==0;
  delete b;
  delete a;
  return result;
}
