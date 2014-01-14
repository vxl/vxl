// This is brl/bpro/core/sdet_pro/processes/sdet_filter_bank_processes.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#include <brdb/brdb_value.h>
#include <sdet/sdet_texture_classifier_params.h>
#include <sdet/sdet_texture_classifier.h>
#include <vil/vil_image_view.h>
#include <vnl/vnl_random.h>
#include <vil/vil_math.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_plane.h>
#include <vil/vil_resample_bilin.h>
#include <vil/vil_convert.h>

//: this process extracts the filter bank of an image (assumes a float image in the range [0, 1])
//  and saves it in the filter_bank object of the passed texture_classifier instance
//  the params of the classifier instance are used
bool sdet_extract_filter_bank_process_cons(bprb_func_process& pro)
{
  vcl_vector<vcl_string> input_types;
  input_types.push_back("sdet_texture_classifier_sptr"); // classifier instance
  input_types.push_back("vcl_string"); // input image name
  input_types.push_back("vcl_string");   // filter bank folder
  if (!pro.set_input_types(input_types))
    return false;

  vcl_vector<vcl_string> output_types;
  return pro.set_output_types(output_types);
}

bool sdet_extract_filter_bank_process(bprb_func_process& pro)
{
  if (!pro.verify_inputs())
  {
    vcl_cout << pro.name() << "sdet_extract_filter_bank_process inputs are not valid"<< vcl_endl;
    return false;
  }
  // get inputs
  sdet_texture_classifier_sptr tc_ptr = pro.get_input<sdet_texture_classifier_sptr>(0);
  vcl_string name = pro.get_input<vcl_string>(1);
  vcl_string folder = pro.get_input<vcl_string>(2);

  // assumes to load a float image in [0,1] using the name, name should be full path to the image
  if (!tc_ptr->compute_filter_bank_float_img(folder, name))
  {
    vcl_cout << "problems computing filter bank on the image!\n";
    return false;
  }
  
  return true;
}


//: this process extracts a "gauss" band from the input image and adds that as another layer to the filter_bank of the passed classifier
//  practically increases the dimension of the textons
//  the params of the classifier instance are used
bool sdet_add_to_filter_bank_process_cons(bprb_func_process& pro)
{
  vcl_vector<vcl_string> input_types;
  input_types.push_back("sdet_texture_classifier_sptr"); // classifier instance
  input_types.push_back("vcl_string");   // input image name
  input_types.push_back("unsigned");     // the plane to extract the filters from
  input_types.push_back("vcl_string");   // filter bank folder
  input_types.push_back("bool");         // option to turn on gauss smoothing on image
  if (!pro.set_input_types(input_types))
    return false;

  vcl_vector<vcl_string> output_types;
  return pro.set_output_types(output_types);
}

bool sdet_add_to_filter_bank_process(bprb_func_process& pro)
{
  if (!pro.verify_inputs())
  {
    vcl_cout << pro.name() << "sdet_extract_filter_bank_process inputs are not valid"<< vcl_endl;
    return false;
  }
  // get inputs
  sdet_texture_classifier_sptr tc_ptr = pro.get_input<sdet_texture_classifier_sptr>(0);
  vcl_string name = pro.get_input<vcl_string>(1);
  unsigned n = pro.get_input<unsigned>(2);
  vcl_stringstream res_name; res_name << n;
  vcl_string folder = pro.get_input<vcl_string>(3);
  bool is_smooth = pro.get_input<bool>(4);

  vil_image_view_base_sptr img_sptr = vil_load(name.c_str());

  vil_image_view<float> img_f;
  if (vil_image_view<vxl_byte>* img_ptr = dynamic_cast<vil_image_view<vxl_byte>*>(img_sptr.ptr())) {
    vcl_cout << " loaded image, ni: " << img_ptr->ni() << " " << img_ptr->nj() << " nplanes: " << img_ptr->nplanes()
             << " with pixel format: " << img_ptr->pixel_format() << vcl_endl;
    vil_image_view<vxl_byte> img_band = vil_plane(*img_ptr, n);
    vil_convert_stretch_range_limited(img_band, img_f, (vxl_byte)0, (vxl_byte)255, 0.0f, 1.0f);
  }else if (vil_image_view<float>* img_ptr = dynamic_cast<vil_image_view<float>*>(img_sptr.ptr())) {
    vcl_cout << " loaded image, ni: " << img_ptr->ni() << " " << img_ptr->nj() << " nplanes: " << img_ptr->nplanes() 
             << " with pixel format: " << img_ptr->pixel_format() << vcl_endl;
    img_f = vil_plane(*img_ptr, n);
  }else {
    vcl_cout << "Error: " << pro.name() << " -- The image pixel format: " << img_sptr->pixel_format() << " is not supported!\n";
    return false;
  }

  if (is_smooth)
    vcl_cout << " gauss smooth is applied on the input image" << vcl_endl;

  unsigned tni = tc_ptr->filter_responses().ni();
  unsigned tnj = tc_ptr->filter_responses().nj();
  if (tni != img_f.ni() || tnj != img_f.nj()) {
    vcl_cout << "filter responses have ni: " << tni << " nj: " << tnj << "..";
    vcl_cout << " input image has ni: " << img_f.ni() << " nj: " << img_f.nj() << "! resampling..\n";
    
    vil_image_view<float> out_img(tni, tnj);
    vil_resample_bilin(img_f, out_img, tni, tnj);
    tc_ptr->add_gauss_response(out_img, folder, name, res_name.str(), is_smooth);
  } else
    tc_ptr->add_gauss_response(img_f, folder, name, res_name.str(), is_smooth);
  
  return true;
}
