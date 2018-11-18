// This is brl/bseg/bbgm/pro/processes/bbgm_display_dist_image_process.cxx

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
#include <bsta/bsta_basic_functors.h>
#include <bsta/algo/bsta_adaptive_updater.h>
#include <brdb/brdb_value.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_math.h>
#include <bbgm/bbgm_viewer.h>
#include <bbgm/bbgm_viewer_sptr.h>


namespace type_definitions {
  //: mixture of independent gaussian's of 3 dimensions
  typedef bsta_num_obs<bsta_mixture<bsta_num_obs<bsta_gauss_if3> > > mix_gauss_type_if3;
  typedef bsta_num_obs<bsta_mixture_fixed<bsta_num_obs<bsta_gauss_sf1>, 3> > mix_fixed_gauss_type_sf1;
  typedef bsta_num_obs<bsta_mixture<bsta_num_obs<bsta_gauss_sf1> > > mix_gauss_type_sf1;
}

//: if the application needs to display different types than the ones already registered, then add them to this function
void register_mean_viewers()
{
  using namespace type_definitions;
  bbgm_mean_viewer::register_view_maker(new bbgm_view_maker<mix_gauss_type_if3, bsta_mean_functor<mix_gauss_type_if3> >);
  bbgm_mean_viewer::register_view_maker(new bbgm_view_maker<mix_fixed_gauss_type_sf1, bsta_mean_functor<mix_fixed_gauss_type_sf1> >);
  bbgm_mean_viewer::register_view_maker(new bbgm_view_maker<mix_gauss_type_sf1, bsta_mean_functor<mix_gauss_type_sf1> >);
}

void register_variance_viewers()
{
  using namespace type_definitions;
  bbgm_variance_viewer::register_view_maker(new bbgm_view_maker<mix_gauss_type_if3, bsta_diag_covar_functor<mix_gauss_type_if3> >);
  bbgm_variance_viewer::register_view_maker(new bbgm_view_maker<mix_fixed_gauss_type_sf1, bsta_var_functor<mix_fixed_gauss_type_sf1> >);
  bbgm_variance_viewer::register_view_maker(new bbgm_view_maker<mix_gauss_type_sf1, bsta_var_functor<mix_gauss_type_sf1> >);
}

void register_weight_viewers()
{
  using namespace type_definitions;
  bbgm_weight_viewer::register_view_maker(new bbgm_view_maker<mix_gauss_type_if3, bsta_weight_functor<mix_gauss_type_if3> >);
  bbgm_weight_viewer::register_view_maker(new bbgm_view_maker<mix_fixed_gauss_type_sf1, bsta_weight_functor<mix_fixed_gauss_type_sf1> >);
  bbgm_weight_viewer::register_view_maker(new bbgm_view_maker<mix_gauss_type_sf1, bsta_weight_functor<mix_gauss_type_sf1> >);
}


// constructor function
bool bbgm_display_dist_image_process_cons(bprb_func_process& pro)
{
  //input
  std::vector<std::string> in_types(4), out_types(1);
  in_types[0]= "bbgm_image_sptr"; //background image
  in_types[1]= "vcl_string"; //what to display, e.g. mean, variance etc.
  in_types[2]= "int"; //the component to display
  in_types[3]= "bool"; //scale the output to byte range
  pro.set_input_types(in_types);

  //output
  out_types[0]= "vil_image_view_base_sptr";
  pro.set_output_types(out_types);
  return true;
}

//: Execute the process function
bool bbgm_display_dist_image_process(bprb_func_process& pro)
{
  // Sanity check
  if (!pro.verify_inputs()){
    std::cerr << "In bbgm_display_dist_image_process::execute() -"
             << " invalid inputs\n";
    return false;
  }

  // Retrieve background image
  bbgm_image_sptr bgm = pro.get_input<bbgm_image_sptr>(0);
  if (!bgm){
    std::cerr << "In bbgm_display_dist_image_process::execute() -"
             << " null distribution image\n";
    return false;
  }

  //Retrieve attribute to display, e.g. mean
  std::string attr = pro.get_input<std::string>(1);

  //Retrieve component index
  int comp_index = pro.get_input<int>(2);

  //Retrieve scale switch
  bool scale = pro.get_input<bool>(3);

  std::vector<std::string> output_types(1);
  output_types[0]= "vil_image_view_base_sptr";
  pro.set_output_types(output_types);

  bbgm_viewer_sptr viewer;
  if (attr=="mean") {
    viewer = new bbgm_mean_viewer();
    register_mean_viewers();
  }
  else if (attr == "variance"||attr == "std_dev") {
    viewer = new bbgm_variance_viewer();
    register_variance_viewers();
  }
  else if (attr == "weight") {
    viewer = new bbgm_weight_viewer();
    register_weight_viewers();
  }
  else {
    std::cout << "In bbgm_display_dist_image_process::execute() -"
             << " display attribute not available\n";
    return false;
  }
  if (!viewer->probe(bgm)){
    std::cout << "In bbgm_display_dist_image_process::execute() -"
             << " displayer cannot process distribution image type\n"
             << bgm->is_a() << '\n';
    return false;
  }
  viewer->set_active_component(comp_index);
  vil_image_view<double> d_image;
  if (!viewer->apply(bgm, d_image)){
    std::cout << "In bbgm_display_dist_image_process::execute() -"
             << " extract view (apply) failed\n";
    return false;
  }
  brdb_value_sptr output0;
  //convert to a byte image for display
  vil_image_view<vxl_byte> byte_image;
  if (attr == "variance"||attr == "std_dev"){
    if (attr == "std_dev") vil_math_sqrt(d_image);
    if (scale){
      double dmin, dmax;
      vil_math_value_range(d_image, dmin, dmax);
      vil_convert_stretch_range_limited(d_image, byte_image, dmin, dmax);
      output0 =
        new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<vxl_byte>(byte_image));
    }
    else{
    vil_image_view<float> fimg;
    vil_convert_cast(d_image, fimg);
    output0 =
    new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<float>(fimg));
    }
  }
  else if (scale){
    vil_convert_stretch_range_limited(d_image, byte_image, 0.0, 1.0);
    output0 =
    new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<vxl_byte>(byte_image));
  }
  else{
    vil_image_view<float> fimg;
    vil_convert_cast(d_image, fimg);
    output0 =
    new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<float>(fimg));
  }
  pro.set_output(0, output0);
  return true;
}
