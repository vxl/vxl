#include "sdet_classify_clouds.h"

#include <sstream>
#include <stdexcept>
#include <tuple>

#include <sdet/sdet_atmospheric_image_classifier.h>
#include <sdet/sdet_texture_classifier.h>
#include <vil/vil_blocked_image_resource.h>
#include <vil/vil_blocked_image_resource_sptr.h>
#include <vil/vil_convert.h>
#include <vil/vil_crop.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_view.h>

std::tuple<vil_image_view<float>, vil_image_view<vxl_byte>, vil_image_view<vil_rgb<vxl_byte> >, float>
sdet_classify_clouds(sdet_texture_classifier const& cloud_classifier,
                     std::string const& texton_dict_path,
                     vil_image_resource_sptr image,
                     unsigned int i,
                     unsigned int j,
                     unsigned int ni,
                     unsigned int nj,
                     unsigned int block_size,
                     std::string const& first_category,
                     std::string const& cat_ids_file,
                     float scale_factor)
{
  sdet_atmospheric_image_classifier tc(cloud_classifier);
  tc.load_dictionary(texton_dict_path);

  std::cout << "max filter radius in dictionary: " << tc.max_filter_radius() << std::endl;
  unsigned ntextons = tc.get_number_of_textons();
  std::cout << " testing using the dictionary with the number of textons: " << ntextons << "\n categories:\n";
  std::vector<std::string> cats = tc.get_dictionary_categories();
  if (!cats.size()) {
    throw std::invalid_argument("The number of categories in the classifier dictionary is zero!\n");
  }

  tc.block_size_ = block_size;

  // input maximum graylevel
  std::cout << "Scale Factor = " << scale_factor << std::endl;

  int invalid = tc.max_filter_radius();

  std::map<unsigned char, vil_rgb<vxl_byte> > cat_color_map;
  std::map<std::string, unsigned char> cat_id_map;

  if (cat_ids_file.compare("") == 0) {
    for (unsigned kk = 0; kk < cats.size(); kk++)
      cat_id_map[cats[kk]] = kk;
  } else {
    std::ifstream ifs(cat_ids_file.c_str());
    std::string cat_name; int id; int r, g, b;
    ifs >> cat_name;
    while (!ifs.eof()) {
      ifs >> id; ifs >> r; ifs >> g; ifs >> b;
      cat_id_map[cat_name] = (unsigned char)id;
      cat_color_map[(unsigned char)id] = vil_rgb<vxl_byte>(r,g,b);
      std::cout << "\t\t" << cat_name << " color: " << cat_color_map[(unsigned char)id] << '\n';
      ifs >> cat_name;
    }
  }

  // check input of first_category
  if (cat_id_map.find(first_category) == cat_id_map.end()) {
    std::ostringstream buffer;
    buffer << "Can not find the input first category " << first_category << " among all categories!" << std::endl;
    throw std::invalid_argument(buffer.str());
  }

  std::vector<std::string> cats2;
  std::cout << " output id image will use the following ids for the categories:\n";
  for (auto & iter : cat_id_map) {
    std::cout << iter.first << " " << (int)iter.second << std::endl;
    cats2.push_back(iter.first);
  }
  tc.set_atmospheric_categories(cats2);

  // report resource info
  std::cout << " image: ni=" << image->ni() << ", nj=" << image->nj() << ", nplanes=" << image->nplanes()
    << " file_format=" << image->file_format() << ", pixel_format=" << image->pixel_format() << std::endl;

  // crop info
  unsigned nii = ni + 2*invalid;
  unsigned njj = nj + 2*invalid;
  int ii = i-invalid; int jj = j-invalid;

  // crop via get_copy_view (try blocked_image_resource for speed)
  vil_image_view_base_sptr roi;
  vil_blocked_image_resource_sptr bir = blocked_image_resource(image);
  if (!bir) {
    roi = image->get_copy_view(ii, nii, jj, njj);
  } else {
    roi = bir->get_copy_view(ii, nii, jj, njj);
  }

  // check for valid roi
  if (!roi) {
    std::ostringstream buffer;
    buffer << "Could not crop from image with size (" << image->ni() << "," << image->nj()
      << ") at position (" << i << "," << j << ") of size (" << ni << ", " << nj << ") with margin: " << invalid << std::endl;
    throw std::invalid_argument(buffer.str());
  }

  // report roi info
  std::cout << " roi: ni=" << roi->ni() << ", nj=" << roi->nj() << ", nplanes=" << roi->nplanes()
      << ", pixel_format=" << roi->pixel_format() << std::endl;

  // cast to float
  vil_image_view<float> roi_float = *vil_convert_cast(float(), roi);

  // convert to grey (if necessary)
  vil_image_view<float> roi_float_grey;
  if (roi->nplanes() > 1) {
    vil_convert_planes_to_grey(roi_float, roi_float_grey);
  } else {
    roi_float_grey = roi_float;
  }

  // scale
  vil_math_scale_values(roi_float_grey, scale_factor);

  // report roi float info
  std::cout << " roi_float_grey:  ni=" << roi_float_grey.ni() << ", nj=" << roi_float_grey.nj() << ", nplanes=" << roi_float_grey.nplanes()
      << ", pixel_format=" << roi_float_grey.pixel_format() << std::endl;

  vil_image_view<float> outf = vil_crop(roi_float_grey, invalid, ni, invalid, nj);
  std::map<std::string, float> cat_percentage_map;
  vil_image_view<vxl_byte> class_img = tc.classify_image_blocks_qual2(roi_float_grey, cat_id_map, cat_percentage_map);
  vil_image_view<vxl_byte> out_class_img = vil_crop(class_img, invalid, ni, invalid, nj);

  // transfer id map to color map
  vil_image_view<vil_rgb<vxl_byte> > out_rgb_img(out_class_img.ni(), out_class_img.nj());
  for (unsigned i = 0; i < out_class_img.ni(); i++)
    for (unsigned j = 0; j < out_class_img.nj(); j++)
      out_rgb_img(i,j) = cat_color_map[out_class_img(i, j)];

  // return the cropped img, the classification greyscale id map, the classification rgb id map, and the percentage of the "first" category
  return std::make_tuple(outf, out_class_img, out_rgb_img, cat_percentage_map[first_category]);

};

