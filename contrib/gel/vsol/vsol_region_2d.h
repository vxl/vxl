#ifndef vsol_region_2d_h_
#define vsol_region_2d_h_
//*****************************************************************************
//:
// \file
// \brief  Region of a 2D space
//
// \author
// Francois BERTEL
//
// \verbatim
//  Modifications
//   2000/05/02 Francois BERTEL Creation
//   2000/06/17 Peter Vanroose  Implemented all operator==()s and type info
//   2003/01/08 Peter Vanroose  Added pure virtual is_convex()
//   2003/11/06 Amir Tamrakar   Added safe cast methods to polygon_2d
//   2004/10/09 Peter Vanroose  Inlined all 1-line methods in class decl
// \endverbatim
//*****************************************************************************

#include <vsol/vsol_spatial_object_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
class vsol_polygon_2d;
class vsol_poly_set_2d;

class vsol_region_2d : public vsol_spatial_object_2d
{
 protected:
  enum vsol_region_2d_type
  { REGION_NO_TYPE=0,
    POLYGON,
    POLYGON_SET,
    NUM_REGION_TYPES
  };

  //---------------------------------------------------------------------------
  //: Return the region type
  //---------------------------------------------------------------------------
  virtual vsol_region_2d_type region_type(void) const { return vsol_region_2d::REGION_NO_TYPE; }

 public:
  //---------------------------------------------------------------------------
  //: Return the spatial type
  //---------------------------------------------------------------------------
  vsol_spatial_object_2d_type spatial_type(void) const override { return vsol_spatial_object_2d::REGION; }

  //***************************************************************************
  // Initialization
  //***************************************************************************

  //---------------------------------------------------------------------------
  // Destructor
  //---------------------------------------------------------------------------
  ~vsol_region_2d() override = default;

  //***************************************************************************
  // virtuals of vsol_spatial_object_2d
  //***************************************************************************

  vsol_region_2d* cast_to_region(void) override { return this; }
  vsol_region_2d const* cast_to_region(void) const override { return this; }

  virtual vsol_polygon_2d* cast_to_polygon(void) { return nullptr; }
  virtual vsol_polygon_2d const* cast_to_polygon(void) const { return nullptr; }

  virtual vsol_poly_set_2d* cast_to_poly_set(void) { return nullptr; }
  virtual vsol_poly_set_2d const* cast_to_poly_set(void) const { return nullptr; }
  //***************************************************************************
  // Status report
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return the area of `this'
  //---------------------------------------------------------------------------
  virtual double area(void) const=0;

  //---------------------------------------------------------------------------
  //: Return the centroid of `this'
  //---------------------------------------------------------------------------
  virtual vsol_point_2d_sptr centroid(void) const=0;

  //---------------------------------------------------------------------------
  //: Return true if this region is convex
  //---------------------------------------------------------------------------
  virtual bool is_convex(void) const=0;

  //: Return a platform independent string identifying the class
  std::string is_a() const override { return std::string("vsol_region_2d"); }

  //: Return true if the argument matches the string identifying the class or any parent class
  virtual bool is_class(const std::string& cls) const { return cls==is_a(); }
};

#endif // vsol_region_2d_h_
