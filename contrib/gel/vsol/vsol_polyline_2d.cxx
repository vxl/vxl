// This is gel/vsol/vsol_polyline_2d.cxx
#include "vsol_polyline_2d.h"
//:
// \file

#include <vsol/vsol_point_2d.h>
#include <vgl/algo/vgl_homg_operators_2d.h>
#include <vcl_cmath.h> // for vcl_abs(double)

//*****************************************************************************
// External declarations for implementation
//*****************************************************************************
#include <vcl_cassert.h>

//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
//: Default Constructor
//---------------------------------------------------------------------------
vsol_polyline_2d::vsol_polyline_2d()
{
	storage_=new vcl_vector<vsol_point_2d_sptr>();
}

//---------------------------------------------------------------------------
//: Constructor from a vcl_vector of points
//---------------------------------------------------------------------------

vsol_polyline_2d::vsol_polyline_2d(const vcl_vector<vsol_point_2d_sptr> &new_vertices)
{
  storage_=new vcl_vector<vsol_point_2d_sptr>(new_vertices);
}

//---------------------------------------------------------------------------
// Copy constructor
//---------------------------------------------------------------------------
vsol_polyline_2d::vsol_polyline_2d(const vsol_polyline_2d &other)
{
  storage_=new vcl_vector<vsol_point_2d_sptr>(*other.storage_);
  for (unsigned int i=0;i<storage_->size();++i)
    (*storage_)[i]=new vsol_point_2d(*((*other.storage_)[i]));
}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vsol_polyline_2d::~vsol_polyline_2d()
{
  delete storage_;
}

//---------------------------------------------------------------------------
//: Clone `this': creation of a new object and initialization
// See Prototype pattern
//---------------------------------------------------------------------------
vsol_spatial_object_2d_sptr vsol_polyline_2d::clone(void) const
{
  return new vsol_polyline_2d(*this);
}

//***************************************************************************
// Access
//***************************************************************************

//---------------------------------------------------------------------------
//: Return the first point of `this'
//---------------------------------------------------------------------------
vsol_point_2d_sptr vsol_polyline_2d::p0(void) const
{
  return p0_;
}

//---------------------------------------------------------------------------
//: Return the last point of `this'
//---------------------------------------------------------------------------
vsol_point_2d_sptr vsol_polyline_2d::p1(void) const
{
  return p1_;
}

//---------------------------------------------------------------------------
//: Return vertex `i'
//  REQUIRE: valid_index(i)
//---------------------------------------------------------------------------
vsol_point_2d_sptr vsol_polyline_2d::vertex(const int i) const
{
  // require
  assert(valid_index(i));

  return (*storage_)[i];
}

//***************************************************************************
// Comparison
//***************************************************************************

//---------------------------------------------------------------------------
//: Has `this' the same points than `other' in the same order ?
//---------------------------------------------------------------------------
bool vsol_polyline_2d::operator==(const vsol_polyline_2d &other) const
{
  bool result = (this==&other);

  if (!result)
  {
    result = (storage_->size()==other.storage_->size());
    if (result)
    {
      vsol_point_2d_sptr p=(*storage_)[0];

      unsigned int i=0;
      for (result=false;i<storage_->size()&&!result;++i)
        result = (*p==*(*other.storage_)[i]);
      if (result)
      {
        for (int j=1;j<size()&&result;++i,++j)
        {
          if (i>=storage_->size()) i=0;
          result = ((*storage_)[i]==(*storage_)[j]);
        }
      }
    }
  }
  return result;
}

//: spatial object equality

bool vsol_polyline_2d::operator==(const vsol_spatial_object_2d& obj) const
{
  return
   obj.spatial_type() == vsol_spatial_object_2d::CURVE &&
   ((vsol_curve_2d const&)obj).curve_type() == vsol_curve_2d::POLYLINE
  ? *this == (vsol_polyline_2d const&) (vsol_polyline_2d const&) obj
  : false;
}

//***************************************************************************
// Status report
//***************************************************************************

//---------------------------------------------------------------------------
//: Return the real type of a conic. It is a CURVE
//---------------------------------------------------------------------------
vsol_spatial_object_2d::vsol_spatial_object_2d_type
vsol_polyline_2d::spatial_type(void) const
{
  return CURVE;
}

//---------------------------------------------------------------------------
//: Return the length of `this'
//---------------------------------------------------------------------------
double vsol_polyline_2d::length(void) const
{
  assert(false); // TO DO
  return -1;
}


//---------------------------------------------------------------------------
//: Compute the bounding box of `this'
//---------------------------------------------------------------------------
void vsol_polyline_2d::compute_bounding_box(void) const
{
  set_bounding_box((*storage_)[0]->x(), (*storage_)[0]->y());
  for (unsigned int i=1;i<storage_->size();++i)
    add_to_bounding_box((*storage_)[i]->x(), (*storage_)[i]->y());
}

//---------------------------------------------------------------------------
//: Return the number of vertices
//---------------------------------------------------------------------------
int vsol_polyline_2d::size(void) const
{
  return storage_->size();
}


//***************************************************************************
// Status setting
//***************************************************************************

//---------------------------------------------------------------------------
//: Set the first point of the curve
// Require: in(new_p0)
//---------------------------------------------------------------------------
void vsol_polyline_2d::set_p0(const vsol_point_2d_sptr &new_p0)
{
  p0_=new_p0;
  storage_->push_back(p0_);
}

//---------------------------------------------------------------------------
//: Set the last point of the curve
// Require: in(new_p1)
//---------------------------------------------------------------------------
void vsol_polyline_2d::set_p1(const vsol_point_2d_sptr &new_p1)
{
  p1_=new_p1;
  storage_->push_back(p0_);
}

//---------------------------------------------------------------------------
//: Add another point to the curve
//---------------------------------------------------------------------------
void vsol_polyline_2d::add_vertex(const vsol_point_2d_sptr &new_p)
{
  storage_->push_back(new_p);
}