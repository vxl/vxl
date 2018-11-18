// This is brl/bseg/bvpl/bvpl_octree/bvpl_taylor_basis.h
#ifndef bvpl_taylor_basis_h
#define bvpl_taylor_basis_h

//:
// \file
// \brief A class with utilities to apply taylor kernels to a scene
// \author Isabel Restrepo mir@lems.brown.edu
// \date  31-Jan-2011.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bvpl/kernels/bvpl_taylor_basis_factory.h>
#include "bvpl_taylor_scenes_map.h"
#include <boxm/boxm_scene_base.h>


class bvpl_taylor_basis
{

public:
  //  bvpl_taylor_basis(boxm_scene_base_sptr scene_base, bvpl_taylor_basis_loader loader, std::string path_out):
  //  kernel_loader_(loader), scene_base_(scene_base), path_out_(path_out){}
  //
  static bool assemble_basis(const bvpl_taylor_scenes_map_sptr& taylor_scenes,int block_i, int block_j, int block_k);

  //: Computes the square errors between the scene and the taylor approximation at every voxel
  // This function operates on a block specified by an index
  static bool compute_approximation_error(const boxm_scene_base_sptr& data_scene_base,
                                          const boxm_scene_base_sptr& basis_scene_base,
                                          const boxm_scene_base_sptr& error_scene_base,
                                          const bvpl_taylor_basis_loader& loader,
                                          int block_i, int block_j, int block_k, double cell_length);

  //: Sums the square errors for a percenatge of random samples
  static double sum_errors(const boxm_scene_base_sptr& error_scene_base,
                           int block_i, int block_j, int block_k, unsigned long tree_nsamples);

protected:

  //
  //  bvpl_taylor_basis_loader kernel_loader_;
  //  boxm_scene_base_sptr scene_base_;
  //  std::string path_out_;
};

#endif
