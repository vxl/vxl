#include "sdet_classify.h"

#include <sstream>
#include <stdexcept>
#include <tuple>

#include <sdet/sdet_atmospheric_image_classifier.h>
#include <vil/vil_blocked_image_resource.h>
#include <vil/vil_blocked_image_resource_sptr.h>
#include <vil/vil_convert.h>
#include <vil/vil_crop.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_view.h>

std::tuple<vil_image_view<float>, vil_image_view<vxl_byte>, vil_image_view<vil_rgb<vxl_byte> >, float>
sdet_classify(sdet_atmospheric_image_classifier tc,
              vil_image_view<float> const& image,
              std::string const& category,
              std::string const& cat_ids_file,
              float scale_factor)
{
  /*
   * Classify an image, using the given texton texture classifier.
   *
   * TODO: Use lib4c to log debug info in this function.
   *
   * Params
   * ------
   * tc - texture classifier.
   * image - the image to be classified.
   * category - The category of interest. The last returned float is the percentage of this category in the input image.
   * cat_ids_file - Optional file giving category names and colors.
   * scale_factor - Optional float to scale image values by. Defaults to 1/2048.
   *
   * Returns
   * -------
   * (input_valid, class_id, class_rgb, category_percent)
   *
   * input_valid - A cropped copy of the input image, showing the inner region which can be classified (no padding, so edges are lopped off).
   * class_id - The classification IDs over the input_valid image.
   * class_rgb - The RGB heatmap of classifications over the input_valid image.
   * category_percent - The percentage of pixels falling within the category of interest.
   */

  // Log classifier filter radius and number of textons
  /* std::cout << "max filter radius: " << tc.max_filter_radius() << std::endl; */
  /* unsigned ntextons = tc.get_number_of_textons(); */
  /* std::cout << " testing using the dictionary with the number of textons: " << ntextons << "\n categories:\n"; */

  // Get different classification categories
  std::vector<std::string> cats = tc.get_dictionary_categories();
  if (!cats.size()) {
    throw std::invalid_argument("The number of categories in the classifier dictionary is zero!\n");
  }

  // Log maximum gray level
  /* std::cout << "Scale Factor = " << scale_factor << std::endl; */

  // The number of edge pixels around the border of the input image
  // which cannot be classified (because filters would stretch past
  // into invalid pixels
  int edge_buffer = tc.max_filter_radius();

  // Mapping categories to ids
  std::map<std::string, unsigned char> cat_id_map;
  // Mapping those ids to colors
  std::map<unsigned char, vil_rgb<vxl_byte> > cat_color_map;

  // Assign ids to every category, optionally reading from an id file
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

      // Log category definition
      /* std::cout << "\t\t" << cat_name << " color: " << cat_color_map[(unsigned char)id] << '\n'; */

      ifs >> cat_name;
    }
  }

  // Check that the category string is one of the categories
  if (cat_id_map.find(category) == cat_id_map.end()) {
    std::ostringstream buffer;
    buffer << "Can not find the input category " << category << " among all categories!" << std::endl;
    throw std::invalid_argument(buffer.str());
  }


  // Log category IDs
  /* std::cout << " output id image will use the following ids for the categories:\n"; */

  // Print out id mappings, set categories in classifier in case we loaded them from disk
  std::vector<std::string> cats2;
  for (auto & iter : cat_id_map) {
    cats2.push_back(iter.first);
    /* std::cout << iter.first << " " << (int)iter.second << std::endl; */
  }
  tc.set_atmospheric_categories(cats2);

  // Log input image info
  /* std::cout << " image: ni=" << image.ni() << ", nj=" << image.nj() << ", nplanes=" << image.nplanes() */
  /*   << ", pixel_format=" << image.pixel_format() << std::endl; */

  // Convert image to grey (if necessary)
  vil_image_view<float> image_grey;
  if (image.nplanes() > 1) {
    vil_convert_planes_to_grey(image, image_grey);
  } else {
    image_grey = image;
  }

  // Scale image values
  vil_math_scale_values(image_grey, scale_factor);

  // Log scaled image info
  /* std::cout << " image_grey:  ni=" << image_grey.ni() << ", nj=" << image_grey.nj() << ", nplanes=" << image_grey.nplanes() */
  /*     << ", pixel_format=" << image_grey.pixel_format() << std::endl; */

  // Inner block of valid pixels which can be classified
  int ni_valid = image_grey.ni() - (2 * edge_buffer);
  int nj_valid = image_grey.nj() - (2 * edge_buffer);

  if (ni_valid <= 0 || nj_valid <= 0) {
    throw std::invalid_argument("Filter radius larger than image dimensions!\n");
  }

  // Crop the inner valid pixels which can be classified
  // i.e. this is the region of the input image which will be classified
  vil_image_view<float> outf = vil_crop(image_grey, edge_buffer, ni_valid, edge_buffer, nj_valid);

  // Classify the input image
  std::map<std::string, float> cat_percentage_map;
  vil_image_view<vxl_byte> class_img = tc.classify_image_blocks_qual2(image_grey, cat_id_map, cat_percentage_map);

  // Crop the inner valid pixels which were classified
  vil_image_view<vxl_byte> out_class_img = vil_crop(class_img, edge_buffer, ni_valid, edge_buffer, nj_valid);

  // Generate color map from id map
  vil_image_view<vil_rgb<vxl_byte> > out_rgb_img(out_class_img.ni(), out_class_img.nj());
  for (unsigned i = 0; i < out_class_img.ni(); i++)
    for (unsigned j = 0; j < out_class_img.nj(); j++)
      out_rgb_img(i,j) = cat_color_map[out_class_img(i, j)];

  // Return the region of the input which was classified, the classification IDs for that region,
  // the rgb heatmap of those classifications, and the percentage of pixels falling within the category of interest
  return std::make_tuple(outf, out_class_img, out_rgb_img, cat_percentage_map[category]);
};

