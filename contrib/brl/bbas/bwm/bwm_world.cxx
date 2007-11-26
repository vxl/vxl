#include "bwm_world.h"

bwm_world* bwm_world::instance_ = 0;

bwm_world* bwm_world::instance() {
  if (!instance_) 
    instance_ = new bwm_world();
  return bwm_world::instance_;
}