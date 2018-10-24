#ifndef vsol_region_3d_h_
#define vsol_region_3d_h_
//*****************************************************************************
//:
// \file
// \brief  Region of a 3D space
//
// \author
// Francois BERTEL
//
// \verbatim
//  Modifications
//   2000/05/04 Francois BERTEL Creation
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
 protected:
  enum vsol_region_3d_type
  { REGION_NO_TYPE=0,
    POLYGON,
    NUM_REGION_TYPES
  };

  //---------------------------------------------------------------------------
  //: Return the region type
  //---------------------------------------------------------------------------
  virtual vsol_region_3d_type region_type(void) const { return vsol_region_3d::REGION_NO_TYPE; }

 public:
  //---------------------------------------------------------------------------
  //: Return the spatial type
  //---------------------------------------------------------------------------
  vsol_spatial_object_3d_type spatial_type(void) const override { return vsol_spatial_object_3d::REGION; }

  //***************************************************************************
  // Initialization
  //***************************************************************************

  //---------------------------------------------------------------------------
  // Destructor
  //---------------------------------------------------------------------------
  ~vsol_region_3d() override = default;

  //***************************************************************************
  // virtuals of vsol_spatial_object_3d
  //***************************************************************************

  vsol_region_3d* cast_to_region(void) override { return this; }
  vsol_region_3d const* cast_to_region(void) const override { return this; }

  virtual vsol_polygon_3d *cast_to_polygon(void) {return nullptr;}
  virtual vsol_polygon_3d const* cast_to_polygon(void) const {return nullptr;}

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

  //: Return a platform independent string identifying the class
  std::string is_a() const override { return std::string("vsol_region_3d"); }

  //: Return true if the argument matches the string identifying the class or any parent class
  bool is_class(const std::string& cls) const override
  { return cls==is_a() || vsol_surface_3d::is_class(cls); }
};

#endif // vsol_region_3d_h_
