//This is brl/bpro/core/brad_pro/processes/brad_estimate_shadows_process.cxx
//:
// \file
#include <string>
#include <iostream>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <brdb/brdb_value.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <brad/brad_image_metadata.h>
#include <brad/brad_atmospheric_parameters.h>
#include <brad/brad_estimate_shadows.h>
#ifdef DEBUG
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#endif


//:sets input and output types
bool brad_estimate_shadows_process_cons(bprb_func_process& pro)
{
   //inputs
   //0: The normalized image (pixel values are band-averaged radiance with units W m^-2 sr^-1 um-1
   //1: image metadata
   //2: atmospheric parameters
   //3: bool: prob. density output if TRUE,  prob. value output if FALSE

   std::vector<std::string> input_types_(4);
   input_types_[0] = "vil_image_view_base_sptr";
   input_types_[1] = "brad_image_metadata_sptr";
   input_types_[2] = "brad_atmospheric_parameters_sptr";
   input_types_[3] = "bool";

   if (!pro.set_input_types(input_types_))
      return false;

   //outputs:
   //0: output image, pixel values = probability (density) of shadow
   std::vector<std::string> output_types_(1);
   output_types_[0] = "vil_image_view_base_sptr";

   if (!pro.set_output_types(output_types_))
      return false;

   return true;
}

bool brad_estimate_shadows_process(bprb_func_process& pro)
{
   //check number of inputs
   if (!pro.verify_inputs())
   {
      std::cout << pro.name() << " Invalid inputs" << std::endl;
      return false;
   }

   //get the inputs
   vil_image_view_base_sptr input_img = pro.get_input<vil_image_view_base_sptr>(0);
   brad_image_metadata_sptr mdata = pro.get_input<brad_image_metadata_sptr>(1);
   brad_atmospheric_parameters_sptr atm_params = pro.get_input<brad_atmospheric_parameters_sptr>(2);
   bool output_density = pro.get_input<bool>(3);

   auto *radiance_img = dynamic_cast<vil_image_view<float>*>(input_img.ptr());
   if (!radiance_img) {
      std::cerr << "ERROR: brad_estimate_shadows_process: input image is not a vil_image_view<float>" << std::endl;
      return false;
   }
   // create new image
   auto *shadow_probs = new vil_image_view<float>(radiance_img->ni(), radiance_img->nj());

   bool result = false;
   if (output_density) {
     result = brad_estimate_shadow_prob_density(*radiance_img, *mdata, *atm_params, *shadow_probs);
   }
   else {
     result = brad_estimate_shadow_prob(*radiance_img, *mdata, *atm_params, *shadow_probs);
   }

   // assign to smart pointer
   vil_image_view_base_sptr shadow_probs_base = shadow_probs;
   pro.set_output_val<vil_image_view_base_sptr>(0, shadow_probs_base);

   return result;
}
