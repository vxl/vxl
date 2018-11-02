// this is contrib/brl/bbas/bwm/bwm_observable_point.h

#ifndef bwm_observable_point_h_
#define bwm_observable_point_h_

//*****************************************************************************
//:
// \file
// \brief An observable point object in 3D
//
// \author Gamze Tunali (gtunali@brown.edu)
// \date   2007-10-25
//
// \verbatim
//  Modifications
// \endverbatim
//*****************************************************************************

#include <iostream>
#include <string>
#include "bwm_observable.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vgl/vgl_point_3d.h>
#include <vsol/vsol_point_3d_sptr.h>
#include <vsol/vsol_point_3d.h>

class bwm_observable_point : public bwm_observable
{
 public:
  bwm_observable_point() : object_(nullptr) {}
  ~bwm_observable_point() {}

  std::string type_name() const { return "bwm_observable_point"; }

  bwm_observable_point(const vsol_point_3d_sptr p) { object_ = p; }

  bwm_observable_point(const vgl_point_3d<double>& p) { object_ = new vsol_point_3d(p); }

  bwm_observable_point(bwm_observable_point& p)
  : bwm_observable(), object_(new vsol_point_3d(p.object_->get_p())) {}

 private:
  vsol_point_3d_sptr object_;
};

#endif
