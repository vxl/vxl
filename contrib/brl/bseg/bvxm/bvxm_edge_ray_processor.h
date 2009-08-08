// This is brl/bseg/bvxm/bvxm_edge_ray_processor.h
#ifndef bvxm_edge_ray_processor_h_
#define bvxm_edge_ray_processor_h_
//:
// \file
// \brief A class for an edge_ray processor
//
// \author Joseph Mundy
// \date August 7, 2009
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
#include <bvxm/bvxm_voxel_world_sptr.h>
#include <bvxm/bvxm_image_metadata.h>
#include <vcl_string.h>
class  bvxm_edge_ray_processor
{
 public:

  bvxm_edge_ray_processor(): world_(0){}
  bvxm_edge_ray_processor(bvxm_voxel_world_sptr& world): world_(world){}

  void set_voxel_world(bvxm_voxel_world_sptr& world){world_ = world;}

  // initialize the voxel grid for edges
  bool init_edges(unsigned scale_idx=0);

  //: update voxel grid for edges with data from image/camera pair and return the edge probability density of pixel values
  bool update_edges(bvxm_image_metadata const& metadata, unsigned scale_idx=0);

  //: generate the expected edge image from the specified viewpoint. the expected image should be allocated by the caller.
  bool expected_edge_image(bvxm_image_metadata const& camera,vil_image_view_base_sptr &expected, float n_normal, unsigned scale_idx=0);

  //: save the edge probability grid in a ".raw" format readable by Drishti volume rendering software
  bool save_edges_raw(vcl_string filename, float n_normal, unsigned scale_idx=0);

  //: save the edge probability grid as a 3-d tiff image
  bool save_edges_vff(vcl_string filename, unsigned scale_idx=0);


 private:
  bvxm_voxel_world_sptr world_;
};

#endif // bvxm_edge_ray_processor_h_
