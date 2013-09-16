// This is brl/bseg/bvxm/pro/processes/bvxm_heightmap_ortho_process.h
#ifndef bvxm_heightmap_ortho_process_h_
#define bvxm_heightmap_ortho_process_h_
//:
// \file
// \brief A class for generating an orthographic height map
//        creates an orthographic vpgl_geo_camera first, then renders the height map using this camera
//        WARNING: this process assumes that the world is compass-alinged 
//
//        -  Inputs:
//             * bvxm_voxel_world_sptr
//
//        -  Output:
//             * vil_image_view_base_sptr   generated image
//
// \author  Ozge C. Ozcanli 
// \date    Apr 03, 2013
// \verbatim
//  Modifications
// \endverbatim

#include <bprb/bprb_func_process.h>

// generate a heightmap with an ortho camera - generate a vpgl_geo_camera for the scene 
//: global variables
namespace bvxm_heightmap_ortho_process_globals
{
  const unsigned n_inputs_ = 1;
  const unsigned n_outputs_ = 2;
}

//: set input and output types
bool bvxm_heightmap_ortho_process_cons(bprb_func_process& pro);

// generates a height map from a given camera viewpoint
bool bvxm_heightmap_ortho_process(bprb_func_process& pro);

#endif // bvxm_heightmap_ortho_process_h_
