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
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr");
  input_types.emplace_back("unsigned");
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //this process has 1 outputs
  // output(0): the output image with the specified number of planes
  std::vector<std::string> output_types;
  output_types.emplace_back("vil_image_view_base_sptr");  // label image
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}


//: Execute the process
bool vil_convert_to_n_planes_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 2) {
    std::cout << "vil_convert_to_n_planes_process: The input number should be 2" << std::endl;
    return false;
  }

  // get the inputs
  unsigned i=0;
  //Retrieve image from input
  vil_image_view_base_sptr img = pro.get_input<vil_image_view_base_sptr>(i++);
  auto nplanes = pro.get_input<unsigned>(i++);

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
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr");
  input_types.emplace_back("unsigned");
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //this process has 1 outputs
  // output(0): the output image with the specified number of planes
  std::vector<std::string> output_types;
  output_types.emplace_back("vil_image_view_base_sptr");  // single plane image
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}


//: Execute the process
bool vil_get_plane_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 2) {
    std::cout << "vil_convert_to_n_planes_process: The input number should be 2" << std::endl;
    return false;
  }

  // get the inputs
  unsigned i=0;
  //Retrieve image from input
  vil_image_view_base_sptr img = pro.get_input<vil_image_view_base_sptr>(i++);
  auto plane_id = pro.get_input<unsigned>(i++);

  if (plane_id >= img->nplanes()) {
    std::cerr << "In vil_get_plane_process() - input image does not have a plane with id: " << plane_id << '\n';
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
  } else if (img->pixel_format() == VIL_PIXEL_FORMAT_UINT_16) {
    vil_image_view<vxl_uint_16> img_f(img);
    vil_image_view<vxl_uint_16> band = vil_plane(img_f, plane_id);
    vil_image_view_base_sptr out_img_ptr = new vil_image_view<vxl_uint_16>(band);
    pro.set_output_val<vil_image_view_base_sptr>(0, out_img_ptr);
  } else if (img->pixel_format() == VIL_PIXEL_FORMAT_RGBA_UINT_16) {
    vil_image_view<vil_rgba<vxl_uint_16> > img_f(img);
    vil_image_view<vxl_uint_16> band = vil_plane(img_f, plane_id);
    vil_image_view_base_sptr out_img_ptr = new vil_image_view<vxl_uint_16>(band);
    pro.set_output_val<vil_image_view_base_sptr>(0, out_img_ptr);
  } else if (img->pixel_format() == VIL_PIXEL_FORMAT_RGB_BYTE) {
    vil_image_view<vil_rgb<vxl_byte> > img_f(img);
    vil_image_view<vxl_byte> band = vil_plane(img_f, plane_id);
    vil_image_view_base_sptr out_img_ptr = new vil_image_view<vxl_byte>(band);
    pro.set_output_val<vil_image_view_base_sptr>(0, out_img_ptr);
  }
  else {
    std::cerr << "In vil_get_plane_process() - unsupport image pixel format: " << img->pixel_format() << "!\n";
    return false;
  }

  return true;
}


bool vil_get_number_of_planes_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr");
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  std::vector<std::string> output_types;
  output_types.emplace_back("unsigned");  // return number of planes
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

//: Execute the process
bool vil_get_number_of_planes_process(bprb_func_process& pro)
{
  if (pro.n_inputs()< 1) {
    std::cout << "vil_get_number_of_planes_process: The input number should be 1" << std::endl;
    return false;
  }
  unsigned i=0;
  vil_image_view_base_sptr img = pro.get_input<vil_image_view_base_sptr>(i++);
  pro.set_output_val<unsigned>(0, img->nplanes());
  return true;
}

bool vil_combine_planes_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr");  // red plane
  input_types.emplace_back("vil_image_view_base_sptr");  // green plane
  input_types.emplace_back("vil_image_view_base_sptr");  // blue
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //this process has 1 outputs
  // output(0): the output image with the specified number of planes
  std::vector<std::string> output_types;
  output_types.emplace_back("vil_image_view_base_sptr");  // RGB image
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

//: Execute the process
bool vil_combine_planes_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 3) {
    std::cout << "vil_combine_planes_process: The input number should be 3" << std::endl;
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
  } else if (img_r->pixel_format() == VIL_PIXEL_FORMAT_UINT_16) {
    vil_image_view<vxl_uint_16> img_out(img_r->ni(), img_r->nj(), 3);
    vil_image_view<vxl_uint_16> img_rr(img_r);
    vil_image_view<vxl_uint_16> img_gg(img_g);
    vil_image_view<vxl_uint_16> img_bb(img_b);
    for (unsigned i = 0; i < img_r->ni(); i++)
      for (unsigned j = 0; j < img_r->nj(); j++) {
        img_out(i,j,0) = img_rr(i,j);
        img_out(i,j,1) = img_gg(i,j);
        img_out(i,j,2) = img_bb(i,j);
      }

    pro.set_output_val<vil_image_view_base_sptr>(0, new vil_image_view<vxl_uint_16>(img_out));
  } else {
    std::cerr << "In vil_combine_planes_process() - for now only supports FLOAT format!\n";
    return false;
  }


  return true;
}

bool vil_combine_planes_process2_cons(bprb_func_process& pro)
{
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr");  // blue plane
  input_types.emplace_back("vil_image_view_base_sptr");  // green plane
  input_types.emplace_back("vil_image_view_base_sptr");  // red
  input_types.emplace_back("vil_image_view_base_sptr");  // nir
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //this process has 1 outputs
  // output(0): the output image with the specified number of planes
  std::vector<std::string> output_types;
  output_types.emplace_back("vil_image_view_base_sptr");  // output 4-band image
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

//: Execute the process
bool vil_combine_planes_process2(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 4) {
    std::cout << "vil_combine_planes_process2: The input number should be 4" << std::endl;
    return false;
  }

  // get the inputs
  unsigned i=0;
  //Retrieve image from input
  vil_image_view_base_sptr img_b = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr img_g = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr img_r = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr img_nir = pro.get_input<vil_image_view_base_sptr>(i++);

  if (img_r->pixel_format() == VIL_PIXEL_FORMAT_FLOAT) {
    vil_image_view<float> img_out(img_r->ni(), img_r->nj(), 4);
    vil_image_view<float> img_rr(img_r);
    vil_image_view<float> img_gg(img_g);
    vil_image_view<float> img_bb(img_b);
    vil_image_view<float> img_nirr(img_nir);
    for (unsigned i = 0; i < img_r->ni(); i++)
      for (unsigned j = 0; j < img_r->nj(); j++) {
        img_out(i,j,0) = img_bb(i,j);
        img_out(i,j,1) = img_gg(i,j);
        img_out(i,j,2) = img_rr(i,j);
        img_out(i,j,3) = img_nirr(i,j);
      }
    pro.set_output_val<vil_image_view_base_sptr>(0, new vil_image_view<float>(img_out));
  } else if (img_r->pixel_format() == VIL_PIXEL_FORMAT_BYTE) {
    vil_image_view<vxl_byte> img_out(img_r->ni(), img_r->nj(), 4);
    vil_image_view<vxl_byte> img_rr(img_r);
    vil_image_view<vxl_byte> img_gg(img_g);
    vil_image_view<vxl_byte> img_bb(img_b);
    vil_image_view<vxl_byte> img_nirr(img_nir);
    for (unsigned i = 0; i < img_r->ni(); i++)
      for (unsigned j = 0; j < img_r->nj(); j++) {
        img_out(i,j,0) = img_bb(i,j);
        img_out(i,j,1) = img_gg(i,j);
        img_out(i,j,2) = img_rr(i,j);
        img_out(i,j,3) = img_nirr(i,j);
      }
    pro.set_output_val<vil_image_view_base_sptr>(0, new vil_image_view<vxl_byte>(img_out));
  } else if (img_r->pixel_format() == VIL_PIXEL_FORMAT_UINT_16) {
    vil_image_view<vxl_uint_16> img_out(img_r->ni(), img_r->nj(), 4);
    vil_image_view<vxl_uint_16> img_rr(img_r);
    vil_image_view<vxl_uint_16> img_gg(img_g);
    vil_image_view<vxl_uint_16> img_bb(img_b);
    vil_image_view<vxl_uint_16> img_nirr(img_nir);
    for (unsigned i = 0; i < img_r->ni(); i++)
      for (unsigned j = 0; j < img_r->nj(); j++) {
        img_out(i,j,0) = img_bb(i,j);
        img_out(i,j,1) = img_gg(i,j);
        img_out(i,j,2) = img_rr(i,j);
        img_out(i,j,3) = img_nirr(i,j);
      }
    pro.set_output_val<vil_image_view_base_sptr>(0, new vil_image_view<vxl_uint_16>(img_out));
  } else {
    std::cerr << pro.name() << " -- unsupport image pixel format: " << img_r->pixel_format() << "!\n";
    return false;
  }


  return true;
}

//: process to combine 8 planes into a single image
/// Note that user is responsible for the sequence of the image planes
bool vil_combine_palnes_8_bands_process_cons(bprb_func_process& pro)
{
  // this process takes 8 inputs:
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr");  // coastal plane
  input_types.emplace_back("vil_image_view_base_sptr");  // blue plane
  input_types.emplace_back("vil_image_view_base_sptr");  // green plane
  input_types.emplace_back("vil_image_view_base_sptr");  // yellow plane
  input_types.emplace_back("vil_image_view_base_sptr");  // red plane
  input_types.emplace_back("vil_image_view_base_sptr");  // red edge plane
  input_types.emplace_back("vil_image_view_base_sptr");  // nir1
  input_types.emplace_back("vil_image_view_base_sptr");  // nir2

  // this process takes 1 outputs
  std::vector<std::string> output_types;
  output_types.emplace_back("vil_image_view_base_sptr");  // output image with 8 planes
  return pro.set_input_types(input_types) && pro.set_output_types(output_types);
}

bool vil_combine_palnes_8_bands_process(bprb_func_process& pro)
{
  // sanity check
  if (!pro.verify_inputs()) {
    std::cerr << pro.name() << ": invalid input!\n";
    return false;
  }

  // get the inputs
  unsigned in_i = 0;
  vil_image_view_base_sptr img_c    = pro.get_input<vil_image_view_base_sptr>(in_i++);
  vil_image_view_base_sptr img_b    = pro.get_input<vil_image_view_base_sptr>(in_i++);
  vil_image_view_base_sptr img_g    = pro.get_input<vil_image_view_base_sptr>(in_i++);
  vil_image_view_base_sptr img_y    = pro.get_input<vil_image_view_base_sptr>(in_i++);
  vil_image_view_base_sptr img_r    = pro.get_input<vil_image_view_base_sptr>(in_i++);
  vil_image_view_base_sptr img_re   = pro.get_input<vil_image_view_base_sptr>(in_i++);
  vil_image_view_base_sptr img_nir1 = pro.get_input<vil_image_view_base_sptr>(in_i++);
  vil_image_view_base_sptr img_nir2 = pro.get_input<vil_image_view_base_sptr>(in_i++);
  unsigned ni = img_c->ni();
  unsigned nj = img_c->nj();

  if (img_c->pixel_format() == VIL_PIXEL_FORMAT_FLOAT)
  {
    vil_image_view<float> img_out(img_c->ni(), img_r->nj(), 8);
    std::vector<vil_image_view<float> > imgs_vec;
    imgs_vec.emplace_back(img_c);     imgs_vec.emplace_back(img_b);
    imgs_vec.emplace_back(img_g);     imgs_vec.emplace_back(img_y);
    imgs_vec.emplace_back(img_r);     imgs_vec.emplace_back(img_re);
    imgs_vec.emplace_back(img_nir1);  imgs_vec.emplace_back(img_nir2);
    for (unsigned i = 0; i < ni; i++)
      for (unsigned j = 0; j < nj; j++)
        for (unsigned p = 0; p < 8; p++)
          img_out(i,j,p) = imgs_vec[p](i,j);
    pro.set_output_val<vil_image_view_base_sptr>(0, new vil_image_view<float>(img_out));
  }
  else if (img_c->pixel_format() == VIL_PIXEL_FORMAT_BYTE)
  {
    vil_image_view<vxl_byte> img_out(img_c->ni(), img_r->nj(), 8);
    std::vector<vil_image_view<vxl_byte> > imgs_vec;
    imgs_vec.emplace_back(img_c);     imgs_vec.emplace_back(img_b);
    imgs_vec.emplace_back(img_g);     imgs_vec.emplace_back(img_y);
    imgs_vec.emplace_back(img_r);     imgs_vec.emplace_back(img_re);
    imgs_vec.emplace_back(img_nir1);  imgs_vec.emplace_back(img_nir2);
    for (unsigned i = 0; i < ni; i++)
      for (unsigned j = 0; j < nj; j++)
        for (unsigned p = 0; p < 8; p++)
          img_out(i,j,p) = imgs_vec[p](i,j);
    pro.set_output_val<vil_image_view_base_sptr>(0, new vil_image_view<vxl_byte>(img_out));
  }
  else if (img_c->pixel_format() == VIL_PIXEL_FORMAT_UINT_16)
  {
    vil_image_view<vxl_uint_16> img_out(img_c->ni(), img_r->nj(), 8);
    std::vector<vil_image_view<vxl_uint_16> > imgs_vec;
    imgs_vec.emplace_back(img_c);     imgs_vec.emplace_back(img_b);
    imgs_vec.emplace_back(img_g);     imgs_vec.emplace_back(img_y);
    imgs_vec.emplace_back(img_r);     imgs_vec.emplace_back(img_re);
    imgs_vec.emplace_back(img_nir1);  imgs_vec.emplace_back(img_nir2);
    for (unsigned i = 0; i < ni; i++)
      for (unsigned j = 0; j < nj; j++)
        for (unsigned p = 0; p < 8; p++)
          img_out(i,j,p) = imgs_vec[p](i,j);
    pro.set_output_val<vil_image_view_base_sptr>(0, new vil_image_view<vxl_uint_16>(img_out));
  }
  else if (img_c->pixel_format() == VIL_PIXEL_FORMAT_INT_16)
  {
    vil_image_view<vxl_int_16> img_out(img_c->ni(), img_r->nj(), 8);
    std::vector<vil_image_view<vxl_int_16> > imgs_vec;
    imgs_vec.emplace_back(img_c);     imgs_vec.emplace_back(img_b);
    imgs_vec.emplace_back(img_g);     imgs_vec.emplace_back(img_y);
    imgs_vec.emplace_back(img_r);     imgs_vec.emplace_back(img_re);
    imgs_vec.emplace_back(img_nir1);  imgs_vec.emplace_back(img_nir2);
    for (unsigned i = 0; i < ni; i++)
      for (unsigned j = 0; j < nj; j++)
        for (unsigned p = 0; p < 8; p++)
          img_out(i,j,p) = imgs_vec[p](i,j);
    pro.set_output_val<vil_image_view_base_sptr>(0, new vil_image_view<vxl_int_16>(img_out));
  }
  else {
    std::cerr << pro.name() << ": unsupported input image pixel format: " << img_c->pixel_format() << "!\n";
    return false;
  }
  return true;
}
