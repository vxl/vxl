#ifndef VSOL_REGION_2D_H
#define VSOL_REGION_2D_H
//*****************************************************************************
//
// .NAME vsol_region_2d -  Region of a 2D space
// .LIBRARY vsol
// .INCLUDE vsol/vsol_region_2d.h
// .FILE    vsol/vsol_region_2d.cxx
//
// .SECTION Author
// François BERTEL
//
// .SECTION Modifications
// 2000/06/17 Peter Vanroose  Implemented all operator==()s and type info
// 2000/05/02 François BERTEL Creation
//*****************************************************************************

class vsol_region_2d;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vsol/vsol_region_2d_sptr.h>
#include <vsol/vsol_spatial_object_2d.h>
#include <vnl/vnl_vector.h>

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
  // Status report
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return the area of `this'
  //---------------------------------------------------------------------------
  virtual double area(void) const=0;
};

#endif // #ifndef VSOL_REGION_2D_H
