#ifndef brec_bg_pair_density_H_
#define brec_bg_pair_density_H_
//:
// \file
// \brief brec background pair model
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date October 01, 2008
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <utility>
#include "brec_pair_density.h"

#include <bvxm/bvxm_voxel_world.h>

#include <bsta/bsta_distribution.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture_fixed.h>
#include <bsta/bsta_gaussian_indep.h>


class brec_bg_pair_density : public brec_pair_density
{
 protected:
  typedef bvxm_voxel_traits<APM_MOG_RGB>::voxel_datatype rgb_mog_type;
  typedef bvxm_voxel_traits<APM_MOG_RGB>::obs_datatype rgb_obs_datatype;

  typedef bvxm_voxel_traits<APM_MOG_GREY>::voxel_datatype grey_mog_type;
  typedef bvxm_voxel_traits<APM_MOG_GREY>::obs_datatype grey_obs_datatype;

 public:
  ~brec_bg_pair_density() override = default;
  brec_bg_pair_density(bvxm_voxel_world_sptr w, vpgl_camera_double_sptr cam,
                       std::string voxel_type, unsigned bin, unsigned scale,
                       unsigned ni, unsigned nj)
  : brec_pair_density(), verbose(false), world_(w), cam_(cam), voxel_type_(std::move(voxel_type)),
    bin_(bin), scale_(scale), ni_(ni), nj_(nj), i_(0), j_(0) {}

  //: if an existing density map will be used there is no need to generate mixture of gaussians image, hence no need for world, camera etc.
  brec_bg_pair_density(unsigned ni, unsigned nj)
  : brec_pair_density(), verbose(false), world_(nullptr), cam_(nullptr), voxel_type_(""),
    bin_(0), scale_(0), ni_(ni), nj_(nj), i_(0), j_(0) {}

  //: generates a mixture of gaussians image of the current world using the given camera as done in brec_normalize_image_process
  bool generate_mixture_image();

  //: initialize a bg map using the given change map
  static bool initialize_bg_map(vil_image_view<float>& change_map, vil_image_view<float>& map, float high_prior = 0.9f, float top_percentile = 0.1f);

  void set_image_coords(unsigned i, unsigned j) { i_ = i; j_ = j; in_ = i+1, jn_ = j; }  // assumes horizontal pass as default
  void set_image_coords(unsigned i, unsigned j, unsigned in, unsigned jn) { i_ = i; j_ = j; in_ = in; jn_ = jn; }

  double operator()(const double y0, const double y1) override;
  double operator()(const rgb_obs_datatype y0, const rgb_obs_datatype y1);

  vil_image_view<float> prob_density(vil_image_view<grey_obs_datatype>& obs) override;
  virtual vil_image_view<float> prob_density_non_pair(vil_image_view<grey_obs_datatype>& obs);

  //: generates an appearance likelihood map for a given view as done in brec_detect_changes_process
  bool generate_appearance_likelihood_map(vil_image_view<vxl_byte>& obs, vil_image_view<float>& map);
  virtual vil_image_view<float> prob_density_from_likelihood_map(vil_image_view<vxl_byte>& obs);
  //: use the given prob density instead of generating it from the world
  virtual vil_image_view<float> prob_density_from_likelihood_map(vil_image_view<vxl_byte>& img, vil_image_view<float>& prob_density);

  bool verbose;

 private:
  bvxm_voxel_world_sptr world_;
  vpgl_camera_double_sptr cam_;
  std::string voxel_type_;
  unsigned bin_;
  unsigned scale_;
  unsigned ni_;
  unsigned nj_;
  unsigned i_;
  unsigned j_;
  unsigned in_;
  unsigned jn_;

  bvxm_voxel_slab_base_sptr mog_image_sptr;
  bvxm_voxel_slab<rgb_mog_type> rgb_mog_image_;
  bvxm_voxel_slab<grey_mog_type> grey_mog_image_;
};

#endif // brec_bg_pair_density_H_
