// This is brl/bpro/core/sdet_pro/processes/sdet_texture_classify_satellite_clouds.cxx
#include <iostream>
#include <algorithm>
#include <tuple>
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#include <brdb/brdb_value.h>
#include <sdet/sdet_atmospheric_image_classifier.h>
#include <sdet/sdet_texture_classifier_params.h>
#include <sdet/algo/sdet_classify.h>
#include <vil/vil_image_view.h>
#include <vnl/vnl_random.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_blocked_image_resource.h>
#include <vil/vil_blocked_image_resource_sptr.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/file_formats/vil_nitf2_image.h>
#include <vil/vil_convert.h>
#include <vil/vil_crop.h>
#include <vil/vil_pixel_format.h>

// pass the handle to the nitf image resource as a vil_image_resource_sptr
// also pass the image location (i,j) and (width, height) as the ROI portion in the image to classify
// this process crops a larger image to account for the margin needed for classification and runs the classification
// then it only returns the ROI image as the output, so output image has size (width, height)

//: initialize input and output types
bool sdet_texture_classify_satellite_clouds_process_cons(bprb_func_process& pro)
{
  // process takes 11 inputs:
  std::vector<std::string> input_types;
  input_types.emplace_back("sdet_texture_classifier_sptr"); //texton classifier
  input_types.emplace_back("vil_image_view_base_sptr"); //input image
  input_types.emplace_back("vcl_string");  // a simple text file with the list of ids&colors for each category, if passed as "" just use 0, 1, 2, .. etc.
  input_types.emplace_back("vcl_string");  // the category whose percentage of pixels among all classified pixel will be returned
  input_types.emplace_back("float"); // scale_factor  (pixel_graylevel*scale_factor should be on the range [0,1])
  if (!pro.set_input_types(input_types))
    return false;

  std::vector<std::string> output_types;
  output_types.emplace_back("vil_image_view_base_sptr");  // output cropped image - scaled to [0,1]
  output_types.emplace_back("vil_image_view_base_sptr");  // output id image  - a byte image
  output_types.emplace_back("vil_image_view_base_sptr");  // output rgb image - a rgb image associated with class id image
  output_types.emplace_back("float");  // percentage of pixels among the classified pixels for the category that is listed "first" in the text file
  return pro.set_output_types(output_types);
}

//: generates the edge map
bool sdet_texture_classify_satellite_clouds_process(bprb_func_process& pro)
{
  if (!pro.verify_inputs())
  {
    std::cout << pro.name() << "texture classifier process inputs are not valid"<< std::endl;
    return false;
  }

  // get inputs
  sdet_texture_classifier_sptr tc_sptr = pro.get_input<sdet_texture_classifier_sptr>(0);
  vil_image_view_base_sptr image_sptr = pro.get_input<vil_image_view_base_sptr>(1);
  std::string first_category = pro.get_input<std::string>(2);
  std::string cat_ids_file = pro.get_input<std::string>(3);
  float scale_factor  = pro.get_input<float>(4);

  // Cast texture classifier to atmospheric image classifier
  auto* tc_ptr = dynamic_cast<sdet_atmospheric_image_classifier*> (tc_sptr.as_pointer());
  if (!tc_ptr) {
    std::cerr << pro.name() << " :-- input texture classifier is not sdet_atmospheric_image_classifier" << std::endl;
    return false;
  }

  // Cast image view base to image view <float>
  auto* image_ptr = dynamic_cast<vil_image_view<float>*> (image_sptr.as_pointer());
  if (!image_ptr) {
    std::cerr << pro.name() << " :-- input image is not vil_image_view<float>" << std::endl;
    return false;
  } else if (image_ptr->nplanes() != 1) {
    std::cerr << pro.name() << " :-- input image is not single band" << std::endl;
    return false;
  }

  try {
    std::tuple<vil_image_view<float>, vil_image_view<vxl_byte>, vil_image_view<vil_rgb<vxl_byte> >, float>
      ret = sdet_classify(*tc_ptr, *image_ptr, first_category, cat_ids_file, scale_factor);

    vil_image_view_base_sptr img_ptr = new vil_image_view<float>(std::get<0>(ret));
    pro.set_output_val<vil_image_view_base_sptr>(0, img_ptr);
    vil_image_view_base_sptr img_ptr2 = new vil_image_view<vxl_byte>(std::get<1>(ret));
    pro.set_output_val<vil_image_view_base_sptr>(1, img_ptr2);
    vil_image_view_base_sptr img_ptr3 = new vil_image_view<vil_rgb<vxl_byte> >(std::get<2>(ret));
    pro.set_output_val<vil_image_view_base_sptr>(2, img_ptr3);
    pro.set_output_val<float>(3, std::get<3>(ret));  // returns the percentage of the "first" category

    return true;
  }
  catch (std::invalid_argument& e) {
    std::cout << e.what();
    return false;
  }

}

//: perform the cloud classification for the given image
//  Note that there will be a margin around the image
//: initialize input and output types
bool sdet_texture_classify_satellite_clouds_process2_cons(bprb_func_process& pro)
{
  // process takes 5 inputs
  std::vector<std::string> input_types;
  input_types.emplace_back("sdet_texture_classifier_sptr"); //texton classifier
  input_types.emplace_back("vcl_string"); // path to dictionary
  input_types.emplace_back("vil_image_resource_sptr"); //input image resouce
  input_types.emplace_back("unsigned");   // i
  input_types.emplace_back("unsigned");   // j (i,j) is the upper left pixel coordinate for ROI in the image resource
  input_types.emplace_back("unsigned");   // width
  input_types.emplace_back("unsigned");   // height (width, height) is the size of the ROI in terms of pixels
  input_types.emplace_back("unsigned");   //texture block size
  input_types.emplace_back("vcl_string");  // a simple text file with the list of ids&colors for each category, if passed as "" just use 0, 1, 2, .. etc.
  input_types.emplace_back("vcl_string");  // the category whose percentage of pixels among all classified pixel will be returned
  if (!pro.set_input_types(input_types))
    return false;

  std::vector<std::string> output_types;
  output_types.emplace_back("vil_image_view_base_sptr");  // output cropped image - scaled to [0,1]
  output_types.emplace_back("vil_image_view_base_sptr");  // output id image  - a byte image
  output_types.emplace_back("vil_image_view_base_sptr");  // output rgb image - a rgb image associated with class id image
  output_types.emplace_back("float");  // percentage of pixels among the classified pixels for the category that is listed "first" in the text file
  return pro.set_output_types(output_types);
}

//: execute the classification
bool sdet_texture_classify_satellite_clouds_process2(bprb_func_process& pro)
{
  if (!pro.verify_inputs())
  {
    std::cout << pro.name() << "texture classifier process2 inputs are not valid"<< std::endl;
    return false;
  }
  // get inputs
  sdet_texture_classifier_sptr tcptr = pro.get_input<sdet_texture_classifier_sptr>(0);
  auto* tcp = static_cast<sdet_texture_classifier_params*>(tcptr.ptr());
  sdet_atmospheric_image_classifier tc(*tcp);
  std::string texton_dict_path = pro.get_input<std::string>(1);
  tc.load_dictionary(texton_dict_path);

  std::cout << "max filter radius in dictionary: " << tc.max_filter_radius() << std::endl;
  unsigned ntextons = tc.get_number_of_textons();
  std::cout << " testing using the dictionary with the number of textons: " << ntextons << "\n categories:\n";
  std::vector<std::string> cats = tc.get_dictionary_categories();
  if (!cats.size()) {
    std::cerr << "The number of categories is zero!! in the classifier dictionary!\n";
    return false;
  }

  vil_image_resource_sptr image = pro.get_input<vil_image_resource_sptr>(2);
  auto i = pro.get_input<unsigned>(3);
  auto j = pro.get_input<unsigned>(4);
  auto width = pro.get_input<unsigned>(5);
  auto height = pro.get_input<unsigned>(6);
  tc.block_size_ = pro.get_input<unsigned>(7);
  std::string cat_ids_file = pro.get_input<std::string>(8);
  std::string first_category = pro.get_input<std::string>(9);

  if (!image)
  {
    std::cerr << pro.name() << ": problems with the input image resource handle!\n";
    return false;
  }

  int invalid_margin = tc.max_filter_radius();

  std::map<unsigned char, vil_rgb<vxl_byte> > cat_color_map;
  std::map<std::string, unsigned char> cat_id_map;

  if (cat_ids_file.compare("") == 0) {
    for (unsigned kk = 0; kk < cats.size(); kk++)
      cat_id_map[cats[kk]] = kk;
  }
  else {
    std::ifstream ifs(cat_ids_file.c_str());
    std::string cat_name; int id, r, g, b;
    ifs >> cat_name;
    while (!ifs.eof()) {
      ifs >> id;  ifs >> r;  ifs >> g;  ifs >> b;
      cat_id_map[cat_name] = (unsigned char)id;
      cat_color_map[(unsigned char)id] = vil_rgb<vxl_byte>(r,g,b);
      std::cout << "\t\t" << cat_name << " color: " << cat_color_map[(unsigned char)id] << '\n';
      ifs >> cat_name;
    }
  }

  // check input of first category
  if (cat_id_map.find(first_category) == cat_id_map.end()) {
    std::cerr << pro.name() << ": can not find the input first category " << first_category << " among all categories!\n";
    return false;
  }

  std::vector<std::string> cats2;
  std::cout << " output id image will use the following ids for the categories:\n";
  for (auto & mit : cat_id_map) {
    std::cout << mit.first << " " << (int)mit.second << std::endl;
    cats2.push_back(mit.first);
  }
  tc.set_atmospheric_categories(cats2);

  auto *nitf_image = static_cast<vil_nitf2_image*>(image.ptr());
  std::cout << " image size: ni: " << image->ni() << ", nj: " << image->nj() << std::endl;

  std::vector<vil_nitf2_image_subheader* > headers = nitf_image->get_image_headers();
  vil_nitf2_image_subheader* hdr = headers[0];
  unsigned number_of_bits = hdr->get_number_of_bits_per_pixel();
  std::cout << "number of bits per pixels: " << number_of_bits << std::endl;
  if (number_of_bits != 11) {
    std::cerr << pro.name() << ": process only works with 11 bits image!\n";
    return false;
  }
  vil_blocked_image_resource_sptr bir = blocked_image_resource(image);

  // crop the image
  if ( (i+width) >= image->ni() || (j+height) >= image->nj() ) {
    std::cerr << pro.name() << ": can not crop from image with size " << image->ni() << " " << image->nj()
                           << " at position (" << i << ", " << j << ") of size (" << width << ", " << height << ") with margin " << invalid_margin << std::endl;
    return false;
  }
  vil_image_view_base_sptr roi = bir->get_copy_view(i, width, j, height);
  if (!roi) {
    std::cerr << pro.name() << ": can not crop from image with size " << image->ni() << " " << image->nj()
             << " at position (" << i << ", " << j << ") of size (" << width << ", " << height << ") with margin " << invalid_margin << std::endl;
    return false;
  }

  vil_image_view<vxl_uint_16> img(roi);
  vil_image_view<float> imgf(img.ni(), img.nj());
  vil_convert_cast(img, imgf);
  vil_math_scale_values(imgf, 1.0f/2048.0f);
  vil_image_view<float> outf = imgf;
  std::map<std::string, float> cat_percentage_map;
  vil_image_view<vxl_byte> class_img = tc.classify_image_blocks_qual2(imgf, cat_id_map, cat_percentage_map);
  // obtain the output image with original size
  vil_image_view<vxl_byte> out_class_img = class_img;

  // transfer id map to color map
  vil_image_view<vil_rgb<vxl_byte> > out_rgb_img(out_class_img.ni(), out_class_img.nj());
  for (unsigned i = 0; i < out_class_img.ni(); i++)
    for (unsigned j = 0; j < out_class_img.nj(); j++)
      out_rgb_img(i,j) = cat_color_map[out_class_img(i,j)];

  // generate output
  unsigned out_i = 0;

  vil_image_view_base_sptr img_ptr  = new vil_image_view<float>(outf);
  pro.set_output_val<vil_image_view_base_sptr>(out_i++, img_ptr);
  vil_image_view_base_sptr img_ptr2 = new vil_image_view<vxl_byte>(out_class_img);
  pro.set_output_val<vil_image_view_base_sptr>(out_i++, img_ptr2);
  vil_image_view_base_sptr img_ptr3 = new vil_image_view<vil_rgb<vxl_byte> >(out_rgb_img);
  pro.set_output_val<vil_image_view_base_sptr>(out_i++, img_ptr3);
  pro.set_output_val<float>(out_i++, cat_percentage_map[first_category]);  // returns the percentage of the 'first' category

  return true;
}
