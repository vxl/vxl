// This is brl/bseg/bmrf/bmrf_curve_3d_builder.cxx
//:
// \file

#include "bmrf_curve_3d_builder.h"
#include "bmrf_node.h"


//: Constructor
bmrf_curve_3d_builder::bmrf_curve_3d_builder() 
 : network_(NULL)
{
}

//: Constructor
bmrf_curve_3d_builder::bmrf_curve_3d_builder(bmrf_network_sptr network)
 : network_(network)
{
}


//: Build The curves
vcl_set<bmrf_curvel_3d_sptr> 
bmrf_curve_3d_builder::build()
{
  vcl_set<bmrf_curvel_3d_sptr> curves;
  return curves;
}
