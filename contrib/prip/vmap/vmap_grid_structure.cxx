// This is contrib/prip/vmap/vmap_grid_structure.cxx

#include "vmap_grid_structure.h"

  vmap_grid_structure::vmap_grid_structure(int arg_width, int arg_height)
  {
    initialise(arg_width, arg_height);
  }


  //: Initializes the vmap_grid_structure with "arg_width"*"arg_height" faces
  void vmap_grid_structure::initialise(int arg_width, int arg_height)
  {
    _width = arg_width; _height=arg_height ;
    _horizontalEdges = _width*(_height+1) ;
    _verticalEdges = _height*(_width+1) ;
    _nb_darts=2*(_horizontalEdges+_verticalEdges) ;
  }

