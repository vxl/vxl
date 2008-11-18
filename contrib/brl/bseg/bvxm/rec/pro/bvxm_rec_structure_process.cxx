#include "bvxm_rec_structure_process.h"

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <vil/vil_image_view_base.h>
#include <vil/io/vil_io_image_view.h>
#include <vil/vil_new.h>
#include <brip/brip_vil_float_ops.h>
#include <vul/vul_timer.h>

#include <rec/bvxm_part_hierarchy_builder.h>
#include <rec/bvxm_part_hierarchy.h>
#include <rec/bvxm_part_gaussian_sptr.h>
#include <rec/bvxm_part_gaussian.h>

bvxm_rec_structure_process::bvxm_rec_structure_process()
{
  //inputs
  input_data_.resize(2,brdb_value_sptr(0));
  input_types_.resize(2);
  input_types_[0] = "vil_image_view_base_sptr";      // input orig view
  input_types_[1] = "unsigned";      // detector id for the type of structure to be recognized

  //output
  output_data_.resize(2,brdb_value_sptr(0));
  output_types_.resize(2);
  output_types_[0]= "vil_image_view_base_sptr";      // output prob map
  output_types_[1]= "vil_image_view_base_sptr";      // output map overlayed on orig view as a byte image
}


bool bvxm_rec_structure_process::execute()
{
  // Sanity check
  if (!this->verify_inputs())
    return false;

  brdb_value_t<vil_image_view_base_sptr>* input0 =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[0].ptr());
  vil_image_view_base_sptr orig_view = input0->value();

  if (orig_view->pixel_format() != VIL_PIXEL_FORMAT_BYTE)
    return false;

  vil_image_view<vxl_byte> orig_img(orig_view);
  unsigned ni = orig_img.ni();
  unsigned nj = orig_img.nj();
  vil_image_resource_sptr img = vil_new_image_resource_of_view(orig_img);

  vul_timer t2;
  t2.mark();

  brdb_value_t<unsigned>* input1 = static_cast<brdb_value_t<unsigned>* >(input_data_[1].ptr());
  unsigned d_id = input1->value();

  bvxm_part_hierarchy_sptr h;
  switch (d_id) {
    case 0: { h = bvxm_part_hierarchy_builder::construct_detector_roi1_0(); } break;
    case 1: { h = bvxm_part_hierarchy_builder::construct_detector_roi1_1(); } break;
    case 2: { h = bvxm_part_hierarchy_builder::construct_detector_roi1_2(); } break;
    case 3: { h = bvxm_part_hierarchy_builder::construct_detector_roi1_3(); } break;
    case 4: { h = bvxm_part_hierarchy_builder::construct_detector_roi1_4(); } break;
    default: { vcl_cout << "In bvxm_rec_structure_process::execute() -- Unrecognized detector type!!\n"; return false; }
  }

  // now extract instances of primitive part types in h
  vcl_vector<bvxm_part_instance_sptr> parts_0;
  vcl_vector<bvxm_part_instance_sptr>& d_ins = h->get_dummy_primitive_instances();
  unsigned prev_size = parts_0.size();
  for (unsigned i = 0; i < d_ins.size(); i++) {
    if (d_ins[i]->kind_ != bvxm_part_instance_kind::GAUSSIAN)
      return false;

    bvxm_part_gaussian_sptr gp = d_ins[i]->cast_to_gaussian();
    if (!gp)
      return false;

    if (!extract_gaussian_primitives(img, gp->lambda0_, gp->lambda1_, gp->theta_, gp->bright_, gp->cutoff_percentage_, gp->detection_threshold_, gp->type_, parts_0))
      return false;

    vcl_cout << "extracted " << parts_0.size()-prev_size << " primitive parts of type: " << d_ins[i]->type_ << vcl_endl;
    prev_size = parts_0.size();
  }

  unsigned highest = h->highest_layer_id();
  vcl_vector<bvxm_part_instance_sptr> parts_upper_most(parts_0);
  for (unsigned l = 1; l <= highest; l++) {
    vcl_vector<bvxm_part_instance_sptr> parts_current;
    h->extract_upper_layer(parts_upper_most, ni, nj, parts_current);
    vcl_cout << "extracted " << parts_current.size() << " parts of layer " << l << '\n';
    parts_upper_most.clear();
    parts_upper_most = parts_current;
  }

  vil_image_view<float> output_map_float(ni, nj);
  bvxm_part_hierarchy::generate_output_map(parts_upper_most, output_map_float);

  vil_image_view<vxl_byte> output_img(ni, nj, 3);
  bvxm_part_hierarchy::generate_output_img(parts_upper_most, orig_img, output_img);

  vil_image_view_base_sptr out_map_sptr = new vil_image_view<float>(output_map_float);
  brdb_value_sptr output = new brdb_value_t<vil_image_view_base_sptr>(out_map_sptr);
  output_data_[0] = output;

  vil_image_view_base_sptr out_map_sptr1 = new vil_image_view<vxl_byte>(output_img);
  brdb_value_sptr output1 = new brdb_value_t<vil_image_view_base_sptr>(out_map_sptr1);
  output_data_[1] = output1;

  vcl_cout << " whole process took: " << t2.real() / (60*1000.0f) << " mins.\n";

  return true;
}

