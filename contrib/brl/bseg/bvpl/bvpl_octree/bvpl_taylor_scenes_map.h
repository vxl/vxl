// This is bvpl_taylor_scenes_map.h
#ifndef bvpl_taylor_scenes_map_h
#define bvpl_taylor_scenes_map_h

//:
// \file
// \brief A class to hold the smartpointers to response scenes and their corresponding names
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
class bvpl_taylor_scenes_map;

typedef vbl_smart_ptr<bvpl_taylor_scenes_map> bvpl_taylor_scenes_map_sptr;

//: A class to hold the smartpointers to response scenes and their corresponding names
class bvpl_taylor_scenes_map : public vbl_ref_count
{
public:

  bvpl_taylor_scenes_map(bvpl_taylor_basis_loader loader);

  boxm_scene_base_sptr get_scene(std::string basis) {return scenes_[basis];}

  const bvpl_taylor_basis_loader& loader() {return loader_ ;}

private:
  std::map<std::string, boxm_scene_base_sptr> scenes_;
  bvpl_taylor_basis_loader loader_;

};
#endif
