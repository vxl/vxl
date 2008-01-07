#include "bdgl_curve_region.h"
//-----------------------------------------------------------------------------
//:
// \file
// \author P.L. Bazin
//-----------------------------------------------------------------------------

#include <vdgl/vdgl_edgel.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_edgel_chain_sptr.h>
#include <vcl_cmath.h>

//-----------------------------------------------------------------------------
void bdgl_curve_region::init(vdgl_edgel_chain_sptr curve)
{
  vdgl_edgel ed;
  double dist;
  int N = (*curve).size();

  if (N==0){
    center_.set(0,0);
    radius_ = 0.0;
    return;
  }

  center_.set(0,0);
  for (int i = 0; i<N; i++) {
    ed = curve->edgel(i);
    center_.set(center_.x() + ed.get_x(), center_.y() + ed.get_y());
  }
  center_.set( center_.x()/(double)N , center_.y()/(double)N );

  radius_ = 0.0;
  for (int i = 0; i<N; i++) {
    ed = curve->edgel(i);
    dist = vcl_sqrt( (center_.x()-ed.get_x())*(center_.x()-ed.get_x())
      + (center_.y()-ed.get_y())*(center_.y()-ed.get_y()) );
    if (dist > radius_) radius_ = dist;
  }

  //vcl_cout<<" region: ["<<floor(center_.x())<<','<<floor(center_.y())<<"] "<<radius_<<'\n';

  return;
}

//-----------------------------------------------------------------------------
