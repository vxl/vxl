#ifndef vsol_volume_3d_h_
#define vsol_volume_3d_h_
//*****************************************************************************
//:
//  \file
// \brief Abstract volume in a 3D space
//
// \author
// Peter Vanroose
//
// \verbatim
//  Modifications
//   2000/06/28 Peter Vanroose  First version
// \endverbatim
//*****************************************************************************

class vsol_volume_3d;
class vsol_polyhedron;
#include "vsol_volume_3d_sptr.h"
#include <vsol/vsol_spatial_object_3d.h>
#include <vsol/vsol_point_3d_sptr.h>

class vsol_volume_3d : public vsol_spatial_object_3d
{
 protected:
  enum vsol_volume_3d_type
  { VOLUME_NO_TYPE=0,
    POLYHEDRON,
    NUM_VOLUME_TYPES
  };
  vsol_volume_3d_type volume_type(void) const { return vsol_volume_3d::VOLUME_NO_TYPE; }

 public:
  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  virtual ~vsol_volume_3d() {}

  //***************************************************************************
  // Basic operations
  //***************************************************************************
  virtual vsol_volume_3d* cast_to_volume() { return this;}
  virtual vsol_volume_3d const* cast_to_volume() const { return this;}

  virtual vsol_polyhedron* cast_to_polyhedron() { return 0;}
  virtual vsol_polyhedron const* cast_to_polyhedron() const { return 0;}

  //---------------------------------------------------------------------------
  //: Is the point `p' inside `this' volume ?
  //---------------------------------------------------------------------------
  virtual bool in(vsol_point_3d_sptr const& p) const=0;

  //---------------------------------------------------------------------------
  //: Return the spatial type of `this'
  //---------------------------------------------------------------------------
  vsol_spatial_object_3d_type spatial_type(void) const { return vsol_spatial_object_3d::VOLUME; }

  //---------------------------------------------------------------------------
  //: Return the volume of `this'
  //---------------------------------------------------------------------------
  virtual double volume(void) const=0;

  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const { return vcl_string("vsol_volume_3d"); }

  //: Return true if the argument matches the string identifying the class or any parent class
  virtual bool is_class(const vcl_string& cls) const { return cls==is_a(); }
};

#endif // vsol_volume_3d_h_
