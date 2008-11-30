#include "bvxm_rec_update_changes_wrt_area_process.h"

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <vil/vil_image_view_base.h>
#include <vil/vil_convert.h>
#include <vil/io/vil_io_image_view.h>
#include <brip/brip_vil_float_ops.h>
#include <vpgl/vpgl_camera.h>

#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_mog_grey_processor.h>
#include <bvxm/bvxm_image_metadata.h>
#include <bvxm/bvxm_voxel_traits.h>
#include <bvxm/bvxm_util.h>
#include <vul/vul_timer.h>

#include <rec/bvxm_bg_pair_density.h>
#include <rec/bvxm_fg_pair_density.h>
#include <rec/bvxm_fg_bg_pair_density.h>
#include <rec/bvxm_bayesian_propagation.h>

bvxm_rec_update_changes_wrt_area_process::bvxm_rec_update_changes_wrt_area_process()
{
  //inputs
  input_data_.resize(2,brdb_value_sptr(0));
  input_types_.resize(2);
  //input_types_[0] = "vil_image_view_base_sptr";
  input_types_[0] = "vil_image_view_base_sptr";      // input prob map p(x in B) (float map with values in [0,1]
  input_types_[1] = "vil_image_view_base_sptr";      // input area map/glitch map

  //output
  output_data_.resize(2,brdb_value_sptr(0));
  output_types_.resize(2);
  output_types_[0]= "vil_image_view_base_sptr";      // output new prob map
  output_types_[1]= "vil_image_view_base_sptr";      // output new prob map as a byte image

  parameters()->add("lambda for p(ad | gb)", "lambda1", 3.9645f);
  parameters()->add("k for p(ad | gb)", "k1", 1.4778f);
  parameters()->add("lambda for p(ad | not gb)", "lambda2", 0.9667f);
  parameters()->add("k for p(ad | not gb)", "k2", 1.0f); // if k is 1, weibull becomes an exponential distribution
}


bool bvxm_rec_update_changes_wrt_area_process::execute()
{
  // Sanity check
  if (!this->verify_inputs())
    return false;

  brdb_value_t<vil_image_view_base_sptr>* input0 =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[0].ptr());
  vil_image_view_base_sptr bg_map_v = input0->value();
  vil_image_view<float> bg_map(bg_map_v);

  brdb_value_t<vil_image_view_base_sptr>* input1 =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[1].ptr());
  vil_image_view_base_sptr glitch_map_v = input1->value();
  vil_image_view<float> glitch_map(glitch_map_v);

  vul_timer t2;
  t2.mark();

  bvxm_bayesian_propagation bp(bg_map);
  // to construct P(Ad | Gb), values based on stats collected from data
  float lambda1=-1.f; parameters()->get_value("lambda1", lambda1);
  float k1=-1.f;      parameters()->get_value("k1", k1);
  // to construct P(Ad | not Gb), values based on stats collected from data
  float lambda2=-1.f; parameters()->get_value("lambda2", lambda2);
  float k2=-1.f;      parameters()->get_value("k2", k2);
  bp.run_area(glitch_map, lambda1, k1, lambda2, k2);

  vil_image_view_base_sptr out_map_sptr = new vil_image_view<float>(bp.bg_map_);
  brdb_value_sptr output = new brdb_value_t<vil_image_view_base_sptr>(out_map_sptr);
  output_data_[0] = output;

  vil_image_view<vxl_byte> out_b(bp.bg_map_.ni(), bp.bg_map_.nj());
  vil_convert_stretch_range_limited(bp.bg_map_, out_b, 0.0f, 1.0f);

  vil_image_view_base_sptr out_map_sptr2 = new vil_image_view<vxl_byte>(out_b);
  brdb_value_sptr output2 = new brdb_value_t<vil_image_view_base_sptr>(out_map_sptr2);
  output_data_[1] = output2;

  vcl_cout << " whole process took: " << t2.real() / (60*1000.0f) << " mins.\n";

  return true;
}

