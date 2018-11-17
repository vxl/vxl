// This is bvpl_pca_error_scenes.h
#ifndef bvpl_pca_error_scenes_h
#define bvpl_pca_error_scenes_h

//:
// \file
// \brief A class to hold the smartpointers to pca error scenes
// \author Isabel Restrepo mir@lems.brown.edu
// \date  15-Feb-2011.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bvpl/kernels/bvpl_taylor_basis_factory.h>
#include <boxm/boxm_scene_base.h>
#include <vsl/vsl_binary_io.h>

// Forward declarations
class bvpl_pca_error_scenes;

typedef vbl_smart_ptr<bvpl_pca_error_scenes> bvpl_pca_error_scenes_sptr;

//: A class to hold the smartpointers to response scenes and their corresponding names
class bvpl_pca_error_scenes : public vbl_ref_count
{
public:

  bvpl_pca_error_scenes(const boxm_scene_base_sptr& data_scene_base, const std::string& taylor_path, unsigned dim);

  //: Return the error scene associated with a number of components used for reconstruction
  boxm_scene_base_sptr get_scene(unsigned ncomponent) {return scenes_[ncomponent];}

private:
  std::vector<boxm_scene_base_sptr> scenes_;

};
#endif
