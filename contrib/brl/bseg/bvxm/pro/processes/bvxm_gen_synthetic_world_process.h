//This is brl/bseg/bvxm/pro/processes/bvxm_gen_synthetic_world_process.h
#ifndef bvxm_gen_synthetic_world_process_h_
#define bvxm_gen_synthetic_world_process_h_
//:
// \file
// \brief A class for generating a synthetic bvxm_voxel_world.
//
// Adapted from bvxm_test_gen_synthetic_world
//
// \author Isabel Restrepo
// \date Apr 03, 2008
// \verbatim
//  Modifications
//   Ozge C Ozcanli  - added parameters
//   Isabel Restrepo - Jan 27, 2009 - converted process-class to functions which is the new design for bvxm_processes.
//   Isabel Restrepo - 3/10/09 - Moved local functions and variables to processes/bvxm_synth_world_generator.
//   Peter Vanroose  - Jul 10, 2009 - split into .h and .cxx
// \endverbatim

#include <bprb/bprb_func_process.h>

//:set input and output types
bool bvxm_gen_synthetic_world_process_cons(bprb_func_process& pro);

//:generates a synthetic world
bool bvxm_gen_synthetic_world_process(bprb_func_process& pro);

#endif // bvxm_gen_synthetic_world_process_h_
