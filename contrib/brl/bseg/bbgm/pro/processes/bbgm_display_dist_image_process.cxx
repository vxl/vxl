// This is brl/bseg/bbgm/pro/processes/bbgm_display_dist_image_process.cxx

//:
// \file
#include <bprb/bprb_func_process.h>
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
#include <bbgm/bbgm_viewer.h>
#include <bbgm/bbgm_viewer_sptr.h>

// constructor function
bool bbgm_display_dist_image_process_cons(bprb_func_process& pro)
{
  //input
  vcl_vector<vcl_string> in_types(4), out_types(1);
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
    vcl_cerr << "In bbgm_display_dist_image_process::execute() -"
             << " invalid inputs\n";
    return false;
  }

  // Retrieve background image
  bbgm_image_sptr bgm = pro.get_input<bbgm_image_sptr>(0);
  if (!bgm){
    vcl_cerr << "In bbgm_display_dist_image_process::execute() -"
             << " null distribution image\n";
    return false;
  }

  //Retrieve attribute to display, e.g. mean
  vcl_string attr = pro.get_input<vcl_string>(1);

  //Retrieve component index
  int comp_index = pro.get_input<int>(2);

  //Retrieve scale switch
  bool scale = pro.get_input<bool>(3);

  vcl_vector<vcl_string> output_types(1);
  output_types[0]= "vil_image_view_base_sptr";
  pro.set_output_types(output_types);

  bbgm_viewer_sptr viewer;
  if (attr=="mean")
    viewer = new bbgm_mean_viewer();
  else if (attr == "variance"||attr == "std_dev")
    viewer = new bbgm_variance_viewer();
  else if (attr == "weight")
    viewer = new bbgm_weight_viewer();
  else{
    vcl_cout << "In bbgm_display_dist_image_process::execute() -"
             << " display attribute not available\n";
    return false;
  }
  if (!viewer->probe(bgm)){
    vcl_cout << "In bbgm_display_dist_image_process::execute() -"
             << " displayer cannot process distribution image type\n"
             << bgm->is_a() << '\n';
    return false;
  }
  viewer->set_active_component(comp_index);
  vil_image_view<double> d_image;
  if (!viewer->apply(bgm, d_image)){
    vcl_cout << "In bbgm_display_dist_image_process::execute() -"
             << " extract view (apply) failed\n";
    return false;
  }
  brdb_value_sptr output0;
  //convert to a byte image for display
  vil_image_view<vxl_byte> byte_image;
  if(attr == "variance"||attr == "std_dev"){
    if(attr == "std_dev") vil_math_sqrt(d_image);
    if(scale){
      double dmin, dmax;
      vil_math_value_range(d_image, dmin, dmax);
      vil_convert_stretch_range_limited(d_image, byte_image, dmin, dmax);
      output0 =
        new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<vxl_byte>(byte_image));
    }else{
    vil_image_view<float> fimg;
    vil_convert_cast(d_image, fimg);
    output0 =
    new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<float>(fimg));
    }
  }else if(scale){
    vil_convert_stretch_range_limited(d_image, byte_image, 0.0, 1.0);
    output0 =
    new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<vxl_byte>(byte_image));
  }else{
    vil_image_view<float> fimg;
    vil_convert_cast(d_image, fimg);
    output0 =
    new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<float>(fimg));
  }
  pro.set_output(0, output0);
  return true;
}

