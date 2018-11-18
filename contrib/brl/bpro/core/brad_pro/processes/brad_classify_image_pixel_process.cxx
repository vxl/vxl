// This is brl/bpro/core/brad_pro/processes/brad_classify_image_pixel_process.cxx
#include <bprb/bprb_func_process.h>
#include <brad/brad_eigenspace.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
//:
// \file


namespace bbas_core_brad_classify_image_pixel
{
}

//: Constructor
bool brad_classify_image_pixel_process_cons(bprb_func_process& pro)
{
using namespace bbas_core_brad_classify_image_pixel;
  bool ok=false;
  std::vector<std::string> input_types(5);
  input_types[0]="brad_eigenspace_sptr"; //eigenspace
  input_types[1]="bsta_joint_histogram_3d_base_sptr"; //no atmospherics
  input_types[2]="bsta_joint_histogram_3d_base_sptr"; //with atmospherics
  input_types[3]="vil_image_view_base_sptr"; //input image
  input_types[4]="float";//prob ratio
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //outputs
  std::vector<std::string> output_types(1);
  output_types[0]= "vil_image_view_base_sptr";
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

//: Execute the process
bool brad_classify_image_pixel_process(bprb_func_process& pro)
{
  using namespace bbas_core_brad_classify_image_pixel;
  // Sanity check
  if (pro.n_inputs()!= 5) {
    std::cout << "brad_classify_image_pixel_process: The input number should be 5" << std::endl;
    return false;
  }
  brad_eigenspace_sptr es_ptr = pro.get_input<brad_eigenspace_sptr>(0);
  if (!es_ptr) {
    std::cout << "in classify_image_pixel_process, null eigenspace pointer\n";
    return false;
  }
  bsta_joint_histogram_3d_base_sptr hno_ptr =
    pro.get_input<bsta_joint_histogram_3d_base_sptr>(1);

  auto* hist_no = dynamic_cast<bsta_joint_histogram_3d<float>*>(hno_ptr.ptr());

  if (!hist_no) {
    std::cout << "in classify_image_pixel_process, hist can't be cast\n";
    return false;
  }

  bsta_joint_histogram_3d_base_sptr h_atmos_ptr =
    pro.get_input<bsta_joint_histogram_3d_base_sptr>(2);

  auto* hist_atmos = dynamic_cast<bsta_joint_histogram_3d<float>*>(h_atmos_ptr.ptr());

  if (!hist_atmos) {
    std::cout << "in classify_image_pixel_process, hist can't be cast\n";
    return false;
  }
  vil_image_view_base_sptr image = pro.get_input<vil_image_view_base_sptr>(3);
  if (!image) {
    std::cout << "in classify_image_pixel_process, input image view is null\n";
    return false;
  }
  vil_image_view<float> fimage = *vil_convert_cast(float(), image);
  auto prob_ratio = pro.get_input<float>(4);

  vil_image_view<float> class_image;
  CAST_CALL_EIGENSPACE(es_ptr, ep->classify_image_pixel(fimage, *hist_no, *hist_atmos, prob_ratio, class_image), "in classify_image_pixel_process - classify function failed\n")
  pro.set_output_val<vil_image_view_base_sptr>(0, new vil_image_view<float>(class_image));
  return true;
}
