// This is brl/bseg/bbgm/pro/bbgm_measure_process.cxx
#include "bbgm_measure_process.h"
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>
#include <bbgm/bbgm_image_of.h>
#include <bbgm/bbgm_image_sptr.h>
#include <bbgm/bbgm_update.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_gauss_if3.h>
#include <bsta/bsta_gauss_f1.h>
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
//: Constructor
bbgm_measure_process::bbgm_measure_process()
{
  //input
  input_data_.resize(4,brdb_value_sptr(0));
  input_types_.resize(4);
  input_types_[0]= "bbgm_image_sptr"; //background image
  input_types_[1]= "vil_image_view_base_sptr"; //test image
  input_types_[2]= "vcl_string"; //what to measure
  input_types_[3]= "float"; // measure tolerance


  //output
  output_data_.resize(1, brdb_value_sptr(0));
  output_types_.resize(1);
  output_types_[0]= "vil_image_view_base_sptr";
}


//: Destructor
bbgm_measure_process::~bbgm_measure_process()
{
}


//: Execute the process
bool
bbgm_measure_process::execute()
{
  // Sanity check
  if (!this->verify_inputs()) {
    vcl_cerr << "In bbgm_measure_process::execute() -"
             << " invalid inputs\n";
    return false;
  }

  // Retrieve background image

  brdb_value_t<bbgm_image_sptr>* input0 =
    static_cast<brdb_value_t<bbgm_image_sptr>* >(input_data_[0].ptr());
  bbgm_image_sptr bgm = input0->value();
  if (!bgm) {
    vcl_cerr << "In bbgm_measure_process::execute() -"
             << " null distribution image\n";
    return false;
  }

  brdb_value_t<vil_image_view_base_sptr>* input1 =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[1].ptr());
  vil_image_view_base_sptr img_ptr = input1->value();
  if (!img_ptr) {
    vcl_cerr << "In bbgm_measure_process::execute() -"
             << " null measurement input image\n";
    return false;
  }
  vil_image_view<float> image = *vil_convert_cast(float(), img_ptr);
  if (img_ptr->pixel_format() == VIL_PIXEL_FORMAT_BYTE)
    vil_math_scale_values(image,1.0/255.0);

  unsigned np = image.nplanes();

  //Retrieve attribute to measure, e.g. probability
  brdb_value_t<vcl_string>* input2 =
    static_cast<brdb_value_t<vcl_string>* >(input_data_[2].ptr());
  vcl_string attr = input2->value();

  //Retrieve measure tolerance
  brdb_value_t<float>* input3 =
    static_cast<brdb_value_t<float>* >(input_data_[3].ptr());
  float tolerance = input3->value();

  vil_image_view<float> result;
  if (np ==1) {
    typedef bsta_gauss_f1 bsta_gauss1_t;
    typedef bsta_num_obs<bsta_gauss1_t> gauss_type1;
    typedef bsta_mixture<gauss_type1> mix_gauss_type1;
    typedef bsta_num_obs<mix_gauss_type1> obs_mix_gauss_type1;
    bbgm_image_of<obs_mix_gauss_type1> *model =
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
      vcl_cout << "In bbgm_measure_process::execute() -"
               << " measurement not available\n";
      return false;
    }
  }
  if (np ==3)
  {
    typedef bsta_gauss_if3 bsta_gauss3_t;
    typedef bsta_gauss3_t::vector_type vector3_;
    typedef bsta_num_obs<bsta_gauss3_t> gauss_type3;
    typedef bsta_mixture<gauss_type3> mix_gauss_type3;
    typedef bsta_num_obs<mix_gauss_type3> obs_mix_gauss_type3;
    bbgm_image_of<obs_mix_gauss_type3> *model =
      static_cast<bbgm_image_of<obs_mix_gauss_type3>*>(bgm.ptr());

    if (attr=="probability") {
      bsta_probability_functor<mix_gauss_type3> functor_;
      measure(*model, image, result, functor_, tolerance);
    }
#ifdef MEASURE_BKGROUND
    else if (attr=="prob_background") {
      bsta_prob_density_functor<mix_gauss_type3> functor_;
      measure_bkground(*model, image, result, functor_, tolerance);
    }
#endif // MEASURE_BKGROUND
    else {
      vcl_cout << "In bbgm_measure_process::execute() -"
               << " measurement not available\n";
      return false;
    }
  }
  brdb_value_sptr output0 =
    new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<float>(result));
  output_data_[0] = output0;

  return true;
}

