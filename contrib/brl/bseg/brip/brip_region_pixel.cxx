#include <iostream>
#include "brip_region_pixel.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

std::ostream& operator<<(std::ostream& s, brip_region_pixel const& sd)
{
  return
  s << "[location:(" << sd.location_.x() << ' ' << sd.location_.y()
    << ") nearest:(" << sd.nearest_.x() << ' ' << sd.nearest_.y()
    << ") cost:" << sd.cost_
    << " count:" << sd.count_
    << " label:" << sd.label_
    << " dist:"  << sd.dist_ << ']';
}
