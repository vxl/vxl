// This is brl/bseg/bvxm/pro/processes/bvxm_orthorectify_process.h
#ifndef bvxm_orthorectify_process_h_
#define bvxm_orthorectify_process_h_
//:
// \file
// \brief Given an ortho height map of the scene, its camera, an input image and its camera, generate the ortho version of the input image
//        the rays are cast into volume using the camera of the input image, when the rays first hit a surface, the value of the image is written onto ortho image
//        The inputs ortho height map and ortho camera can be generated from an existing bvxm world using bvxm_heightmap_ortho_process.
//        However, note that this process works in general with any ortho height map and ortho camera.
//        It is not necessary to build a bvxm world, the world scene file in this process is only used to define a volume to cast the rays.
//
// \author  Ozge C. Ozcanli
// \date    Nov 20, 2013
// \verbatim
//  Modifications
// \endverbatim

#include <bprb/bprb_func_process.h>

//: global variables
namespace bvxm_orthorectify_process_globals
{
  constexpr unsigned n_inputs_ = 5;
  constexpr unsigned n_outputs_ = 1;
}

//: set input and output types
bool bvxm_orthorectify_process_cons(bprb_func_process& pro);

bool bvxm_orthorectify_process(bprb_func_process& pro);

#endif // bvxm_orthorectify_process_h_
