// this is contrib/brl/bbas/bwm/bwm_observable_point.h

#ifndef bwm_observable_point_h_
#define bwm_observable_point_h_

//*****************************************************************************
//:
// \file
// \brief An observable point object in 3D
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
#include <vsol/vsol_point_3d_sptr.h>
#include <vsol/vsol_point_3d.h>

class bwm_observable_point : public bwm_observable
{
public:
  bwm_observable_point() : object_(0) {}
  ~bwm_observable_point() {}

  vcl_string type_name() const { return "bwm_observable_point"; }

  bwm_observable_point(const vsol_point_3d_sptr p) { object_ = p; }

  bwm_observable_point(const vgl_point_3d<double>& p) { object_ = new vsol_point_3d(p); }

  bwm_observable_point(bwm_observable_point& p) { object_ = new vsol_point_3d(p.object_->get_p()); }

private:
  vsol_point_3d_sptr object_;
};

#endif