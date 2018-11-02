// This is brl/bbas/bwm/bwm_observable_polyline.h
#ifndef bwm_observable_polyline_h_
#define bwm_observable_polyline_h_
//*****************************************************************************
//:
// \file
// \brief An observable polyline object in 3D
// \author Gamze Tunali (gtunali@brown.edu)
// \date   2007-10-25
//*****************************************************************************

#include <iostream>
#include <string>
#include "bwm_observable.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vsol/vsol_point_3d_sptr.h>
#include <vsol/vsol_polyline_3d_sptr.h>
#include <vsol/vsol_polyline_3d.h>

class bwm_observable_polyline : public bwm_observable
{
 public:
  bwm_observable_polyline() : object_(nullptr) {}
  ~bwm_observable_polyline() {}

  std::string type_name() const { return "bwm_observable_polyline"; }

  bwm_observable_polyline(std::vector<vsol_point_3d_sptr> const& vertices)
  { object_ = new vsol_polyline_3d(vertices); }

 private:
  vsol_polyline_3d_sptr object_;
};

#endif
