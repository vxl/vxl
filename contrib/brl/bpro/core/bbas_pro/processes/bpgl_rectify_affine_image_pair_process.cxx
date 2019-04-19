//This is brl/bpro/core/bbas_pro/processes/bpgl_rectify_affine_image_pair_process.cxx
#include <string>
#include <iostream>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vpgl/vpgl_camera_double_sptr.h>
#include <vpgl/vpgl_affine_camera.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vgl/vgl_box_3d.h>

#include <bpgl/algo/bpgl_rectify_affine_image_pair.h>


//: process to rectify affine image pair
//
// bpgl_rectify_affine_image_pair(vil_image_resource_sptr const& resc0, vpgl_affine_camera<double> const& acam0,
//                                vil_image_resource_sptr const& resc1, vpgl_affine_camera<double> const& acam1);
//
// bool process(vgl_box_3d<double>const& scene_box )
//
// const vpgl_affine_camera<double>& rect_acam0()const{return rect_acam0_;}
// const vpgl_affine_camera<double>& rect_acam1()const{return rect_acam1_;}
// const vil_image_view<float>& rectified_fview0() const{return rect_fview0_;}
// const vil_image_view<float>& rectified_fview1() const{return rect_fview1_;}

namespace bpgl_rectify_affine_image_pair_process_globals
{
  unsigned n_inputs_  = 13;
  unsigned n_outputs_ = 4;
}


bool bpgl_rectify_affine_image_pair_process_cons(bprb_func_process& pro)
{
  using namespace bpgl_rectify_affine_image_pair_process_globals;

  std::vector<std::string> input_types_;
  input_types_.emplace_back("vil_image_view_base_sptr");  // image0
  input_types_.emplace_back("vpgl_camera_double_sptr");   // camera0
  input_types_.emplace_back("vil_image_view_base_sptr");  // image0
  input_types_.emplace_back("vpgl_camera_double_sptr");   // camera0
  input_types_.emplace_back("double");     // min point x (e.g. lower left corner of a scene bbox)
  input_types_.emplace_back("double");     // min point y
  input_types_.emplace_back("double");     // min point z
  input_types_.emplace_back("double");     // max point x (e.g. upper right corner of a scene bbox)
  input_types_.emplace_back("double");     // max point y
  input_types_.emplace_back("double");     // max point z
  input_types_.emplace_back("unsigned");   // n_points -- randomly sample this many points form the voxel volume, e.g. 100
  input_types_.emplace_back("vcl_string"); // output file for H0 rectification
  input_types_.emplace_back("vcl_string"); // output file for H1 rectification

  std::vector<std::string> output_types_;
  output_types_.emplace_back("vil_image_view_base_sptr"); // rectified image0
  output_types_.emplace_back("vpgl_camera_double_sptr");  // rectified camera0
  output_types_.emplace_back("vil_image_view_base_sptr"); // rectified image1
  output_types_.emplace_back("vpgl_camera_double_sptr");  // rectified camera1

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


bool bpgl_rectify_affine_image_pair_process(bprb_func_process& pro)
{
  using namespace bpgl_rectify_affine_image_pair_process_globals;

  // sanity check
  if (!pro.verify_inputs()) {
    std::cerr << pro.name() << ": wrong inputs!!!\n";
    return false;
  }

  // reusable variables
  unsigned i = 0;
  bool success = false;

  // get inputs
  i = 0;
  auto image0_sptr  = pro.get_input<vil_image_view_base_sptr>(i++);
  auto camera0_sptr = pro.get_input<vpgl_camera_double_sptr>(i++);
  auto image1_sptr  = pro.get_input<vil_image_view_base_sptr>(i++);
  auto camera1_sptr = pro.get_input<vpgl_camera_double_sptr>(i++);
  auto min_x        = pro.get_input<double>(i++);
  auto min_y        = pro.get_input<double>(i++);
  auto min_z        = pro.get_input<double>(i++);
  auto max_x        = pro.get_input<double>(i++);
  auto max_y        = pro.get_input<double>(i++);
  auto max_z        = pro.get_input<double>(i++);
  auto n_points     = pro.get_input<unsigned>(i++);
  auto file_H0      = pro.get_input<std::string>(i++);
  auto file_H1      = pro.get_input<std::string>(i++);


  // convert cameras
  auto* camera0_ptr = dynamic_cast<vpgl_affine_camera<double>*> (camera0_sptr.as_pointer());
  if (!camera0_ptr) {
    std::cerr << pro.name() << " :-- camera 0 is not affine" << std::endl;
    return false;
  }

  auto* camera1_ptr = dynamic_cast<vpgl_affine_camera<double>*> (camera1_sptr.as_pointer());
  if (!camera1_ptr) {
    std::cerr << pro.name() << " :-- camera 1 is not affine" << std::endl;
    return false;
  }

  // bounding box
  vgl_box_3d<double> scene_box(min_x,min_y,min_z, max_x,max_y,max_z);

  // process
  bpgl_rectify_affine_image_pair rectify_object;

  success = rectify_object.set_images_and_cams(image0_sptr, *camera0_ptr, image1_sptr, *camera1_ptr);
  if (!success) {
    std::cerr << pro.name() << " :-- set_images_and_cams failed" << std::endl;
    return false;
  }
  rectify_params rp;
  rp.n_points_ = n_points;
  rectify_object.set_params(rp);
  success = rectify_object.process(scene_box);
  if (!success) {
    std::cerr << pro.name() << " :-- process failed" << std::endl;
    return false;
  }

  // write rectification homographies to file (optional)
  if (!file_H0.empty()) {
    std::ofstream ofs0(file_H0.c_str());
    ofs0 << rectify_object.H0();
    ofs0.close();
  }

  if (!file_H1.empty()) {
    std::ofstream ofs1(file_H1.c_str());
    ofs1 << rectify_object.H1();
    ofs1.close();
  }

  // return
  i = 0;
  pro.set_output_val<vil_image_view_base_sptr>(i++,
      new vil_image_view<float>(rectify_object.rectified_fview0()));
  pro.set_output_val<vpgl_camera_double_sptr>(i++,
      new vpgl_affine_camera<double>(rectify_object.rect_acam0()));
  pro.set_output_val<vil_image_view_base_sptr>(i++,
      new vil_image_view<float>(rectify_object.rectified_fview1()));
  pro.set_output_val<vpgl_camera_double_sptr>(i++,
      new vpgl_affine_camera<double>(rectify_object.rect_acam1()));
  return true;
}
