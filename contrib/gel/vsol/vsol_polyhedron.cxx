#include "vsol_polyhedron.h"

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
// -- Constructor from a vcl_vector (not a geometric vector but a list of points)
// Require: new_vertices.size()>=3 and valid_vertices(new_vertices)
//---------------------------------------------------------------------------
vsol_polyhedron::vsol_polyhedron(vsol_point_list_3d const& new_vertices)
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
  storage_.clear(); storage_.reserve(storage_.size());
  for (int i=0;i<storage_.size();++i)
    storage_[i]=other.storage_[i]; // smart pointers do refcounting
}

//***************************************************************************
// Comparison
//***************************************************************************

//---------------------------------------------------------------------------
// -- Has `this' the same points than `other' in the same order ?
//---------------------------------------------------------------------------
bool vsol_polyhedron::operator==(vsol_polyhedron const &other) const
{
  if (this==&other) return true;
  if (storage_.size()!=other.storage_.size()) return false;

  vsol_point_3d_sptr const& p=storage_[0]; // alias

  int j=-1;
  while (++j < storage_.size())
    if (p==other.storage_[j]) break;
  if (j == storage_.size()) return false;

  for(int i=1; i < storage_.size(); ++i)
  {
    if (++j >= storage_.size()) j=0;
    if (storage_[i]!=other.storage_[j])
      return false;
  }
  return true;
}

// -- spatial object equality

bool vsol_polyhedron::operator==(const vsol_spatial_object_3d& obj) const
{
  return
   obj.spatial_type() == vsol_spatial_object_3d::VOLUME &&
   ((vsol_volume_3d const&)obj).volume_type() == vsol_volume_3d::POLYHEDRON
  ? *this == (vsol_polyhedron const&) (vsol_volume_3d const&) obj
  : false;
}

//---------------------------------------------------------------------------
// -- Compute the bounding box of `this'
//---------------------------------------------------------------------------
vsol_box_3d_sptr vsol_polyhedron::compute_bounding_box(void) const
{
  double xmin=storage_[0]->x();
  double ymin=storage_[0]->y();
  double zmin=storage_[0]->z();
  double xmax=xmin;
  double ymax=ymin;
  double zmax=zmin;

  for (int i=0;i<storage_.size();++i)
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

  vsol_box_3d* b = new vsol_box_3d;
  b->set_min_x(xmin);
  b->set_max_x(xmax);
  b->set_min_y(ymin);
  b->set_max_y(ymax);
  b->set_min_z(zmin);
  b->set_max_z(zmax);
  return b;
}

//---------------------------------------------------------------------------
// -- Return the volume of `this'
//---------------------------------------------------------------------------
double vsol_polyhedron::volume(void) const
{
  // TO DO
  vcl_cerr << "Warning: vsol_polyhedron::volume() has not been implemented yet\n";
  return -1;
}

//---------------------------------------------------------------------------
// -- Is `this' convex ?
//---------------------------------------------------------------------------
bool vsol_polyhedron::is_convex(void) const
{
  if (storage_.size()<=4) return true; // A tetrahedron is always convex

  vsol_point_3d_sptr const& p0 = storage_[0];
  vsol_point_3d_sptr const& p1 = storage_[1];
  vsol_point_3d_sptr const& p2 = storage_[2];
  vsol_point_3d_sptr const& p3 = storage_[3];

  for (int i=4;i<storage_.size();++i)
  {
    vsol_point_3d_sptr const& pi = storage_[i];
    // TODO
  }
  return false;
}

//***************************************************************************
// Basic operations
//***************************************************************************

//---------------------------------------------------------------------------
// -- Is `p' in `this' ?
//---------------------------------------------------------------------------
bool vsol_polyhedron::in(vsol_point_3d const& p) const
{
  // TO DO
  vcl_cerr << "Warning: vsol_polyhedron::in() has not been implemented yet\n";
  return false;
}
