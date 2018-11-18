#include "brec_bg_pair_density.h"
//:
// \file
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date October 01, 2008

#include <bvxm/grid/bvxm_voxel_slab.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

double brec_bg_pair_density::operator()(const double y0, const double y1)
{
  //: the prob is p(y0 | x0 in B)*p(y1 | x1 in B), use i_,j_ and assume that y1 is immediately to the right of y0
  double p0 = grey_mog_image_(i_, j_).prob_density((grey_obs_datatype)y0);
  double p1 = grey_mog_image_(in_, jn_).prob_density((grey_obs_datatype)y1);

  return p0*p1;
}

double brec_bg_pair_density::operator()(const rgb_obs_datatype y0, const rgb_obs_datatype y1)
{
  double p0 = rgb_mog_image_(i_, j_).prob_density(y0);
  double p1 = rgb_mog_image_(in_, jn_).prob_density(y1);

  return p0*p1;
}

vil_image_view<float>
brec_bg_pair_density::prob_density(vil_image_view<grey_obs_datatype>& obs)
{
  unsigned ni = obs.ni();
  unsigned nj = obs.nj();

  vil_image_view<float> map(ni, nj);
  map.fill(0.0f);
  for (unsigned i = 0; i < ni; i++) {
    for (unsigned j = 0; j < nj; j++) {
      if (i+1 < ni) {
        this->set_image_coords(i,j);
        map(i,j) = (float)this->operator()(obs(i,j), obs(i+1, j));
      }
    }
  }

  return map;
}

vil_image_view<float>
brec_bg_pair_density::prob_density_non_pair(vil_image_view<grey_obs_datatype>& obs)
{
  unsigned ni = obs.ni();
  unsigned nj = obs.nj();

  vil_image_view<float> map(ni, nj);
  map.fill(0.0f);
  for (unsigned i = 0; i < ni; i++) {
    for (unsigned j = 0; j < nj; j++) {
      map(i,j) = grey_mog_image_(i, j).prob_density((grey_obs_datatype)obs(i,j));
    }
  }

  return map;
}

bool brec_bg_pair_density::generate_mixture_image()
{
  if (!world_)
    return false;

  // create metadata:
  vil_image_view_base_sptr dummy_img = new vil_image_view<vxl_byte>(ni_, nj_);
  bvxm_image_metadata observation(dummy_img,cam_);  // image is only required to get ni and nj

  vil_image_view_base_sptr mix_exp_img;

  //1) get probability mixtures of all pixels in image
  if (voxel_type_ == "apm_mog_rgb") {
    world_->mixture_of_gaussians_image<APM_MOG_RGB>(observation, mog_image_sptr, bin_);
    auto* mog_image_ptr = dynamic_cast<bvxm_voxel_slab<rgb_mog_type>*>(mog_image_sptr.ptr());

    bvxm_voxel_traits<APM_MOG_RGB>::appearance_processor apm_processor;
    bvxm_voxel_slab<rgb_obs_datatype> exp_img = apm_processor.expected_color(*mog_image_ptr);
    mix_exp_img = new vil_image_view<vxl_byte>(ni_, nj_, 3);
    bvxm_util::slab_to_img(exp_img, mix_exp_img);

    if (verbose) {
      vil_save(*mix_exp_img, "./mixture_expected_img.png");
    }

    rgb_mog_image_ = *mog_image_ptr;
  }
  else if (voxel_type_ == "apm_mog_grey")
  {
    world_->mixture_of_gaussians_image<APM_MOG_GREY>(observation, mog_image_sptr, bin_);
    auto* mog_image_ptr = dynamic_cast<bvxm_voxel_slab<grey_mog_type>*>(mog_image_sptr.ptr());

    bvxm_voxel_traits<APM_MOG_GREY>::appearance_processor apm_processor;
    bvxm_voxel_slab<float> exp_img = apm_processor.expected_color(*mog_image_ptr);
    mix_exp_img = new vil_image_view<vxl_byte>(ni_, nj_, 1);
    bvxm_util::slab_to_img(exp_img, mix_exp_img);

    if (verbose) {
      vil_save(*mix_exp_img, "./mixture_expected_img.png");
    }
    grey_mog_image_ = *mog_image_ptr;

    if (verbose) {
      vil_image_view<float> imgf(ni_, nj_, 1);
      vil_image_view<vxl_byte> mix_exp_img_b(mix_exp_img);
      vil_convert_stretch_range_limited(mix_exp_img_b, imgf, (vxl_byte)0, (vxl_byte)255, 0.0f, 1.0f);
      vil_image_view<float> outf = this->prob_density(imgf);
      vil_image_view<vxl_byte> outf_b(ni_, nj_, 1);
      float minv, maxv;
      vil_math_value_range(outf, minv, maxv);
      std::cout << "minv: " << minv << " maxv: " << maxv << std::endl;
      vil_convert_stretch_range_limited(outf, outf_b, 0.0f, 4.0f);
      vil_save(outf_b, "./mixture_expected_img_likelihood_map.png");
    }
  }
  else {
    std::cout << "In brec_normalize_image_process::execute() -- input appearance model: " << voxel_type_ << " is not supported\n";
    return false;
  }

  return true;
}

bool
brec_bg_pair_density::generate_appearance_likelihood_map(vil_image_view<vxl_byte>& img, vil_image_view<float>& prob_map)
{
  //vil_image_view<float> prob_map(img.ni(),img.nj(),1);
  vil_image_view<bool> mask(img.ni(),img.nj(),1);
  assert(prob_map.ni() == mask.ni() && prob_map.nj() == mask.nj());

  // create metadata:
  vil_image_view_base_sptr img_sptr = new vil_image_view<vxl_byte>(img);
  bvxm_image_metadata observation(img_sptr,cam_);  // image is only required to get ni and nj

  bool result;
  if (voxel_type_ == "apm_mog_grey")
    result = world_->pixel_probability_density<APM_MOG_GREY>(observation,prob_map, mask, bin_,scale_);
  else if (voxel_type_ == "apm_mog_rgb")
    result = world_->pixel_probability_density<APM_MOG_RGB>(observation,prob_map, mask, bin_,scale_);
  else
    result = false;

  return result;
}

vil_image_view<float>
brec_bg_pair_density::prob_density_from_likelihood_map(vil_image_view<vxl_byte>& img)
{
  unsigned ni = img.ni();
  unsigned nj = img.nj();

  vil_image_view<float> prob_map(ni, nj,1);
  generate_appearance_likelihood_map(img, prob_map);

  vil_image_view<float> map(ni, nj);
  map.fill(0.0f);
  for (unsigned i = 0; i < ni; i++) {
    for (unsigned j = 0; j < nj; j++) {
      if (i+1 < ni) {
        map(i,j) = prob_map(i,j)*prob_map(i+1,j);
      }
    }
  }

  return map;
}

vil_image_view<float>
brec_bg_pair_density::prob_density_from_likelihood_map(vil_image_view<vxl_byte>& img, vil_image_view<float>& prob_density)
{
  unsigned ni = img.ni();
  unsigned nj = img.nj();

  vil_image_view<float> map(ni, nj);
  map.fill(0.0f);
  for (unsigned i = 0; i < ni; i++) {
    for (unsigned j = 0; j < nj; j++) {
      if (i+1 < ni) {
        map(i,j) = prob_density(i,j)*prob_density(i+1,j);
      }
    }
  }

  return map;
}

//: initialize a bg map using the given change map
bool
brec_bg_pair_density::initialize_bg_map(vil_image_view<float>& prob_density, vil_image_view<float>& map, float high_prior, float top_percentile)
{
  //: compute prior_const such that p(x in bg|y) ~ 1 when p(y|x in bg) is high
  //  Use the top 10% percentile value to determine when p(y|x in bg) is high
  float val = -1e38f; // dummy initialisation (min float), to avoid compiler warning
  vil_math_value_range_percentile(prob_density, double(1.0-top_percentile), val);
#if 1
  std::cout << "1-" << top_percentile << "= " << 1-top_percentile << " percentile value of prob density is: " << val << std::endl;
  float min = +1e38f, max = -1e38f;
  vil_math_value_range(prob_density, min, max);
  std::cout << "same prob density min is: " << min << " max is: " << max << std::endl;
#endif

  //: set p(x in bg|y) = 0.9 when p(y|x in bg) is high, i.e. when p(y|x in bg) = val
  float prior_const = high_prior/(val-high_prior*val+high_prior);
  //float prior_const = 0.9f/(val-0.9f*val+0.9f);

  // now generate the map
  map.fill(0.0f);
  for (unsigned i = 0; i < map.ni(); i++) {
    for (unsigned j = 0; j < map.nj(); j++) {
      map(i,j) = prob_density(i,j)*prior_const / (prob_density(i,j)*prior_const + 1 - prior_const);
      if (map(i,j) > 1.0f)  // just in case
        map(i,j) = 1.0f;
      if (map(i,j) < 0.0f)  // just in case
        map(i,j) = 0.0f;
    }
  }

  return true;
}
