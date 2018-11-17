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

#include <iostream>
#include <string>
#include <bvxm/bvxm_voxel_world_sptr.h>
#include <bvxm/bvxm_image_metadata.h>
#include <bvxm/bvxm_von_mises_tangent_processor.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class  bvxm_edge_ray_processor
{
 public:
  typedef bvxm_von_mises_tangent_processor<float>::dir_dist_t dir_dist_t;
  typedef bvxm_von_mises_tangent_processor<float>::pos_dist_t pos_dist_t;
  typedef bvxm_von_mises_tangent_processor<float>::pos_t pos_t;
  typedef bvxm_von_mises_tangent_processor<float>::dir_t dir_t;

  bvxm_edge_ray_processor(): world_(nullptr) {}
  bvxm_edge_ray_processor(bvxm_voxel_world_sptr& world): world_(world) {}

  void set_voxel_world(bvxm_voxel_world_sptr& world) { world_ = world; }

  // initialize the voxel grid for edges
  bool init_edges(unsigned scale_idx=0);

  //: update voxel grid for edges with data from image/camera pair and return the edge probability density of pixel values
  bool update_edges(bvxm_image_metadata const& metadata, unsigned scale_idx=0);

  //: update voxel grid for edges with data from image/camera pair and return the edge probability density of pixel values
  bool update_edges_with_Lidar_surface(bvxm_image_metadata const& metadata, unsigned scale_idx=0);

  //: generate the expected edge image from the specified viewpoint. the expected image should be allocated by the caller.
  bool expected_edge_image(bvxm_image_metadata const& camera,vil_image_view_base_sptr &expected, float n_normal, unsigned scale_idx=0);

  //: generate the expected edge image from the specified viewpoint. the expected image should be allocated by the caller.
  //  return the height of the voxel where the expected edge image probability is greatest
  bool expected_edge_image_and_heights(bvxm_image_metadata const& camera,vil_image_view_base_sptr &expected, vil_image_view_base_sptr &height_img, float n_normal, unsigned scale_idx=0);

  //: save the edge probability grid in a ".raw" format readable by Drishti volume rendering software
  bool save_edges_raw(const std::string& filename, float n_normal, unsigned scale_idx=0);

  //: save the edge probability grid as a 3-d tiff image
  bool save_edges_vff(const std::string& filename, unsigned scale_idx=0);

  //: initialize a von_mises edge tangent world from two images
  bool init_von_mises_edge_tangents(bvxm_image_metadata const& metadata0,
                                    bvxm_image_metadata const& metadata1,
                                    double initial_sd_ratio = 1.0,
                                    double initial_kappa = 10000.0,
                                    unsigned scale=0);

  //: update a von_mises edge tangent world.
  //  x0 interval is a radius in voxel units. cone angle (in radians) describes
  //  a region around the new tangent vector sample. Together these intervals
  //  are used to compute the probability of the sample.
  bool update_von_mises_edge_tangents(bvxm_image_metadata const& metadata,
                                      double x0_interval = 1.0,
                                      double cone_angle = 0.1,
                                      unsigned scale=0);

  //: Display edge tangent world as vrml
  void display_edge_tangent_world_vrml(std::string const& vrml_path);

  //: Display manually constructed ground truth
  void display_ground_truth(std::string const& gnd_truth_path,
                            std::string const& vrml_path);

 private:
  bvxm_voxel_world_sptr world_;
};

#endif // bvxm_edge_ray_processor_h_
