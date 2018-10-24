// This is gel/vsol/vsol_surface_3d.h
#ifndef vsol_surface_3d_h_
#define vsol_surface_3d_h_
//*****************************************************************************
//:
// \file
// \brief Abstract surface in 3D space
//
// \author Francois BERTEL
// \date   2000-05-04
//
// \verbatim
//  Modifications
//   2000-05-04 Francois BERTEL Creation
//   2001-07-03 Peter Vanroose  Replaced vnl_double_3 by vgl_vector_3d
//   2004-09-06 Peter Vanroose  Added safe cast methods to region_3d
//   2004-10-09 Peter Vanroose  Inlined all 1-line methods in class decl
// \endverbatim
//*****************************************************************************

#include <vsol/vsol_spatial_object_3d.h>
#include <vsol/vsol_point_3d_sptr.h>
#include <vgl/vgl_vector_3d.h>
class vsol_region_3d;

class vsol_surface_3d : public vsol_spatial_object_3d
{
 public:
  //***************************************************************************
  // Initialization
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  ~vsol_surface_3d() override = default;

  //***************************************************************************
  // virtuals of vsol_spatial_object_3d
  //***************************************************************************

  vsol_surface_3d* cast_to_surface(void) override { return this; }
  vsol_surface_3d const* cast_to_surface(void) const override { return this; }

  vsol_region_3d *cast_to_region(void) override {return nullptr;}
  vsol_region_3d const* cast_to_region(void) const override {return nullptr;}

  //***************************************************************************
  // Basic operations
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Is `p' in `this' ?
  //---------------------------------------------------------------------------
  virtual bool in(const vsol_point_3d_sptr &p) const=0;

  //---------------------------------------------------------------------------
  //: Return the unit normal vector at point `p'.
  //  REQUIRE: in(p)
  //---------------------------------------------------------------------------
  virtual vgl_vector_3d<double> normal_at_point(const vsol_point_3d_sptr &p) const=0;

  //: Return a platform independent string identifying the class
  std::string is_a() const override { return std::string("vsol_surface_3d"); }

  //: Return true if the argument matches the string identifying the class or any parent class
  virtual bool is_class(const std::string& cls) const { return cls==is_a(); }
};

#endif // vsol_surface_3d_h_
