// This is brl/bseg/bvxm/pro/processes/bvxm_heightmap_process.h
#ifndef bvxm_heightmap_process_h_
#define bvxm_heightmap_process_h_
//:
// \file
// \brief A class for generating a height map from a given camera viewpoint
//        -  Inputs:
//             * bvxm_voxel_world_sptr
//             * vpgl_camera_double_sptr
//
//        -  Output:
//             * vil_image_view_base_sptr   generated image
//
// \author  Gamze D. Tunali
// \date    Apr 17, 2008
// \verbatim
//  Modifications
//   Isabel Restrepo - Jan 27, 2009 - converted process-class to functions which is the new design for bvxm_processes.
//   Peter Vanroose  - Jul 10, 2009 - split into .h and .cxx
//   Yi Dong         - Apr 21, 2014 - add an option to output an negated depth map --> eventual output is an absolute height map
// \endverbatim

#include <bprb/bprb_func_process.h>

//: global variables
namespace bvxm_heightmap_process_globals
{
  constexpr unsigned n_inputs_ = 5;
  constexpr unsigned n_outputs_ = 1;
}

//: set input and output types
bool bvxm_heightmap_process_cons(bprb_func_process& pro);

// generates a height map from a given camera viewpoint
bool bvxm_heightmap_process(bprb_func_process& pro);

#endif // bvxm_heightmap_process_h_
