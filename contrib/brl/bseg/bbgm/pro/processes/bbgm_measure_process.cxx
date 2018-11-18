// This is brl/bseg/bbgm/pro/processes/bbgm_measure_process.cxx
#define MEASURE_BKGROUND
//:
// \file
#include <iostream>
#include <cstddef>
#include <string>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <bbgm/bbgm_image_of.h>
#include <bbgm/bbgm_image_sptr.h>
#include <bbgm/bbgm_update.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_gauss_if3.h>
#include <bsta/bsta_gauss_sf1.h>
#if 0
#include <bsta/bsta_parzen_sphere.h>
#endif
#include <bsta/bsta_mixture.h>
#include <bsta/bsta_basic_functors.h>
#include <bsta/algo/bsta_adaptive_updater.h>
#include <brdb/brdb_value.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_math.h>
#include <bsta/bsta_histogram.h>
#include <bbgm/bbgm_measure.h>
#include <brip/brip_vil_float_ops.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

bool bbgm_measure_process_cons(bprb_func_process& pro)
{
  std::vector<std::string> in_types(4), out_types(1);
  in_types[0]= "bbgm_image_sptr"; //background image
  in_types[1]= "vil_image_view_base_sptr"; //test image
  in_types[2]= "vcl_string"; //what to measure
  in_types[3]= "float"; // measure tolerance
  pro.set_input_types(in_types);
  out_types[0]= "vil_image_view_base_sptr"; //the display image
  pro.set_output_types(out_types);
  return true;
}

//: Execute the process function
bool bbgm_measure_process(bprb_func_process& pro)
{
  // Sanity check
  if (!pro.verify_inputs()) {
    std::cerr << "In bbgm_measure_process::execute() -"
             << " invalid inputs\n";
    return false;
  }

  // Retrieve background image

  bbgm_image_sptr bgm = pro.get_input<bbgm_image_sptr>(0);
  if (!bgm) {
    std::cerr << "In bbgm_measure_process::execute() -"
             << " null distribution image\n";
    return false;
  }
  std::string image_type = bgm->is_a();
  //for now just check for parzen_sphere in the string
  std::size_t str_indx = image_type.find("parzen_sphere");
  bool parzen = str_indx != std::string::npos;

  vil_image_view_base_sptr img_ptr =
    pro.get_input<vil_image_view_base_sptr>(1);
  if (!img_ptr) {
    std::cerr << "In bbgm_measure_process::execute() -"
             << " null measurement input image\n";
    return false;
  }
  vil_image_view<float> image = *vil_convert_cast(float(), img_ptr);
  if (img_ptr->pixel_format() == VIL_PIXEL_FORMAT_BYTE)
    vil_math_scale_values(image,1.0/255.0);

  unsigned np = image.nplanes();

  //Retrieve attribute to measure, e.g. probability
  std::string attr = pro.get_input<std::string>(2);

  //Retrieve measure tolerance
  auto tolerance = pro.get_input<float>(3);

  vil_image_view<float> result;
  if (np ==1) {
    typedef bsta_gauss_sf1 bsta_gauss1_t;
    typedef bsta_num_obs<bsta_gauss1_t> gauss_type1;
    typedef bsta_mixture<gauss_type1> mix_gauss_type1;
    typedef bsta_num_obs<mix_gauss_type1> obs_mix_gauss_type1;
    auto *model =
      static_cast<bbgm_image_of<obs_mix_gauss_type1>*>(bgm.ptr());

    if (attr=="probability") {
      bsta_probability_functor<mix_gauss_type1> functor_;
      measure(*model, image, result, functor_, tolerance);
    }
#ifdef MEASURE_BKGROUND
    else if (attr=="prob_background") {
      bsta_prob_density_functor<mix_gauss_type1> functor_;
      measure_bkground(*model, image, result, functor_, tolerance);
    }
#endif // MEASURE_BKGROUND
    else {
      std::cout << "In bbgm_measure_process::execute() -"
               << " measurement not available\n";
      return false;
    }
  }
  if (np ==3)
  {
    typedef bsta_gauss_if3 bsta_gauss3_t;
    typedef bsta_num_obs<bsta_gauss3_t> gauss_type3;
    typedef bsta_mixture<gauss_type3> mix_gauss_type3;
    typedef bsta_num_obs<mix_gauss_type3> obs_mix_gauss_type3;
#if 0
    typedef bsta_parzen_sphere<float, 3> parzen_f3_t;
    typedef parzen_f3_t::vector_type pvtype_;
#endif
    if (attr=="probability") {
      if (!parzen) {
        auto *model =
          static_cast<bbgm_image_of<obs_mix_gauss_type3>*>(bgm.ptr());
        bsta_probability_functor<mix_gauss_type3> functor_;
        measure(*model, image, result, functor_, tolerance);
      }
    }
#ifdef MEASURE_BKGROUND
    else if (attr=="prob_background") {
      if (!parzen) {
        auto *model =
          static_cast<bbgm_image_of<obs_mix_gauss_type3>*>(bgm.ptr());
        bsta_prob_density_functor<mix_gauss_type3> functor_;
        measure_bkground(*model, image, result, functor_, tolerance);
      }
    }
#endif // MEASURE_BKGROUND
    else {
      std::cout << "In bbgm_measure_process::execute() -"
               << " measurement not available\n";
      return false;
    }
  }
  std::vector<std::string> output_types(1);
  output_types[0]= "vil_image_view_base_sptr";
  pro.set_output_types(output_types);
  brdb_value_sptr output =
    new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<float>(result));
  pro.set_output(0, output);
  return true;
}
