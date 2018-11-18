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
  std::vector<std::string> input_types;
  input_types.emplace_back("sdet_texture_classifier_sptr"); // classifier instance
  input_types.emplace_back("vcl_string"); // input image name
  input_types.emplace_back("vcl_string");   // filter bank folder
  if (!pro.set_input_types(input_types))
    return false;

  std::vector<std::string> output_types;
  return pro.set_output_types(output_types);
}

bool sdet_extract_filter_bank_process(bprb_func_process& pro)
{
  if (!pro.verify_inputs())
  {
    std::cout << pro.name() << "sdet_extract_filter_bank_process inputs are not valid"<< std::endl;
    return false;
  }
  // get inputs
  sdet_texture_classifier_sptr tc_ptr = pro.get_input<sdet_texture_classifier_sptr>(0);
  std::string name = pro.get_input<std::string>(1);
  std::string folder = pro.get_input<std::string>(2);

  // assumes to load a float image in [0,1] using the name, name should be full path to the image
  if (!tc_ptr->compute_filter_bank_float_img(folder, name))
  {
    std::cout << "problems computing filter bank on the image!\n";
    return false;
  }

  return true;
}

//: this process extracts the filter bank of an image (assumes a float image in the range [0, 1])
//  and saves it in the filter_bank object of the passed texture_classifier instance
//  the params of the classifier instance are used
bool sdet_extract_filter_bank_img_process_cons(bprb_func_process& pro)
{
  std::vector<std::string> input_types;
  input_types.emplace_back("sdet_texture_classifier_sptr"); // classifier instance
  input_types.emplace_back("vil_image_view_base_sptr"); // input image
  if (!pro.set_input_types(input_types))
    return false;

  std::vector<std::string> output_types;
  return pro.set_output_types(output_types);
}

bool sdet_extract_filter_bank_img_process(bprb_func_process& pro)
{
  if (!pro.verify_inputs())
  {
    std::cout << pro.name() << "sdet_extract_filter_bank_process inputs are not valid"<< std::endl;
    return false;
  }
  // get inputs
  sdet_texture_classifier_sptr tc_ptr = pro.get_input<sdet_texture_classifier_sptr>(0);
  vil_image_view_base_sptr img_ptr = pro.get_input<vil_image_view_base_sptr>(1);
  vil_image_view<float> img(img_ptr);

  // assumes to load a float image in [0,1] using the name, name should be full path to the image
  if (!tc_ptr->compute_filter_bank(img))
  {
    std::cout << "problems computing filter bank on the image!\n";
    return false;
  }

  return true;
}


//: this process extracts a "gauss" band from the input image and adds that as another layer to the filter_bank of the passed classifier
//  practically increases the dimension of the textons
//  the params of the classifier instance are used
bool sdet_add_to_filter_bank_process_cons(bprb_func_process& pro)
{
  std::vector<std::string> input_types;
  input_types.emplace_back("sdet_texture_classifier_sptr"); // classifier instance
  input_types.emplace_back("vcl_string");   // input image name
  input_types.emplace_back("unsigned");     // the plane to extract the filters from
  input_types.emplace_back("vcl_string");   // filter bank folder
  input_types.emplace_back("vcl_string");   // filter name : unique name to be used to write the response to filter folder
  input_types.emplace_back("bool");         // option to turn on gauss smoothing on image
  if (!pro.set_input_types(input_types))
    return false;

  std::vector<std::string> output_types;
  return pro.set_output_types(output_types);
}

bool sdet_add_to_filter_bank_process(bprb_func_process& pro)
{
  if (!pro.verify_inputs())
  {
    std::cout << pro.name() << "sdet_extract_filter_bank_process inputs are not valid"<< std::endl;
    return false;
  }
  // get inputs
  sdet_texture_classifier_sptr tc_ptr = pro.get_input<sdet_texture_classifier_sptr>(0);
  std::string name = pro.get_input<std::string>(1);
  auto n = pro.get_input<unsigned>(2);
  std::string folder = pro.get_input<std::string>(3);
  std::string res_name = pro.get_input<std::string>(4);
  bool is_smooth = pro.get_input<bool>(5);

  vil_image_view_base_sptr img_sptr = vil_load(name.c_str());

  vil_image_view<float> img_f;
  if (auto* img_ptr = dynamic_cast<vil_image_view<vxl_byte>*>(img_sptr.ptr())) {
    std::cout << " loaded image, ni: " << img_ptr->ni() << " " << img_ptr->nj() << " nplanes: " << img_ptr->nplanes()
             << " with pixel format: " << img_ptr->pixel_format() << std::endl;
    vil_image_view<vxl_byte> img_band = vil_plane(*img_ptr, n);
    vil_convert_stretch_range_limited(img_band, img_f, (vxl_byte)0, (vxl_byte)255, 0.0f, 1.0f);
  }else if (auto* img_ptr = dynamic_cast<vil_image_view<float>*>(img_sptr.ptr())) {
    std::cout << " loaded image, ni: " << img_ptr->ni() << " " << img_ptr->nj() << " nplanes: " << img_ptr->nplanes()
             << " with pixel format: " << img_ptr->pixel_format() << std::endl;
    img_f = vil_plane(*img_ptr, n);
  }else {
    std::cout << "Error: " << pro.name() << " -- The image pixel format: " << img_sptr->pixel_format() << " is not supported!\n";
    return false;
  }

  if (is_smooth)
    std::cout << " gauss smooth is applied on the input image" << std::endl;

  unsigned tni = tc_ptr->filter_responses().ni();
  unsigned tnj = tc_ptr->filter_responses().nj();
  if (tni != img_f.ni() || tnj != img_f.nj()) {
    std::cout << "filter responses have ni: " << tni << " nj: " << tnj << "..";
    std::cout << " input image has ni: " << img_f.ni() << " nj: " << img_f.nj() << "! resampling..\n";

    vil_image_view<float> out_img(tni, tnj);
    vil_resample_bilin(img_f, out_img, tni, tnj);
    tc_ptr->add_gauss_response(out_img, folder, name, res_name, is_smooth);
  } else
    tc_ptr->add_gauss_response(img_f, folder, name, res_name, is_smooth);

  return true;
}

//: this process extracts various bands from an input 4-band image and adds them as other layers to the filter_bank of the passed classifier
//  practically increases the dimension of the textons
//  the params of the classifier instance are used
bool sdet_add_to_filter_bank_process2_cons(bprb_func_process& pro)
{
  std::vector<std::string> input_types;
  input_types.emplace_back("sdet_texture_classifier_sptr"); // classifier instance
  input_types.emplace_back("vcl_string");   // input image name
  input_types.emplace_back("vcl_string");   // filter bank folder
  if (!pro.set_input_types(input_types))
    return false;

  std::vector<std::string> output_types;
  return pro.set_output_types(output_types);
}

void band_ops(vil_image_view<float>& band1, vil_image_view<float>& band2, vil_image_view<float>&ratio)
{
  vil_image_view<float> dif(band1.ni(), band1.nj());
  vil_image_view<float> sum(band1.ni(), band1.nj());

  vil_math_image_difference(band1, band2, dif);
  vil_math_image_sum(band1, band2, sum);
  vil_math_image_ratio(dif, sum, ratio);   // image values range in [-1,1]
  // scale to -0.03,0.03  -- heuristic scale
  vil_math_scale_and_offset_values(ratio, 0.03, 0.0); // map to [-0.03,0.03]
  // scale to 0,0.03  -- heuristic scale
  //vil_math_scale_and_offset_values(ratio, 1.0, 1.0); // map to [0,2]
  //vil_math_scale_and_offset_values(ratio, 0.03/2.0, 0.0); // map to [0,0.03]
}

bool sdet_add_to_filter_bank_process2(bprb_func_process& pro)
{
  if (!pro.verify_inputs())
  {
    std::cout << pro.name() << "sdet_extract_filter_bank_process inputs are not valid"<< std::endl;
    return false;
  }
  // get inputs
  sdet_texture_classifier_sptr tc_ptr = pro.get_input<sdet_texture_classifier_sptr>(0);
  std::string name = pro.get_input<std::string>(1);
  std::string folder = pro.get_input<std::string>(2);

  vil_image_view_base_sptr img_sptr = vil_load(name.c_str());
  if (img_sptr->nplanes() < 4) {
    std::cerr << pro.name() << " input image does not have at least 4 bands!\n";
    return false;
  }

  // assumes an input image in [0,1] range
  vil_image_view<float> img_f(img_sptr);
  std::cout << " loaded image, ni: " << img_sptr->ni() << " " << img_sptr->nj() << " nplanes: " << img_f.nplanes()
           << " with pixel format: " << img_sptr->pixel_format() << std::endl;

  unsigned tni = tc_ptr->filter_responses().ni();
  unsigned tnj = tc_ptr->filter_responses().nj();
  if (tni != img_f.ni() || tnj != img_f.nj()) {
    std::cout << "filter responses have ni: " << tni << " nj: " << tnj << "..";
    std::cout << " input image has ni: " << img_f.ni() << " nj: " << img_f.nj() << "! resampling..\n";

    vil_image_view<float> out_img(tni, tnj, img_f.nplanes());
    vil_resample_bilin(img_f, out_img, tni, tnj);

    // now compute various channels
    vil_image_view<float> band_b = vil_plane(out_img, 0);
    vil_image_view<float> band_g = vil_plane(out_img, 1);
    vil_image_view<float> band_r = vil_plane(out_img, 2);
    vil_image_view<float> band_nir = vil_plane(out_img, 3);

    vil_image_view<float> ratio(out_img.ni(), out_img.nj());

    band_ops(band_nir, band_r, ratio);
    tc_ptr->add_gauss_response(ratio, folder, name, "nir_r", false);  // last argument = false --> do not gaussian smooth this image

    band_ops(band_r, band_g, ratio);
    tc_ptr->add_gauss_response(ratio, folder, name, "r_g", false);  // last argument = false --> do not gaussian smooth this image

    band_ops(band_r, band_b, ratio);
    tc_ptr->add_gauss_response(ratio, folder, name, "r_b", false);  // last argument = false --> do not gaussian smooth this image

    band_ops(band_g, band_b, ratio);
    tc_ptr->add_gauss_response(ratio, folder, name, "g_b", false);  // last argument = false --> do not gaussian smooth this image

  } else {

    vil_image_view<float> band_b = vil_plane(img_f, 0);
    vil_image_view<float> band_g = vil_plane(img_f, 1);
    vil_image_view<float> band_r = vil_plane(img_f, 2);
    vil_image_view<float> band_nir = vil_plane(img_f, 3);

    vil_image_view<float> ratio(img_f.ni(), img_f.nj());

    band_ops(band_nir, band_r, ratio);
    tc_ptr->add_gauss_response(ratio, folder, name, "nir_r", false);  // last argument = false --> do not gaussian smooth this image

    band_ops(band_r, band_g, ratio);
    tc_ptr->add_gauss_response(ratio, folder, name, "r_g", false);  // last argument = false --> do not gaussian smooth this image

    band_ops(band_r, band_b, ratio);
    tc_ptr->add_gauss_response(ratio, folder, name, "r_b", false);  // last argument = false --> do not gaussian smooth this image

    band_ops(band_g, band_b, ratio);
    tc_ptr->add_gauss_response(ratio, folder, name, "g_b", false);  // last argument = false --> do not gaussian smooth this image

  }

  return true;
}

//: this process extracts a series of filter responses from the input image (assumed to be properly scaled to [0,1])
//  and adds each response as another layer to the other_responses_ of the passed classifier
//  practically increases the dimension of the textons
//  the params of the classifier instance are used
bool sdet_add_responses_to_filter_bank_process_cons(bprb_func_process& pro)
{
  std::vector<std::string> input_types;
  input_types.emplace_back("sdet_texture_classifier_sptr"); // classifier instance
  input_types.emplace_back("vcl_string");   // input image name
  input_types.emplace_back("vil_image_view_base_sptr");     // input image
  input_types.emplace_back("vcl_string");   // filter bank folder
  input_types.emplace_back("vcl_string");   // filter name : unique name to be used to write the response to filter folder  (the id of the filter bank will be appended to this name)
  if (!pro.set_input_types(input_types))
    return false;

  std::vector<std::string> output_types;
  return pro.set_output_types(output_types);
}

bool sdet_add_responses_to_filter_bank_process(bprb_func_process& pro)
{
  if (!pro.verify_inputs())
  {
    std::cout << pro.name() << "sdet_extract_filter_bank_process inputs are not valid"<< std::endl;
    return false;
  }
  // get inputs
  sdet_texture_classifier_sptr tc_ptr = pro.get_input<sdet_texture_classifier_sptr>(0);
  std::string name = pro.get_input<std::string>(1);
  vil_image_view_base_sptr img_sptr = pro.get_input<vil_image_view_base_sptr>(2);
  std::string folder = pro.get_input<std::string>(3);
  std::string res_name = pro.get_input<std::string>(4);

  if (img_sptr->pixel_format() != VIL_PIXEL_FORMAT_FLOAT) {
    std::cerr << " In sdet_add_responses_to_filter_bank_process(): the input image format is not FLOAT!.. it is assumed the input image is properly scaled to [0,1]!\n";
    return false;
  }

  vil_image_view<float> img_f(img_sptr);
  float min_value, max_value;
  vil_math_value_range(img_f, min_value, max_value);
  if (max_value > 1.0 || min_value < 0) {
    std::cerr << " In sdet_add_responses_to_filter_bank_process(): the input image is NOT scaled to [0,1]!\n";
    return false;
  }

  unsigned tni = tc_ptr->filter_responses().ni();
  unsigned tnj = tc_ptr->filter_responses().nj();
  if (tni != img_f.ni() || tnj != img_f.nj()) {
    std::cout << "filter responses have ni: " << tni << " nj: " << tnj << "..";
    std::cout << " input image has ni: " << img_f.ni() << " nj: " << img_f.nj() << "! resampling..\n";

    vil_image_view<float> out_img(tni, tnj);
    vil_resample_bilin(img_f, out_img, tni, tnj);
    tc_ptr->add_filter_responses(out_img, folder, name, res_name);
  } else
    tc_ptr->add_filter_responses(img_f, folder, name, res_name);

  return true;
}
