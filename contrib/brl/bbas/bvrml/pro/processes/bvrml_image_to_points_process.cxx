//This is brl/bbas/bvrml/pro/processes/bvrml_image_to_points_process.cxx
#include <string>
#include <iostream>
#include <bprb/bprb_func_process.h>
//:
// \file This process takes two images as input, the first image is the probabilities of existencies of points at the corresponding x,y (i,j) locations
//       The second float image specifies the height of the point if any
//       a threshold value to threshold the first image
//
#include <bprb/bprb_parameters.h>

#ifdef DEBUG
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#endif

#include <brdb/brdb_value.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_sphere_3d.h>
#include <bvrml/bvrml_write.h>

#include <vil/vil_image_view.h>

//: sets input and output types
bool bvrml_image_to_points_process_cons(bprb_func_process& pro)
{
  //inputs
  std::vector<std::string> input_types_(5);
  input_types_[0] = "vil_image_view_base_sptr";
  input_types_[1] = "vil_image_view_base_sptr";
  input_types_[2] = "vcl_string";  // name of the output file
  input_types_[3] = "float";
  input_types_[4] = "float";  // max height

  //output
  std::vector<std::string> output_types_(1);
  output_types_[0] = "vil_image_view_base_sptr";

  bool good = pro.set_input_types(input_types_) &&
              pro.set_output_types(output_types_);

  brdb_value_sptr idxr = new brdb_value_t<float>(0.5f);  // assuming that xy_img is a probability image, set the default threshold to be 0.5
  pro.set_input(3, idxr);

  return good;
}

bool bvrml_image_to_points_process(bprb_func_process& pro)
{
  // check number of inputs
  if (!pro.verify_inputs())
  {
    std::cout << pro.name() << ": Invalid inputs" << std::endl;
    return false;
  }
    //get the inputs
  vil_image_view_base_sptr xy_img_sptr = pro.get_input<vil_image_view_base_sptr>(0);
  vil_image_view_base_sptr z_img_sptr = pro.get_input<vil_image_view_base_sptr>(1);
  std::string fname = pro.get_input<std::string>(2);
  auto thres = pro.get_input<float>(3);
  auto max_height = pro.get_input<float>(4);

  std::ofstream ofs(fname.c_str());

  bvrml_write::write_vrml_header(ofs);

  vil_image_view<float> xy_img(xy_img_sptr);
  vil_image_view<float> z_img(z_img_sptr);

  vil_image_view<vxl_byte> out_img(z_img_sptr->ni(), z_img_sptr->nj());
  out_img.fill(0);

  for (unsigned i = 0; i < xy_img.ni(); i++)
    for (unsigned j = 0; j < xy_img.nj(); j++) {
      if (xy_img(i,j) > thres) {
        vgl_sphere_3d<float> point(i, j, z_img(i,j), 0.8f);
        bvrml_write::write_vrml_sphere(ofs, point, 0.0f, 0.0f, 1.0f);
        float height = z_img(i,j) > max_height ? max_height : max_height-z_img(i,j);
        out_img(i,j) = (vxl_byte)height;
      }
    }

  ofs.close();

  pro.set_output_val<vil_image_view_base_sptr>(0, new vil_image_view<vxl_byte>(out_img));

  return true;
}
