#ifndef VSOL_REGION_2D_H
#define VSOL_REGION_2D_H
//*****************************************************************************
//:
//  \file
// \brief  Region of a 2D space
//
// \author
// François BERTEL
//
// \verbatim
// Modifications
// 2000/06/17 Peter Vanroose  Implemented all operator==()s and type info
// 2000/05/02 François BERTEL Creation
// \endverbatim
//*****************************************************************************

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vsol/vsol_spatial_object_2d.h>

class vsol_region_2d
  :public vsol_spatial_object_2d
{
public:
  enum vsol_region_2d_type
  { REGION_NO_TYPE=0,
    POLYGON,
    NUM_REGION_TYPES
  };

  //---------------------------------------------------------------------------
  //: Return the spatial type
  //---------------------------------------------------------------------------
  vsol_spatial_object_2d_type spatial_type(void) const { return vsol_spatial_object_2d::REGION; }

  //---------------------------------------------------------------------------
  //: Return the region type
  //---------------------------------------------------------------------------
  virtual vsol_region_2d_type region_type(void) const { return vsol_region_2d::REGION_NO_TYPE; }

  //***************************************************************************
  // Initialization
  //***************************************************************************
public:
  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  virtual ~vsol_region_2d();

  //***************************************************************************
  // virtuals of vsol_spatial_object_2d
  //***************************************************************************

  virtual vsol_region_2d* cast_to_region(void) { return this; }
  virtual vsol_region_2d const* cast_to_region(void) const { return this; }

  //***************************************************************************
  // Status report
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return the area of `this'
  //---------------------------------------------------------------------------
  virtual double area(void) const=0;
};

#endif // #ifndef VSOL_REGION_2D_H
