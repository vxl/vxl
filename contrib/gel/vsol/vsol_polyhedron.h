#ifndef vsol_polyhedron_h_
#define vsol_polyhedron_h_
//*****************************************************************************
//
// .NAME vsol_polyhedron - Polyhedral volume in 3D space
// .LIBRARY	vsol
// .INCLUDE	vsol/vsol_polyhedron.h
// .FILE	vsol_polyhedron.cxx
//
// .SECTION Description
//   Representation of an arbitrary polyhedral volume, i.e., a volume that
//   is bounded by any set of flat polygons.
//   A sufficient minimal representation for this is the set of corner points
//   together with the polygonal faces they form.
//
// .SECTION Author
//   Peter Vanroose, 5 July 2000.
//
// .SECTION Modifications
//*****************************************************************************

class vsol_polyhedron;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include "vsol_polyhedron_ref.h"
#include "vsol_volume_3d.h"

#include "vsol_point_3d_ref.h"
#include <vcl_vector.h>

class vsol_polyhedron
  :public vsol_volume_3d
{
  //***************************************************************************
  // Initialization
  //***************************************************************************
public:
  //---------------------------------------------------------------------------
  //: Constructor from a vcl_vector (i.e., a list of points)
  //: REQUIRE: new_vertices.size()>=4
  //---------------------------------------------------------------------------
  explicit vsol_polyhedron(vsol_point_list_3d const& new_vertices);

  //---------------------------------------------------------------------------
  //: Copy constructor
  //---------------------------------------------------------------------------
  vsol_polyhedron(vsol_polyhedron const& other);

  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  virtual ~vsol_polyhedron() {}

  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  //: See Prototype pattern
  //---------------------------------------------------------------------------
  virtual vsol_spatial_object_3d* clone(void) const { return new vsol_polyhedron(*this); }

  //***************************************************************************
  // Access
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return vertex `i'
  //: REQUIRE: valid_index(i)
  //---------------------------------------------------------------------------
  vsol_point_3d_ref vertex(int i) const { return valid_index(i)?storage_[i]:(vsol_point_3d_ref)0; }

  //***************************************************************************
  // Comparison
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Is `this' spanning the same the same volume than `other' ?
  //---------------------------------------------------------------------------
  virtual bool operator==(vsol_polyhedron const& other) const;
  virtual bool operator==(vsol_spatial_object_3d const& obj) const; // virtual of vsol_spatial_object_3d

  //---------------------------------------------------------------------------
  //: Negation of operator==
  //---------------------------------------------------------------------------
  bool operator!=(vsol_polyhedron const& o) const { return !operator==(o); }

  //***************************************************************************
  // Status report
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return the volume type of a polyhedron.  Its spatial type is a VOLUME.
  //---------------------------------------------------------------------------
  vsol_volume_3d_type volume_type(void)const{return vsol_volume_3d::POLYHEDRON;}

  //---------------------------------------------------------------------------
  //: Compute the bounding box of `this'
  //---------------------------------------------------------------------------
  virtual vsol_box_3d_ref compute_bounding_box(void) const;

  //---------------------------------------------------------------------------
  //: Return the number of vertices
  //---------------------------------------------------------------------------
  int num_vertices(void) const { return storage_.size(); }

  //---------------------------------------------------------------------------
  //: Return the volume of `this'
  //---------------------------------------------------------------------------
  virtual double volume(void) const;

  //---------------------------------------------------------------------------
  //: Is `this' convex ?
  //---------------------------------------------------------------------------
  virtual bool is_convex(void) const;

  //---------------------------------------------------------------------------
  //: Is `i' a valid index for the list of vertices ?
  //---------------------------------------------------------------------------
  bool valid_index(int i) const { return i>=0 && i<storage_.size(); }

  //***************************************************************************
  // Basic operations
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Is `p' in `this' ?
  //---------------------------------------------------------------------------
  virtual bool in(vsol_point_3d const& p) const;

  //***************************************************************************
  // Implementation
  //***************************************************************************
protected:
  //---------------------------------------------------------------------------
  //: Default constructor. Do nothing. Just to enable inherance.
  //---------------------------------------------------------------------------
  vsol_polyhedron(void) {}

  //---------------------------------------------------------------------------
  // Description: List of vertices
  //---------------------------------------------------------------------------
  vsol_point_list_3d storage_;
};

#endif // vsol_polyhedron_h_
