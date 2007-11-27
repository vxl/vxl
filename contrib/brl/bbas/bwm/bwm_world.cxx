#include "bwm_world.h"

bwm_world* bwm_world::instance_ = 0;

bwm_world* bwm_world::instance() {
  if (!instance_) 
    instance_ = new bwm_world();
  return bwm_world::instance_;
}


void  bwm_world::set_world_pt(vgl_point_3d<double> const& pt)
{
  world_pt_ = pt; 
  vgl_vector_3d<double> normal(0, 0, 1);//z axis
  world_plane_ = vgl_plane_3d<double>(normal, pt);
}
