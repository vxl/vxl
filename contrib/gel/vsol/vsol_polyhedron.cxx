// This is gel/vsol/vsol_polyhedron.cxx
#include "vsol_polyhedron.h"
//:
// \file

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
  storage_.clear();
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
void vsol_polyhedron::compute_bounding_box(void) const
{
  set_bounding_box(storage_[0]->x(),
                   storage_[0]->y(),
                   storage_[0]->z());
  for (unsigned int i=1;i<storage_.size();++i)
    add_to_bounding_box(storage_[i]->x(),
                        storage_[i]->y(),
                        storage_[i]->z());
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
bool vsol_polyhedron::in(vsol_point_3d const& ) const
{
  // TODO
  vcl_cerr << "Warning: vsol_polyhedron::in() has not been implemented yet\n";
  return false;
}

//---------------------------------------------------------------------------
//: Destructor
//---------------------------------------------------------------------------
vsol_polyhedron::~vsol_polyhedron()
{
  storage_.clear();
}

//---------------------------------------------------------------------------
//: Return vertex `i'
//  REQUIRE: valid_index(i)
//---------------------------------------------------------------------------
vsol_point_3d_sptr vsol_polyhedron::vertex(int i) const
{
  assert(valid_index(i));
  return storage_[i];
}

void vsol_polyhedron::describe(vcl_ostream &strm, int blanking) const
{
  if (blanking < 0) blanking = 0; while (blanking--) strm << ' ';
  strm << "[vsol_polyhedron";
  for (unsigned int i=0; i<size(); ++i)
    strm << ' ' << *(vertex(i));
  strm << ']' << vcl_endl;
}
