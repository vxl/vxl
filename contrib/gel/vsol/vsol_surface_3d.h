#ifndef VSOL_SURFACE_3D_H
#define VSOL_SURFACE_3D_H
//*****************************************************************************
//
// .NAME vsol_surface_3d - Abstract surface in a 3D space
// .LIBRARY vsol
// .INCLUDE vsol/vsol_surface_3d.h
// .FILE    vsol/vsol_surface_3d.cxx
//
// .SECTION Author
// François BERTEL
//
// .SECTION Modifications
// 2000/05/04 François BERTEL Creation
//*****************************************************************************

class vsol_surface_3d;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vsol/vsol_surface_3d_ref.h>
#include <vsol/vsol_spatial_object_3d.h>

#include <vsol/vsol_point_3d_ref.h>
#include <vnl/vnl_vector_fixed.h>

class vsol_surface_3d
  :public vsol_spatial_object_3d
{
  //***************************************************************************
  // Initialization
  //***************************************************************************
public:
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
  virtual bool in(const vsol_point_3d_ref &p) const=0;

  //---------------------------------------------------------------------------
  //: Return the unit normal vector at point `p'. Have to be deleted manually
  //: REQUIRE: in(p)
  //---------------------------------------------------------------------------
  virtual vnl_vector_fixed<double,3> *
  normal_at_point(const vsol_point_3d_ref &p) const=0;
};

#endif // #ifndef VSOL_SURFACE_3D_H
