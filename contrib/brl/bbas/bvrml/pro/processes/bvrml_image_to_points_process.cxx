//This is brl/bbas/bvrml/pro/processes/bvrml_image_to_points_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file This process takes two images as input, the first image is the probabilities of existencies of points at the corresponding x,y (i,j) locations
//       The second float image specifies the height of the point if any
//       a threshold value to threshold the first image
//
#include <bprb/bprb_parameters.h>

#include <vcl_string.h>
#ifdef DEBUG
#include <vcl_iostream.h>
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
  vcl_vector<vcl_string> input_types_(4);
  input_types_[0] = "vil_image_view_base_sptr";
  input_types_[1] = "vil_image_view_base_sptr";
  input_types_[2] = "vcl_string";  // name of the output file
  input_types_[3] = "float";
  
  //output
  vcl_vector<vcl_string> output_types_(0);

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
    vcl_cout << pro.name() << ": Invalid inputs" << vcl_endl;
    return false;
  }
    //get the inputs
  vil_image_view_base_sptr xy_img_sptr = pro.get_input<vil_image_view_base_sptr>(0);
  vil_image_view_base_sptr z_img_sptr = pro.get_input<vil_image_view_base_sptr>(1);
  vcl_string fname = pro.get_input<vcl_string>(2);
  float thres = pro.get_input<float>(3);
  
  vcl_ofstream ofs(fname.c_str());

  bvrml_write::write_vrml_header(ofs);

  vil_image_view<float> xy_img(xy_img_sptr);
  vil_image_view<float> z_img(z_img_sptr);
  for (unsigned i = 0; i < xy_img.ni(); i++)
    for (unsigned j = 0; j < xy_img.nj(); j++) {
      if (xy_img(i,j) > thres) {
        vgl_sphere_3d<float> point(i, j, z_img(i,j), 0.8f);
        bvrml_write::write_vrml_sphere(ofs, point, 0.0f, 0.0f, 1.0f);
      }
    }
  
  ofs.close();
  return true;
}

