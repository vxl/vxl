#ifndef sdet_algo_classify_h_
#define sdet_algo_classify_h_

#include <iostream>
#include <iosfwd>
#include <vector>
#include <string>

#include <sdet/sdet_atmospheric_image_classifier.h>
#include <vil/vil_image_view.h>

std::tuple<vil_image_view<float>, vil_image_view<vxl_byte>, vil_image_view<vil_rgb<vxl_byte> >, float>
sdet_classify(sdet_atmospheric_image_classifier tc,
              vil_image_view<float> const& image,
              std::string const& category,
              std::string const& cat_ids_file = "",
              float scale_factor = 1.0 / 2048.0);

#endif // sdet_algo_classify_h_
