#include "bvxm_rec_update_changes_process.h"

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <vil/vil_image_view_base.h>
#include <vil/io/vil_io_image_view.h>
#include <vil/vil_convert.h>
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

bvxm_rec_update_changes_process::bvxm_rec_update_changes_process()
{
  //inputs
  input_data_.resize(4,brdb_value_sptr(0));
  input_types_.resize(4);
  //input_types_[0] = "vil_image_view_base_sptr";
  input_types_[0] = "vil_image_view_base_sptr";      // input change map
  input_types_[1] = "vil_image_view_base_sptr";      // input orig view
  input_types_[2] = "unsigned";      // number of passes
  input_types_[3] = "float";      // sigma for foreground similarity

  //output
  output_data_.resize(2,brdb_value_sptr(0));
  output_types_.resize(2);
  output_types_[0]= "vil_image_view_base_sptr";      // output new prob map
  output_types_[1]= "vil_image_view_base_sptr";      // output new prob map as a byte image
}


bool bvxm_rec_update_changes_process::execute()
{
  // Sanity check
  if (!this->verify_inputs())
    return false;

  brdb_value_t<vil_image_view_base_sptr>* input0 =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[0].ptr());
  vil_image_view_base_sptr change_map = input0->value();

  vil_image_view<float> change_map_f(change_map);

  brdb_value_t<vil_image_view_base_sptr>* input1 =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[1].ptr());
  vil_image_view_base_sptr orig_view = input1->value();

  brdb_value_t<unsigned>* input2 =
    static_cast<brdb_value_t<unsigned>* >(input_data_[2].ptr());
  unsigned n = input2->value();  // number of passes

  brdb_value_t<float>* input3 = static_cast<brdb_value_t<float>* >(input_data_[3].ptr());
  float sigma = input3->value();  //

  unsigned ni = change_map_f.ni();
  unsigned nj = change_map_f.nj();

  vul_timer t2;
  t2.mark();

  vil_image_view<vxl_byte> orig_img(orig_view);

  bvxm_fg_pair_density fgp;
  //fgp.set_sigma(0.058); // set manually for now
  //fgp.set_sigma(0.02); // set manually for now
  //fgp.set_sigma(0.2); // set manually for now
  fgp.set_sigma(sigma);
  bvxm_fg_bg_pair_density fgbgp;
  //fgbgp.set_sigma(0.058);
  //fgbgp.set_sigma(0.02);
  //fgbgp.set_sigma(0.2);
  fgbgp.set_sigma(sigma);
  vcl_cout << "\t update process running with sigma: " << sigma << vcl_endl;

  bvxm_bayesian_propagation bp(orig_img, change_map_f);
  bp.initialize_bg_map(); // just turning change_map_f (a density map) into a probability map with values in [0,1]
  bp.run_using_prob_density_as_bgp(fgp, fgbgp, n);

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

