// This is gel/vdgl/vdgl_interpolator.cxx

#include "vdgl_interpolator.h"
#include <vsol/vsol_point_2d.h>
#include <vgl/vgl_distance.h>

double vdgl_interpolator::distance_curve_to_point ( vsol_point_2d_sptr p )
{
  vsol_point_2d_sptr curve_point = this->closest_point_on_curve ( p );
  if ( curve_point.ptr() == nullptr )
    return 1e11;

  return vgl_distance ( p->get_p(), curve_point->get_p() );
}
