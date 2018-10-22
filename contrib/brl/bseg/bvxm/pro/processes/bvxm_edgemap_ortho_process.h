// This is brl/bseg/bvxm/pro/processes/bvxm_edgemap_ortho_process.h
#ifndef bvxm_edgemap_ortho_process_h_
#define bvxm_edgemap_ortho_process_h_
//:
// \file
// \brief A class for generating an orthographic edge map
//        creates an orthographic vpgl_geo_camera first, then renders the edge map using this camera
//        WARNING: this process assumes that the world is compass-alinged
//
//        -  Inputs:
//             * bvxm_voxel_world_sptr
//
//        -  Output:
//             * vil_image_view_base_sptr   generated image
//
// \author  Ozge C. Ozcanli
// \date    Aug 07, 2013
// \verbatim
//  Modifications
// \endverbatim

#include <bprb/bprb_func_process.h>

// generate an edge map with an ortho camera - generate a vpgl_geo_camera for the scene
//: global variables
namespace bvxm_edgemap_ortho_process_globals
{
  constexpr unsigned n_inputs_ = 2;
  constexpr unsigned n_outputs_ = 4;
}

//: set input and output types
bool bvxm_edgemap_ortho_process_cons(bprb_func_process& pro);

// generates an edge map using an ortho camera constructed from the scene bounding box, GSD of the image is 1 meter
bool bvxm_edgemap_ortho_process(bprb_func_process& pro);

#endif // bvxm_edgemap_ortho_process_h_
