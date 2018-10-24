#ifndef vsol_volume_3d_h_
#define vsol_volume_3d_h_
//*****************************************************************************
//:
//  \file
// \brief Abstract volume in a 3D space
//
// \author
// Peter Vanroose
//
// \verbatim
//  Modifications
//   2000/06/28 Peter Vanroose  First version
// \endverbatim
//*****************************************************************************

class vsol_volume_3d;
class vsol_polyhedron;
class vsol_mesh_3d; // not in this library to avoid dependencies (see brl/bseg/betr)
#include "vsol_volume_3d_sptr.h"
#include <vsol/vsol_spatial_object_3d.h>
#include <vsol/vsol_point_3d_sptr.h>

class vsol_volume_3d : public vsol_spatial_object_3d
{
 protected:
  enum vsol_volume_3d_type
  { VOLUME_NO_TYPE=0,
    POLYHEDRON,
    MESH,
    NUM_VOLUME_TYPES
  };
  virtual vsol_volume_3d_type volume_type(void) const { return vsol_volume_3d::VOLUME_NO_TYPE; }

 public:
  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  ~vsol_volume_3d() override = default;

  //***************************************************************************
  // Basic operations
  //***************************************************************************
  vsol_volume_3d* cast_to_volume() override { return this;}
  vsol_volume_3d const* cast_to_volume() const override { return this;}

  virtual vsol_polyhedron* cast_to_polyhedron() { return nullptr;}
  virtual vsol_polyhedron const* cast_to_polyhedron() const { return nullptr;}

  virtual vsol_mesh_3d* cast_to_mesh() { return nullptr;}
  virtual vsol_mesh_3d const* cast_to_mesh() const { return nullptr;}
  //---------------------------------------------------------------------------
  //: Is the point `p' inside `this' volume ?
  //---------------------------------------------------------------------------
  virtual bool in(vsol_point_3d_sptr const& p) const=0;

  //---------------------------------------------------------------------------
  //: Return the spatial type of `this'
  //---------------------------------------------------------------------------
  vsol_spatial_object_3d_type spatial_type(void) const override { return vsol_spatial_object_3d::VOLUME; }

  //---------------------------------------------------------------------------
  //: Return the volume of `this'
  //---------------------------------------------------------------------------
  virtual double volume(void) const=0;

  //: Return a platform independent string identifying the class
  std::string is_a() const override { return std::string("vsol_volume_3d"); }

  //: Return true if the argument matches the string identifying the class or any parent class
  virtual bool is_class(const std::string& cls) const { return cls==is_a(); }
};

#endif // vsol_volume_3d_h_
