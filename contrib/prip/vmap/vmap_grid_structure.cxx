// This is prip/vmap/vmap_grid_structure.cxx
#include "vmap_grid_structure.h"
//:
// \file

vmap_grid_structure::vmap_grid_structure(int arg_width, int arg_height)
{
  initialise(arg_width, arg_height);
}

//: Initializes the vmap_grid_structure with "arg_width"*"arg_height" faces
void vmap_grid_structure::initialise(int arg_width, int arg_height)
{
  width_ = arg_width; height_=arg_height ;
  horizontalEdges_ = width_*(height_+1) ;
  verticalEdges_ = height_*(width_+1) ;
  nb_darts_=2*(horizontalEdges_+verticalEdges_) ;
}
