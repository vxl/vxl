#include <vsol/vsol_point_3d.h>

//*****************************************************************************
// External declarations for implementation
//*****************************************************************************
#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <vgl/vgl_distance.h>

//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
// -- Constructor from cartesian coordinates `new_x', `new_y' and `new_z'
//---------------------------------------------------------------------------
vsol_point_3d::vsol_point_3d(const double new_x,
                             const double new_y,
                             const double new_z)
{
  p_=new vgl_point_3d<double>(new_x,new_y,new_z);

}

//---------------------------------------------------------------------------
// Copy constructor
//---------------------------------------------------------------------------
vsol_point_3d::vsol_point_3d(const vsol_point_3d &other)
{
  p_=new vgl_point_3d<double>(other.x(),other.y(),other.z());
}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vsol_point_3d::~vsol_point_3d()
{
  delete p_;
}

//---------------------------------------------------------------------------
// -- Clone `this': creation of a new object and initialization
// See Prototype pattern
//---------------------------------------------------------------------------
vsol_spatial_object_3d_sptr vsol_point_3d::clone(void) const
{
  return new vsol_point_3d(*this);
}

//***************************************************************************
// Access
//***************************************************************************

//---------------------------------------------------------------------------
// -- Return the abscissa
//---------------------------------------------------------------------------
double vsol_point_3d::x(void) const
{
  return p_->x();
}

//---------------------------------------------------------------------------
// -- Return the ordinate
//---------------------------------------------------------------------------
double vsol_point_3d::y(void) const
{
  return p_->y();
}

//---------------------------------------------------------------------------
// -- Return the ordinate
//---------------------------------------------------------------------------
double vsol_point_3d::z(void) const
{
  return p_->z();
}

//***************************************************************************
// Comparison
//***************************************************************************

//---------------------------------------------------------------------------
// -- Has `this' the same coordinates than `other' ?
//---------------------------------------------------------------------------
bool vsol_point_3d::operator==(const vsol_point_3d &other) const
{
  bool result;

  result=this==&other;
  if(!result)
    result=(*p_)==(*(other.p_));
  return result;
}

// -- spatial object equality

bool vsol_point_3d::operator==(const vsol_spatial_object_3d& obj) const
{
  return
   obj.spatial_type() == vsol_spatial_object_3d::POINT
  ? *this == (vsol_point_3d const&)obj
  : false;
}

#if 0 // function commented dout
//---------------------------------------------------------------------------
// -- Has `this' not the same coordinates than `other' ?
//---------------------------------------------------------------------------
bool vsol_point_3d::operator!=(const vsol_point_3d &other) const
{
  return !operator==(other);
}
#endif

//***************************************************************************
// Status report
//***************************************************************************

//---------------------------------------------------------------------------
// -- Return the real type of a point. It is a POINT
//---------------------------------------------------------------------------
enum  vsol_spatial_object_3d::vsol_spatial_object_3d_type
vsol_point_3d::spatial_type(void) const
{
  return POINT;
}

//---------------------------------------------------------------------------
// -- Compute the bounding box of `this'
//---------------------------------------------------------------------------
void vsol_point_3d::compute_bounding_box(void)
{
  if(_bounding_box==0)
    _bounding_box=new vsol_box_3d();
  _bounding_box->set_min_x(p_->x());
  _bounding_box->set_max_x(p_->x());
  _bounding_box->set_min_y(p_->y());
  _bounding_box->set_max_y(p_->y());
  _bounding_box->set_min_z(p_->z());
  _bounding_box->set_max_z(p_->z());
}

//***************************************************************************
// Status setting
//***************************************************************************

//---------------------------------------------------------------------------
// -- Set the abscissa
//---------------------------------------------------------------------------
void vsol_point_3d::set_x(const double new_x)
{
  p_->set_x(new_x);
}

//---------------------------------------------------------------------------
// -- Set the ordinate
//---------------------------------------------------------------------------
void vsol_point_3d::set_y(const double new_y)
{
  p_->set_y(new_y);
}

//---------------------------------------------------------------------------
// -- Set the ordinate
//---------------------------------------------------------------------------
void vsol_point_3d::set_z(const double new_z)
{

  p_->set_z(new_z);

}

//***************************************************************************
// Basic operations
//***************************************************************************

//---------------------------------------------------------------------------
// -- Return the distance (N2) between `this' and `other'
//---------------------------------------------------------------------------
double vsol_point_3d::distance(const vsol_point_3d &other) const
{
  return vgl_distance(*p_,*other.p_);
}

double vsol_point_3d::distance(vsol_point_3d_sptr other) const
{
  return vgl_distance(*p_,*other->p_);
}

//---------------------------------------------------------------------------
// -- Return the middle point between `this' and `other'
//---------------------------------------------------------------------------
vsol_point_3d_sptr vsol_point_3d::middle(const vsol_point_3d &other) const
{

  vsol_point_3d_sptr result=new vsol_point_3d(*this);
  (*(result->p_))=(*(result->p_)) + (*(other.p_));
  result->set_x(result->x()/2);
  result->set_y(result->y()/2);
  result->set_z(result->z()/2);

  return result;
}

//---------------------------------------------------------------------------
// -- Add `v' to `this'
//---------------------------------------------------------------------------
void vsol_point_3d::add_vector(const vnl_double_3 &v)
{
  p_->set_x(p_->x() + v[0]);
  p_->set_y(p_->y() + v[1]);
  p_->set_z(p_->z() + v[2]);

}

//---------------------------------------------------------------------------
// -- Add `v' and `this'
//---------------------------------------------------------------------------
vsol_point_3d_sptr
vsol_point_3d::plus_vector(const vnl_double_3 &v) const
{
  vsol_point_3d_sptr result=new vsol_point_3d(*this);
  result->add_vector(v);
  return result;
}

//---------------------------------------------------------------------------
// -- Return the vector `this',`other'. Has to be deleted manually
//---------------------------------------------------------------------------
vnl_double_3 *
vsol_point_3d::to_vector(const vsol_point_3d &other) const
{
  vnl_double_3 *result = new vnl_double_3;


  (*result)(0)=other.x() - x();
  (*result)(1)=other.y() - y();
  (*result)(2)=other.z() - z();

  return result;
}

//#include <vcl_rel_ops.h> // gcc 2.7
//VCL_INSTANTIATE_INLINE(bool operator!=(vsol_point_3d const &, vsol_point_3d const &));
