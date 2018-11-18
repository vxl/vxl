// This is brl/bseg/bbgm/pro/processes/bbgm_update_dist_image_process.cxx
//:
// \file
#include <iostream>
#include <bprb/bprb_func_process.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bbgm/bbgm_image_of.h>
#include <bbgm/bbgm_image_sptr.h>
#include <bbgm/bbgm_update.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_gauss_if3.h>
#include <bsta/bsta_gauss_sf1.h>
#include <bsta/bsta_mixture.h>
#include <bsta/algo/bsta_adaptive_updater.h>
#include <brdb/brdb_value.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_math.h>


bool bbgm_update_dist_image_process_cons(bprb_func_process& pro)
{
  std::vector<std::string> in_types(7), out_types(1);
  in_types[0] = "bbgm_image_sptr";// the distribution image being updated
  in_types[1]= "vil_image_view_base_sptr";//the update image data
  in_types[2]= "int"; //max_components
  in_types[3]= "int"; //window size
  in_types[4]= "float"; //initial_variance
  in_types[5]= "float"; //g_thresh
  in_types[6]= "float"; //min_stdev
  pro.set_input_types(in_types);
  out_types[0]= "bbgm_image_sptr";// the updated distribution image
  pro.set_output_types(out_types);
  pro.set_input(0, brdb_value_sptr(new brdb_value_t<bbgm_image_sptr>(nullptr)));
  return true;
}

bool bbgm_update_dist_image_process_init(bprb_func_process& pro)
{
  pro.set_input(0, new brdb_value_t<bbgm_image_sptr>(nullptr));
  return true;
}

//: Process execute function
bool bbgm_update_dist_image_process(bprb_func_process& pro)
{
  // Sanity check
  if (!pro.verify_inputs()){
    std::cerr << "In bbgm_update_dist_image_process::execute() -"
             << " invalid inputs\n";
    return false;
  }

  // Retrieve background image
  bbgm_image_sptr bgm = pro.get_input<bbgm_image_sptr>(0);

  //Retrieve image for update
  vil_image_view_base_sptr update_image =
    pro.get_input<vil_image_view_base_sptr>(1);

  //Retrieve max components
  int max_components = pro.get_input<int>(2);

  //Retrieve window_size
  int window_size = pro.get_input<int>(3);

  //Retrieve initial_variance
  auto initial_variance = pro.get_input<float>(4);

  //Retrieve g_thresh
  auto g_thresh = pro.get_input<float>(5);

  //Retrieve min_stdev
  auto min_stdev = pro.get_input<float>(6);


  vil_image_view<float> img = *vil_convert_cast(float(), update_image);
  if (update_image->pixel_format() == VIL_PIXEL_FORMAT_BYTE)
    vil_math_scale_values(img,1.0/255.0);

  unsigned ni = img.ni();
  unsigned nj = img.nj();
  unsigned np = img.nplanes();

  if (np ==1){
    typedef bsta_gauss_sf1 bsta_gauss1_t;
    typedef bsta_num_obs<bsta_gauss1_t> gauss_type1;
    typedef bsta_mixture<gauss_type1> mix_gauss_type1;
    typedef bsta_num_obs<mix_gauss_type1> obs_mix_gauss_type1;
    // get the templated mixture model
    bbgm_image_sptr model_sptr;
    if (!bgm) {
      model_sptr = new bbgm_image_of<obs_mix_gauss_type1>(ni,nj, obs_mix_gauss_type1());
    }
    else model_sptr = bgm;
    auto *model =
      static_cast<bbgm_image_of<obs_mix_gauss_type1>*>(model_sptr.ptr());

    bsta_gauss1_t init_gauss(0, initial_variance);

    bsta_mg_grimson_window_updater<mix_gauss_type1> updater(init_gauss,
                                                            max_components,
                                                            g_thresh,
                                                            min_stdev,
                                                            window_size);

    update(*model,img,updater);

    brdb_value_sptr output = new brdb_value_t<bbgm_image_sptr>(model);
    pro.set_output(0, output);
    return true;
  }
  if (np ==3)
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
    auto *model =
      static_cast<bbgm_image_of<obs_mix_gauss_type3>*>(model_sptr.ptr());

    vector3_ mean, var;
    mean.fill(0.0f); var.fill(initial_variance);
    bsta_gauss3_t init_gauss(mean, var);

    bsta_mg_grimson_window_updater<mix_gauss_type3> updater(init_gauss,
                                                            max_components,
                                                            g_thresh,
                                                            min_stdev,
                                                            window_size);

    update(*model,img,updater);

    brdb_value_sptr output = new brdb_value_t<bbgm_image_sptr>(model);
    pro.set_output(0, output);
    return true;
  }
  return false;
}
