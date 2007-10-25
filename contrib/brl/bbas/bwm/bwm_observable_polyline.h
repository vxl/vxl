// this is contrib/brl/bbas/bwm/bwm_observable_polyline.h

#ifndef bwm_observable_polyline_h_
#define bwm_observable_polyline_h_

//*****************************************************************************
//:
// \file
// \brief An observable polyline object in 3D
//
// \author Gamze Tunali (gtunali@brown.edu)
// \date   2007/10/25
//
// \verbatim
//  Modifications
// \endverbatim
//*****************************************************************************

#include "bwm_observable.h"

#include <vcl_string.h>

#include <vsol/vsol_point_3d_sptr.h>
#include <vsol/vsol_polyline_3d_sptr.h>
#include <vsol/vsol_polyline_3d.h>

class bwm_observable_polyline : public bwm_observable
{
public:
  bwm_observable_polyline() : object_(0) {}
  ~bwm_observable_polyline() {}

  vcl_string type_name() const { return "bwm_observable_polyline"; }

  bwm_observable_polyline(vcl_vector<vsol_point_3d_sptr> const& vertices) 
  { object_ = new vsol_polyline_3d(vertices); }

private:
  vsol_polyline_3d_sptr object_;
};

#endif