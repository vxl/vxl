// This is brl/bseg/bbgm/pro/bbgm_update_dist_image_process.cxx
#include "bbgm_update_dist_image_process.h"
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
#include <bsta/algo/bsta_adaptive_updater.h>
#include <brdb/brdb_value.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_math.h>

//: Constructor
bbgm_update_dist_image_process::bbgm_update_dist_image_process()
{
  //input
  input_data_.resize(7,brdb_value_sptr(0));
  input_types_.resize(7);
  input_types_[0]= "bbgm_image_sptr";
  input_types_[1]= "vil_image_view_base_sptr";
  input_types_[2]= "int"; //max_components
  input_types_[3]= "int"; //window size
  input_types_[4]= "float"; //initial_variance
  input_types_[5]= "float"; //g_thresh
  input_types_[6]= "float"; //min_stdev

  //output
  output_data_.resize(1, brdb_value_sptr(0));
  output_types_.resize(1);
  output_types_[0]="bbgm_image_sptr";
}


//: Destructor
bbgm_update_dist_image_process::~bbgm_update_dist_image_process()
{
}

bool bbgm_update_dist_image_process::init()
{
  input_data_[0]=new brdb_value_t<bbgm_image_sptr>(0);
  return true;
}

//: Execute the process
bool
bbgm_update_dist_image_process::execute()
{
  // Sanity check
  if (!this->verify_inputs()){
    vcl_cerr << "In bbgm_update_dist_image_process::execute() -"
             << " invalid inputs\n";
    return false;
  }
  // Retrieve background image
  brdb_value_t<bbgm_image_sptr>* input0 =
    static_cast<brdb_value_t<bbgm_image_sptr>* >(input_data_[0].ptr());

  bbgm_image_sptr bgm = input0->value();

  //Retrieve update image
  brdb_value_t<vil_image_view_base_sptr>* input1 =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[1].ptr());
  vil_image_view_base_sptr update_image = input1->value();

  //Retrieve max components
  brdb_value_t<int>* input2 =
    static_cast<brdb_value_t<int>* >(input_data_[2].ptr());
  int max_components = input2->value();

  //Retrieve window_size
  brdb_value_t<int>* input3 =
    static_cast<brdb_value_t<int>* >(input_data_[3].ptr());
  int window_size = input3->value();

  //Retrieve initial_variance
  brdb_value_t<float>* input4 =
    static_cast<brdb_value_t<float>* >(input_data_[4].ptr());
  float initial_variance = input4->value();

  //Retrieve g_thresh
  brdb_value_t<float>* input5 =
    static_cast<brdb_value_t<float>* >(input_data_[5].ptr());
  float g_thresh = input5->value();

  //Retrieve min_stdev
  brdb_value_t<float>* input6 =
    static_cast<brdb_value_t<float>* >(input_data_[6].ptr());
  float min_stdev = input6->value();


  vil_image_view<float> img = *vil_convert_cast(float(), update_image);
  if (update_image->pixel_format() == VIL_PIXEL_FORMAT_BYTE)
    vil_math_scale_values(img,1.0/255.0);

  unsigned ni = img.ni();
  unsigned nj = img.nj();
  unsigned np = img.nplanes();

  if(np ==1){
  typedef bsta_gauss_f1 bsta_gauss1_t;
  typedef bsta_num_obs<bsta_gauss1_t> gauss_type1;
  typedef bsta_mixture<gauss_type1> mix_gauss_type1;
  typedef bsta_num_obs<mix_gauss_type1> obs_mix_gauss_type1;
  // get the templated mixture model
  bbgm_image_sptr model_sptr;
  if (!bgm) {
    model_sptr = new bbgm_image_of<obs_mix_gauss_type1>(ni,nj, obs_mix_gauss_type1());
  }
  else model_sptr = bgm;
  bbgm_image_of<obs_mix_gauss_type1> *model =
    static_cast<bbgm_image_of<obs_mix_gauss_type1>*>(model_sptr.ptr());

  bsta_gauss1_t init_gauss(0, initial_variance);
  
  bsta_mg_grimson_window_updater<mix_gauss_type1> updater(init_gauss,
                                                          max_components,
                                                          g_thresh,
                                                          min_stdev,
                                                          window_size);

  update(*model,img,updater);

  brdb_value_sptr output0 = new brdb_value_t<bbgm_image_sptr>(model);
  output_data_[0] = output0;
  return true;
  }
  if(np ==3)
    {
  typedef bsta_gauss_if3 bsta_gauss3_t;
  typedef bsta_gauss3_t::vector_type vector3_;
  typedef bsta_num_obs<bsta_gauss3_t> gauss_type3;
  typedef bsta_mixture<gauss_type3> mix_gauss_type3;
  typedef bsta_num_obs<mix_gauss_type3> obs_mix_gauss_type3;
  // get the templated mixture model
  bbgm_image_sptr model_sptr;
  if (!bgm) {
    model_sptr = new bbgm_image_of<obs_mix_gauss_type3>(ni,nj, obs_mix_gauss_type3());
  }
  else model_sptr = bgm;
  bbgm_image_of<obs_mix_gauss_type3> *model =
    static_cast<bbgm_image_of<obs_mix_gauss_type3>*>(model_sptr.ptr());

  vector3_ mean, var;
  mean.fill(0.0f); var.fill(initial_variance);
  bsta_gauss3_t init_gauss(mean, var);
  
  bsta_mg_grimson_window_updater<mix_gauss_type3> updater( init_gauss,
                                                          max_components,
                                                          g_thresh,
                                                          min_stdev,
                                                          window_size);

  update(*model,img,updater);

  brdb_value_sptr output0 = new brdb_value_t<bbgm_image_sptr>(model);
  output_data_[0] = output0;
  return true;
  }
  return false;
}

