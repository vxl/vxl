#include "vsol_polyhedron.h"

//:
//  \file

//*****************************************************************************
// External declarations for implementation
//*****************************************************************************
#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include "vsol_point_3d.h"

//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
//: Constructor from a vcl_vector (not a geometric vector but a list of points)
// Require: new_vertices.size()>=4 and valid_vertices(new_vertices)
//---------------------------------------------------------------------------
vsol_polyhedron::vsol_polyhedron(vcl_vector<vsol_point_3d_sptr> const& new_vertices)
{
  // require
  assert(new_vertices.size()>=4);

  storage_=new_vertices; // element-wise assignment
}

//---------------------------------------------------------------------------
// Copy constructor
//---------------------------------------------------------------------------
vsol_polyhedron::vsol_polyhedron(vsol_polyhedron const &other)
{
  storage_.clear(); // storage_.reserve(other.storage_.size());
                    // DO NOT DO THIS!  calls unimplemented default constructor of vsol_point_3d; causes memory fault on Alpha
  for (unsigned int i=0;i<other.storage_.size();++i)
    storage_.push_back(other.storage_[i]); // smart pointers do refcounting
}

//***************************************************************************
// Comparison
//***************************************************************************

//---------------------------------------------------------------------------
//: Has `this' the same points than `other' in the same order ?
//---------------------------------------------------------------------------
bool vsol_polyhedron::operator==(vsol_polyhedron const &other) const
{
  if (this==&other) return true;
  if (storage_.size()!=other.storage_.size()) return false;

  vsol_point_3d_sptr const& p=storage_[0]; // alias

  unsigned int j;
  for (j=0; j < storage_.size(); ++j)
    if (p==other.storage_[j]) break;
  if (j == storage_.size()) return false;

  for (unsigned int i=1; i < storage_.size(); ++i)
  {
    if (++j >= storage_.size()) j=0;
    if (storage_[i]!=other.storage_[j])
      return false;
  }
  return true;
}

//: spatial object equality

bool vsol_polyhedron::operator==(vsol_spatial_object_3d const& obj) const
{
  return
   obj.spatial_type() == vsol_spatial_object_3d::VOLUME &&
   ((vsol_volume_3d const&)obj).volume_type() == vsol_volume_3d::POLYHEDRON
  ? *this == (vsol_polyhedron const&) (vsol_volume_3d const&) obj
  : false;
}

//---------------------------------------------------------------------------
//: Compute the bounding box of `this'
//---------------------------------------------------------------------------
void vsol_polyhedron::compute_bounding_box(void)
{
  double xmin=storage_[0]->x();
  double ymin=storage_[0]->y();
  double zmin=storage_[0]->z();
  double xmax=xmin;
  double ymax=ymin;
  double zmax=zmin;

  for (unsigned int i=0;i<storage_.size();++i)
  {
    double x=storage_[i]->x();
    if      (x<xmin) xmin=x;
    else if (x>xmax) xmax=x;
    double y=storage_[i]->y();
    if      (y<ymin) ymin=y;
    else if (y>ymax) ymax=y;
    double z=storage_[i]->z();
    if      (z<zmin) zmin=z;
    else if (z>zmax) zmax=z;
  }

  if(_bounding_box==0)
    _bounding_box = new vsol_box_3d;
  _bounding_box->set_min_x(xmin);
  _bounding_box->set_max_x(xmax);
  _bounding_box->set_min_y(ymin);
  _bounding_box->set_max_y(ymax);
  _bounding_box->set_min_z(zmin);
  _bounding_box->set_max_z(zmax);
}

//---------------------------------------------------------------------------
//: Return the volume of `this'
//---------------------------------------------------------------------------
double vsol_polyhedron::volume(void) const
{
  // TO DO
  vcl_cerr << "Warning: vsol_polyhedron::volume() has not been implemented yet\n";
  return -1;
}

//---------------------------------------------------------------------------
//: Is `this' convex ?
//---------------------------------------------------------------------------
bool vsol_polyhedron::is_convex(void) const
{
  // A polyhedron is always convex since the point order is irrelevant
  return true;
}

//***************************************************************************
// Basic operations
//***************************************************************************

//---------------------------------------------------------------------------
//: Is `p' in `this' ?
//---------------------------------------------------------------------------
bool vsol_polyhedron::in(vsol_point_3d const& p) const
{
  // TO DO
  vcl_cerr << "Warning: vsol_polyhedron::in() has not been implemented yet\n";
  return false;
}
