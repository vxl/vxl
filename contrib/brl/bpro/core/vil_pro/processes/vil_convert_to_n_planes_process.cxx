// This is brl/bpro/core/vil_pro/processes/vil_convert_to_n_planes_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vil/vil_convert.h>
#include <vil/vil_image_view_base.h>

//: Constructor
bool vil_convert_to_n_planes_process_cons(bprb_func_process& pro)
{
  //this process takes two inputs:
  // input(0): the input image vil_image_view_base_sptr
  // input(1): the number of planes in the output image 
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vil_image_view_base_sptr"); 
  input_types.push_back("unsigned"); 
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //this process has 1 outputs
  // output(0): the output image with the specified number of planes
  vcl_vector<vcl_string> output_types;
  output_types.push_back("vil_image_view_base_sptr");  // label image
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}


//: Execute the process
bool vil_convert_to_n_planes_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 2) {
    vcl_cout << "vil_convert_to_n_planes_process: The input number should be 2" << vcl_endl;
    return false;
  }

  // get the inputs
  unsigned i=0;
  //Retrieve image from input
  vil_image_view_base_sptr img = pro.get_input<vil_image_view_base_sptr>(i++);
  unsigned nplanes = pro.get_input<unsigned>(i++);

  vil_image_view_base_sptr out_img_ptr = vil_convert_to_n_planes(nplanes, img);

  pro.set_output_val<vil_image_view_base_sptr>(0, out_img_ptr);
  return true;
}

bool vil_get_plane_process_cons(bprb_func_process& pro)
{
  //this process takes two inputs:
  // input(0): the input image vil_image_view_base_sptr
  // input(1): the plane id to return 
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vil_image_view_base_sptr"); 
  input_types.push_back("unsigned"); 
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //this process has 1 outputs
  // output(0): the output image with the specified number of planes
  vcl_vector<vcl_string> output_types;
  output_types.push_back("vil_image_view_base_sptr");  // single plane image
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}


//: Execute the process
bool vil_get_plane_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 2) {
    vcl_cout << "vil_convert_to_n_planes_process: The input number should be 2" << vcl_endl;
    return false;
  }

  // get the inputs
  unsigned i=0;
  //Retrieve image from input
  vil_image_view_base_sptr img = pro.get_input<vil_image_view_base_sptr>(i++);
  unsigned plane_id = pro.get_input<unsigned>(i++);

  if (plane_id >= img->nplanes()) {
    vcl_cerr << "In vil_get_plane_process() - input image does not have a plane with id: " << plane_id << '\n';
    return false;
  }

  if (img->pixel_format() == VIL_PIXEL_FORMAT_FLOAT) {
    vil_image_view<float> img_f(img);
    vil_image_view<float> band = vil_plane(img_f, plane_id);
    vil_image_view_base_sptr out_img_ptr = new vil_image_view<float>(band);
    pro.set_output_val<vil_image_view_base_sptr>(0, out_img_ptr);
  } else if (img->pixel_format() == VIL_PIXEL_FORMAT_BYTE) {
    vil_image_view<vxl_byte> img_f(img);
    vil_image_view<vxl_byte> band = vil_plane(img_f, plane_id);
    vil_image_view_base_sptr out_img_ptr = new vil_image_view<vxl_byte>(band);
    pro.set_output_val<vil_image_view_base_sptr>(0, out_img_ptr);
  } else {
    vcl_cerr << "In vil_get_plane_process() - for now only supports FLOAT format!\n";
    return false;
  }

  
  return true;
}


bool vil_combine_planes_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vil_image_view_base_sptr");  // red plane
  input_types.push_back("vil_image_view_base_sptr");  // green plane
  input_types.push_back("vil_image_view_base_sptr");  // blue
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //this process has 1 outputs
  // output(0): the output image with the specified number of planes
  vcl_vector<vcl_string> output_types;
  output_types.push_back("vil_image_view_base_sptr");  // RGB image
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

//: Execute the process
bool vil_combine_planes_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 3) {
    vcl_cout << "vil_combine_planes_process: The input number should be 3" << vcl_endl;
    return false;
  }

  // get the inputs
  unsigned i=0;
  //Retrieve image from input
  vil_image_view_base_sptr img_r = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr img_g = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr img_b = pro.get_input<vil_image_view_base_sptr>(i++);

  if (img_r->pixel_format() == VIL_PIXEL_FORMAT_FLOAT) {
    vil_image_view<float> img_out(img_r->ni(), img_r->nj(), 3);
    vil_image_view<float> img_rr(img_r);
    vil_image_view<float> img_gg(img_g);
    vil_image_view<float> img_bb(img_b);
    for (unsigned i = 0; i < img_r->ni(); i++)
      for (unsigned j = 0; j < img_r->nj(); j++) {
        img_out(i,j,0) = img_rr(i,j);
        img_out(i,j,1) = img_gg(i,j);
        img_out(i,j,2) = img_bb(i,j);
      }
    
    pro.set_output_val<vil_image_view_base_sptr>(0, new vil_image_view<float>(img_out));
  } else if (img_r->pixel_format() == VIL_PIXEL_FORMAT_BYTE) {
    vil_image_view<vxl_byte> img_out(img_r->ni(), img_r->nj(), 3);
    vil_image_view<vxl_byte> img_rr(img_r);
    vil_image_view<vxl_byte> img_gg(img_g);
    vil_image_view<vxl_byte> img_bb(img_b);
    for (unsigned i = 0; i < img_r->ni(); i++)
      for (unsigned j = 0; j < img_r->nj(); j++) {
        img_out(i,j,0) = img_rr(i,j);
        img_out(i,j,1) = img_gg(i,j);
        img_out(i,j,2) = img_bb(i,j);
      }
    
    pro.set_output_val<vil_image_view_base_sptr>(0, new vil_image_view<vxl_byte>(img_out));
  } else {
    vcl_cerr << "In vil_get_plane_process() - for now only supports FLOAT format!\n";
    return false;
  }

  
  return true;
}