#ifndef vsol_region_2d_h_
#define vsol_region_2d_h_
//*****************************************************************************
//:
// \file
// \brief  Region of a 2D space
//
// \author
// François BERTEL
//
// \verbatim
// Modifications
// 2000/05/02 François BERTEL Creation
// 2000/06/17 Peter Vanroose  Implemented all operator==()s and type info
// 2003/01/08 Peter Vanroose  Added pure virtual is_convex()
// 2003/11/06 Amir Tamrakar   Added safe cast methods to polygon_2d
// \endverbatim
//*****************************************************************************

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vsol/vsol_spatial_object_2d.h>

class vsol_polygon_2d;

class vsol_region_2d : public vsol_spatial_object_2d
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

  //---------------------------------------------------------------------------
  // Destructor
  //---------------------------------------------------------------------------
  virtual ~vsol_region_2d();

  //***************************************************************************
  // virtuals of vsol_spatial_object_2d
  //***************************************************************************

  virtual vsol_region_2d* cast_to_region(void) { return this; }
  virtual vsol_region_2d const* cast_to_region(void) const { return this; }

  virtual vsol_polygon_2d* cast_to_polygon_2d(void) { return 0; }
  virtual vsol_polygon_2d const* cast_to_polygon_2d(void) const { return 0; }

  //***************************************************************************
  // Status report
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return the area of `this'
  //---------------------------------------------------------------------------
  virtual double area(void) const=0;

  //---------------------------------------------------------------------------
  //: Return true if this region is convex
  //---------------------------------------------------------------------------
  virtual bool is_convex(void) const=0;
};

#endif // #ifndef vsol_region_2d_h_
