// This is brl/bseg/bbgm/pro/bbgm_display_dist_image_process.cxx
#include "bbgm_display_dist_image_process.h"
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
#include <bsta/bsta_histogram.h>
#include <bbgm/bbgm_viewer.h>
#include <bbgm/bbgm_viewer_sptr.h>

//: Constructor
bbgm_display_dist_image_process::bbgm_display_dist_image_process()
{
  //input
  input_data_.resize(3,brdb_value_sptr(0));
  input_types_.resize(3);
  input_types_[0]= "bbgm_image_sptr"; //background image
  input_types_[1]= "vcl_string"; //what to display, e.g. mean, variance etc.
  input_types_[2]= "int"; //the component to display

  //output
  output_data_.resize(1, brdb_value_sptr(0));
  output_types_.resize(1);
  output_types_[0]= "vil_image_view_base_sptr";
}


//: Destructor
bbgm_display_dist_image_process::~bbgm_display_dist_image_process()
{
}


//: Execute the process
bool
bbgm_display_dist_image_process::execute()
{
  // Sanity check
  if (!this->verify_inputs()){
    vcl_cerr << "In bbgm_display_dist_image_process::execute() -"
             << " invalid inputs\n";
    return false;
  }
  // Retrieve background image
  brdb_value_t<bbgm_image_sptr>* input0 =
    static_cast<brdb_value_t<bbgm_image_sptr>* >(input_data_[0].ptr());

  bbgm_image_sptr bgm = input0->value();
  if (!bgm){
    vcl_cerr << "In bbgm_display_dist_image_process::execute() -"
             << " null distribution image\n";
    return false;
  }

  //Retrieve attribute to display, e.g. mean
  brdb_value_t<vcl_string>* input1 =
    static_cast<brdb_value_t<vcl_string>* >(input_data_[1].ptr());
  vcl_string attr = input1->value();

  //Retrieve component index
  brdb_value_t<int>* input2 =
    static_cast<brdb_value_t<int>* >(input_data_[2].ptr());
  int comp_index = input2->value();

  bbgm_viewer_sptr viewer;
  if (attr=="mean")
    viewer = new bbgm_mean_viewer();
  else if (attr == "variance")
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

  //convert to a byte image for display
  vil_image_view<vxl_byte> byte_image;
    vil_convert_stretch_range_limited(d_image, byte_image,
                                      0.0, 1.0);

  brdb_value_sptr output0 =
    new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<vxl_byte>(byte_image));
  output_data_[0] = output0;

  return true;
}

