#ifndef vsol_volume_3d_h_
#define vsol_volume_3d_h_
//*****************************************************************************
//
// .NAME vsol_volume_3d - Abstract volume in a 3D space
// .LIBRARY vsol
// .HEADER  vxl package
// .INCLUDE vsol/vsol_volume_3d.h
// .FILE    vsol_volume_3d.cxx
//
// .SECTION Author
// Peter Vanroose
//
// .SECTION Modifications
// 2000/06/28 Peter Vanroose  First version
//*****************************************************************************

class vsol_volume_3d;

#include "vsol_volume_3d_sptr.h"
#include "vsol_spatial_object_3d.h"

#include "vsol_point_3d_sptr.h"

class vsol_volume_3d
  :public vsol_spatial_object_3d
{
public:
  enum vsol_volume_3d_type
  { VOLUME_NO_TYPE=0,
    POLYHEDRON,
    NUM_VOLUME_TYPES
  };

  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  virtual ~vsol_volume_3d() {}

  //***************************************************************************
  // Basic operations
  //***************************************************************************
  vsol_volume_3d* cast_to_volume(void) { return this;}
  vsol_volume_3d const* cast_to_volume() const { return this;}

  //---------------------------------------------------------------------------
  //: Is the point `p' inside `this' volume ?
  //---------------------------------------------------------------------------
  virtual bool in(vsol_point_3d const& p) const=0;

  //---------------------------------------------------------------------------
  //: Return the spatial type of `this'
  //---------------------------------------------------------------------------
  vsol_spatial_object_3d_type spatial_type(void) const { return vsol_spatial_object_3d::VOLUME; }
  vsol_volume_3d_type volume_type(void) const { return vsol_volume_3d::VOLUME_NO_TYPE; }

  //---------------------------------------------------------------------------
  //: Return the volume of `this'
  //---------------------------------------------------------------------------
  virtual double volume(void) const=0;
};

#endif // vsol_volume_3d_h_
