#ifndef vsol_region_3d_h_
#define vsol_region_3d_h_
//*****************************************************************************
//:
// \file
// \brief  Region of a 3D space
//
// \author
// François BERTEL
//
// \verbatim
//  Modifications
//   2000/05/04 François BERTEL Creation
//   2000/06/17 Peter Vanroose  Implemented all operator==()s and type info
//   2003/01/08 Peter Vanroose  Added pure virtual is_convex()
//   2004/09/06 Peter Vanroose  Added safe cast methods to polygon_3d
//   2004/10/09 Peter Vanroose  Inlined all 1-line methods in class decl
// \endverbatim
//*****************************************************************************

#include <vsol/vsol_surface_3d.h>
class vsol_polygon_3d;

class vsol_region_3d : public vsol_surface_3d
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

  //---------------------------------------------------------------------------
  // Destructor
  //---------------------------------------------------------------------------
  virtual ~vsol_region_3d() {}

  //***************************************************************************
  // virtuals of vsol_spatial_object_3d
  //***************************************************************************

  virtual vsol_region_3d* cast_to_region(void) { return this; }
  virtual vsol_region_3d const* cast_to_region(void) const { return this; }

  virtual vsol_polygon_3d *cast_to_polygon(void) {return 0;}
  virtual vsol_polygon_3d const* cast_to_polygon(void) const {return 0;}

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

#endif // vsol_region_3d_h_
