#ifndef VSOL_REGION_3D_H
#define VSOL_REGION_3D_H
//*****************************************************************************
//:
//  \file
// \brief  Region of a 3D space
//
// \author
// François BERTEL
//
// \verbatim
// Modifications
// 2000/06/17 Peter Vanroose  Implemented all operator==()s and type info
// 2000/05/04 François BERTEL Creation
// \endverbatim
//*****************************************************************************

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vsol/vsol_surface_3d.h>

class vsol_region_3d
  :public vsol_surface_3d
{
public:
  enum vsol_region_3d_type
  { REGION_NO_TYPE=0,
    POLYGON,
    NUM_REGION_TYPES
  };

  //---------------------------------------------------------------------------
  //: Return the spatial type
  //---------------------------------------------------------------------------
  vsol_spatial_object_3d_type spatial_type(void) const { return vsol_spatial_object_3d::REGION; }

  //---------------------------------------------------------------------------
  //: Return the region type
  //---------------------------------------------------------------------------
  virtual vsol_region_3d_type region_type(void) const { return vsol_region_3d::REGION_NO_TYPE; }

  //***************************************************************************
  // Initialization
  //***************************************************************************
public:
  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  virtual ~vsol_region_3d();

  //***************************************************************************
  // virtuals of vsol_spatial_object_3d
  //***************************************************************************

  virtual vsol_region_3d* cast_to_region(void) { return this; }
  virtual vsol_region_3d const* cast_to_region(void) const { return this; }

  //***************************************************************************
  // Status report
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return the area of `this'
  //---------------------------------------------------------------------------
  virtual double area(void) const=0;
};

#endif // #ifndef VSOL_REGION_3D_H
