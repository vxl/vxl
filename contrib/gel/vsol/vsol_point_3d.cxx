// This is gel/vsol/vsol_point_3d.cxx
#include "vsol_point_3d.h"
//:
// \file

//*****************************************************************************
// External declarations for implementation
//*****************************************************************************
#include <vgl/vgl_distance.h>

//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vsol_point_3d::~vsol_point_3d()
{
}

//---------------------------------------------------------------------------
//: Clone `this': creation of a new object and initialization
// See Prototype pattern
//---------------------------------------------------------------------------
vsol_spatial_object_3d_sptr vsol_point_3d::clone(void) const
{
  return new vsol_point_3d(*this);
}

//***************************************************************************
// Comparison
//***************************************************************************

//---------------------------------------------------------------------------
//: Has `this' the same coordinates than `other' ?
//---------------------------------------------------------------------------
bool vsol_point_3d::operator==(const vsol_point_3d &other) const
{
  return this==&other || p_==other.p_;
}

//: spatial object equality

bool vsol_point_3d::operator==(const vsol_spatial_object_3d& obj) const
{
  return
   obj.spatial_type() == vsol_spatial_object_3d::POINT
  ? operator==((vsol_point_3d const&)obj)
  : false;
}

//***************************************************************************
// Status report
//***************************************************************************

//---------------------------------------------------------------------------
//: Return the real type of a point. It is a POINT
//---------------------------------------------------------------------------
vsol_spatial_object_3d::vsol_spatial_object_3d_type
vsol_point_3d::spatial_type(void) const
{
  return POINT;
}

//---------------------------------------------------------------------------
//: Compute the bounding box of `this'
//---------------------------------------------------------------------------
void vsol_point_3d::compute_bounding_box(void)
{
  if (!bounding_box_)
    bounding_box_=new vsol_box_3d;
  bounding_box_->set_min_x(p_.x());
  bounding_box_->set_max_x(p_.x());
  bounding_box_->set_min_y(p_.y());
  bounding_box_->set_max_y(p_.y());
  bounding_box_->set_min_z(p_.z());
  bounding_box_->set_max_z(p_.z());
}

//***************************************************************************
// Status setting
//***************************************************************************

//---------------------------------------------------------------------------
//: Set the abscissa
//---------------------------------------------------------------------------
void vsol_point_3d::set_x(const double new_x)
{
  p_.set(new_x, p_.y(), p_.z());
}

//---------------------------------------------------------------------------
//: Set the ordinate
//---------------------------------------------------------------------------
void vsol_point_3d::set_y(const double new_y)
{
  p_.set(p_.x(), new_y, p_.z());
}

//---------------------------------------------------------------------------
//: Set the ordinate
//---------------------------------------------------------------------------
void vsol_point_3d::set_z(const double new_z)
{
  p_.set(p_.x(), p_.y(), new_z);
}

//***************************************************************************
// Basic operations
//***************************************************************************

//---------------------------------------------------------------------------
//: Return the distance (N2) between `this' and `other'
//---------------------------------------------------------------------------
double vsol_point_3d::distance(const vsol_point_3d &other) const
{
  return vgl_distance(p_,other.p_);
}

double vsol_point_3d::distance(vsol_point_3d_sptr other) const
{
  return vgl_distance(p_,other->p_);
}

//---------------------------------------------------------------------------
//: Return the middle point between `this' and `other'
//---------------------------------------------------------------------------
vsol_point_3d_sptr vsol_point_3d::middle(const vsol_point_3d &other) const
{
  return new vsol_point_3d(midpoint(p_,other.p_));
}

//---------------------------------------------------------------------------
//: Add `v' to `this'
//---------------------------------------------------------------------------
void vsol_point_3d::add_vector(vgl_vector_3d<double> const& v)
{
  p_ += v;
}

//---------------------------------------------------------------------------
//: Add `v' and `this'
//---------------------------------------------------------------------------
vsol_point_3d_sptr
vsol_point_3d::plus_vector(vgl_vector_3d<double> const& v) const
{
  return new vsol_point_3d(p_ + v);
}

//---------------------------------------------------------------------------
//: Return the vector `this',`other'.
//---------------------------------------------------------------------------
vgl_vector_3d<double>
vsol_point_3d::to_vector(const vsol_point_3d &other) const
{
  return vgl_vector_3d<double>(other.x() - x(),other.y() - y(),other.z() - z());
}
