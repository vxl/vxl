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
// \endverbatim
//*****************************************************************************

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vsol/vsol_spatial_object_3d.h>
#include <vsol/vsol_point_3d_sptr.h>
#include <vgl/vgl_vector_3d.h>

class vsol_surface_3d : public vsol_spatial_object_3d
{
 public:
  //***************************************************************************
  // Initialization
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  virtual ~vsol_surface_3d();

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
};

#endif // vsol_surface_3d_h_
