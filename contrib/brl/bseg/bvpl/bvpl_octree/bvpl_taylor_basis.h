// This is bvpl_taylor_basis.h
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
#include <boxm/boxm_scene_base.h>

class bvpl_taylor_basis
{
  
public:
  bvpl_taylor_basis(boxm_scene_base_sptr scene_base, bvpl_taylor_basis_loader loader, vcl_string path_out):
  kernel_loader_(loader), scene_base_(scene_base), path_out_(path_out){}
  
  void apply_basis();
  
  //: Computes the sum of square errors between the scene and the taylor approximation for a percentage of voxels
  // This function operates on a block specified by an index
//  void compute_reconstruction_error(int block_i, int block_j, int block_k,
//                                    vcl_string taylor_scene_path,
//                                    float percentage,
//                                    vcl_string output_scene_path);
  
protected:
  
  
  bvpl_taylor_basis_loader kernel_loader_;
  boxm_scene_base_sptr scene_base_;
  vcl_string path_out_;
};
#endif
