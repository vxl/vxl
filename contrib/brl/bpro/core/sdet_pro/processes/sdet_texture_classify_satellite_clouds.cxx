// This is brl/bpro/core/sdet_pro/processes/sdet_texture_classify_satellite_clouds.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#include <brdb/brdb_value.h>
#include <sdet/sdet_atmospheric_image_classifier.h>
#include <sdet/sdet_texture_classifier_params.h>
#include <vil/vil_image_view.h>
#include <vnl/vnl_random.h>
#include <vcl_algorithm.h>
#include <vil/vil_blocked_image_resource.h>
#include <vil/vil_blocked_image_resource_sptr.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_image_view.h>
#include <vil/file_formats/vil_nitf2_image.h>
#include <vil/vil_convert.h>
#include <vil/vil_crop.h>

// pass the handle to the nitf image resource as a vil_image_resource_sptr
// also pass the image location (i,j) and (width, height) as the ROI portion in the image to classify
// this process crops a larger image to account for the margin needed for classification and runs the classification
// then it only returns the ROI image as the output, so output image has size (width, height) 

//: initialize input and output types
bool sdet_texture_classify_satellite_clouds_process_cons(bprb_func_process& pro)
{
  // process takes 5 inputs:
  vcl_vector<vcl_string> input_types;
  input_types.push_back("sdet_texture_classifier_sptr"); //texton classifier
  input_types.push_back("vcl_string"); // path to dictionary
  input_types.push_back("vil_image_resource_sptr"); //input image resouce
  input_types.push_back("unsigned");   // i 
  input_types.push_back("unsigned");   // j (i,j) is the upper left pixel coordinate for ROI in the image resource
  input_types.push_back("unsigned");   // width
  input_types.push_back("unsigned");   // height (widht, height) is the size of the ROI in terms of pixels
  input_types.push_back("unsigned");   //texture block size
  input_types.push_back("vcl_string");  // a simple text file with the list of ids&colors for each category, if passed as "" just use 0, 1, 2, .. etc.
  if (!pro.set_input_types(input_types))
    return false;

  vcl_vector<vcl_string> output_types;
  output_types.push_back("vil_image_view_base_sptr");  // output cropped image - scaled to [0,1]
  output_types.push_back("vil_image_view_base_sptr");  // output id image  - a byte image
  return pro.set_output_types(output_types);
}

//: generates the edge map
bool sdet_texture_classify_satellite_clouds_process(bprb_func_process& pro)
{
  if (!pro.verify_inputs())
  {
    vcl_cout << pro.name() << "texture classifier process2 inputs are not valid"<< vcl_endl;
    return false;
  }
  // get inputs
  sdet_texture_classifier_sptr tcptr = pro.get_input<sdet_texture_classifier_sptr>(0);
  sdet_texture_classifier_params* tcp = static_cast<sdet_texture_classifier_params*>(tcptr.ptr());
  sdet_atmospheric_image_classifier tc(*tcp);
  vcl_string texton_dict_path = pro.get_input<vcl_string>(1);
  tc.load_dictionary(texton_dict_path);
  
  vcl_cout << "max filter radius in dictionary: " << tc.max_filter_radius() << vcl_endl;
  unsigned ntextons = tc.get_number_of_textons();
  vcl_cout << " testing using the dictionary with the number of textons: " << ntextons << "\n categories:\n";
  vcl_vector<vcl_string> cats = tc.get_dictionary_categories();
  
  //vcl_string img_name = pro.get_input<vcl_string>(2);
  vil_image_resource_sptr image = pro.get_input<vil_image_resource_sptr>(2);
  unsigned i = pro.get_input<unsigned>(3);
  unsigned j = pro.get_input<unsigned>(4);
  unsigned width = pro.get_input<unsigned>(5);
  unsigned height = pro.get_input<unsigned>(6);

  tc.block_size_ = pro.get_input<unsigned>(7); 
  vcl_string cat_ids_file = pro.get_input<vcl_string>(8);

  int invalid = tc.max_filter_radius();

  vcl_map<vcl_string, vil_rgb<vxl_byte> > cat_color_map;

  vcl_map<vcl_string, unsigned char> cat_id_map;
  
  if (cat_ids_file.compare("") == 0) {
    for (unsigned kk = 0; kk < cats.size(); kk++) 
      cat_id_map[cats[kk]] = kk;
  } else {
    vcl_ifstream ifs(cat_ids_file.c_str());
    vcl_string cat_name; int id; int r, g, b;
    ifs >> cat_name;
    while (!ifs.eof()) {
      ifs >> id; ifs >> r; ifs >> g; ifs >> b;
      cat_id_map[cat_name] = (unsigned char)id;
      cat_color_map[cat_name] = vil_rgb<vxl_byte>(r,g,b);
      vcl_cout << "\t\t" << cat_name << " color: " << cat_color_map[cat_name] << '\n';
      ifs >> cat_name;
    }
  }
    
  vcl_vector<vcl_string> cats2;
  vcl_cout << " output id image will use the following ids for the categories:\n";
  for (vcl_map<vcl_string, unsigned char>::iterator iter = cat_id_map.begin(); iter != cat_id_map.end(); iter++) {
    vcl_cout << iter->first << " " << (int)iter->second << vcl_endl;
    cats2.push_back(iter->first);
  }
  tc.set_atmospheric_categories(cats2);    


  //vil_image_resource_sptr image = vil_load_image_resource(img_name.c_str());
  if (!image)
  {
    vcl_cout << "problems with the input image resource handle!\n";
    return false;
  }
  vil_nitf2_image *nitf_image = static_cast<vil_nitf2_image*>(image.ptr());
  vcl_cout << " image size: ni: " << image->ni() << " nj: " << image->nj() << vcl_endl;
  
  vcl_vector< vil_nitf2_image_subheader* > headers = nitf_image->get_image_headers();
  vil_nitf2_image_subheader* hdr = headers[0];
  unsigned number_of_bits = hdr->get_number_of_bits_per_pixel();
  vcl_cout << "number of bits per pixel: " << number_of_bits << vcl_endl;
  if (number_of_bits != 11) {
    vcl_cerr << "This process only works with images having 11 bits!\n";
    return false;
  }

  vil_blocked_image_resource_sptr bir = blocked_image_resource(image);

  unsigned large_crop_size_w = width+2*invalid;
  unsigned large_crop_size_h = height+2*invalid;
  int ii = i-invalid; int jj = j-invalid;
  
  vil_image_view_base_sptr roi = bir->get_copy_view(ii, large_crop_size_w, jj, large_crop_size_h);
  if (!roi) {
    vcl_cerr << "cannot crop from image with size: " << image->ni() << " " << image->nj() << " at position (" << i << ", " << j << ") of size (" << width << ", " << height << ") with margin: " << invalid << vcl_endl;
    return false;
  }
  vil_image_view<vxl_uint_16> img(roi);
      
  vil_image_view<float> imgf(img.ni(), img.nj()); 
  vil_convert_cast(img, imgf);
  vil_math_scale_values(imgf, 1.0f/2048.0f);
      
  vil_image_view<float> outf = vil_crop(imgf, invalid, width, invalid, height);
  vil_image_view<vxl_byte> class_img = tc.classify_image_blocks_qual2(imgf, cat_id_map);
  vil_image_view<vxl_byte> out_class_img = vil_crop(class_img, invalid, width, invalid, height);
  
  vil_image_view_base_sptr img_ptr = new vil_image_view<float>(outf);
  pro.set_output_val<vil_image_view_base_sptr>(0, img_ptr);
  vil_image_view_base_sptr img_ptr2 = new vil_image_view<vxl_byte>(out_class_img);
  pro.set_output_val<vil_image_view_base_sptr>(1, img_ptr2);
  return true;
}
