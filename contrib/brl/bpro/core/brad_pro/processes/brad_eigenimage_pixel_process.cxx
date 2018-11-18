// This is brl/bpro/core/brad_pro/processes/brad_eigenimage_pixel_process.cxx
#include <bprb/bprb_func_process.h>
#include <brad/brad_eigenspace.h>
#include <vil/vil_convert.h>
//:
// \file


namespace bbas_core_brad_eigenimage_pixel
{
}

//: Constructor
bool brad_eigenimage_pixel_process_cons(bprb_func_process& pro)
{
using namespace bbas_core_brad_eigenimage_pixel;
  bool ok=false;
  std::vector<std::string> input_types(2);
  input_types[0]="brad_eigenspace_sptr"; //eigenspace
  input_types[1]="vil_image_view_base_sptr"; //input image
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
bool brad_eigenimage_pixel_process(bprb_func_process& pro)
{
  using namespace bbas_core_brad_eigenimage_pixel;
  // Sanity check
  if (pro.n_inputs()!= 2) {
    std::cout << "brad_eigenimage_pixel_process: The input number should be 2" << std::endl;
    return false;
  }
  brad_eigenspace_sptr es_ptr = pro.get_input<brad_eigenspace_sptr>(0);
  if (!es_ptr) {
    std::cout << "in eigenimage_pixel_process, null eigenspace pointer\n";
    return false;
  }

  vil_image_view_base_sptr image = pro.get_input<vil_image_view_base_sptr>(1);
  if (!image) {
    std::cout << "in eigenimage_pixel_process, input image view is null\n";
    return false;
  }
  vil_image_view<float> fimage = *vil_convert_cast(float(), image);
  vil_image_view<float> eignimage;
  CAST_CALL_EIGENSPACE(es_ptr, ep->compute_eigenimage_pixel(fimage, eignimage), "in eigenimage_pixel_process - eigenimage function failed\n")
  pro.set_output_val<vil_image_view_base_sptr>(0, new vil_image_view<float>(eignimage));
  return true;
}
