// This is gel/vsol/vsol_surface_3d.h
#ifndef vsol_surface_3d_h_
#define vsol_surface_3d_h_
//*****************************************************************************
//:
// \file
// \brief Abstract surface in 3D space
//
// \author François BERTEL
// \date   2000/05/04
//
// \verbatim
//  Modifications
//   2000/05/04 François BERTEL Creation
//   2001/07/03 Peter Vanroose  Replaced vnl_double_3 by vgl_vector_3d
//   2004/09/06 Peter Vanroose  Added safe cast methods to region_3d
//   2004/10/09 Peter Vanroose  Inlined all 1-line methods in class decl
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
  virtual ~vsol_surface_3d() {}

  //***************************************************************************
  // virtuals of vsol_spatial_object_3d
  //***************************************************************************

  virtual vsol_surface_3d* cast_to_surface(void) { return this; }
  virtual vsol_surface_3d const* cast_to_surface(void) const { return this; }

  virtual vsol_region_3d *cast_to_region(void) {return 0;}
  virtual vsol_region_3d const* cast_to_region(void) const {return 0;}

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
  virtual vcl_string is_a() const { return vcl_string("vsol_surface_3d"); }

  //: Return true if the argument matches the string identifying the class or any parent class
  virtual bool is_class(const vcl_string& cls) const { return cls==is_a(); }
};

#endif // vsol_surface_3d_h_
