#ifndef sdet_algo_classify_clouds_h_
#define sdet_algo_classify_clouds_h_

#include <iostream>
#include <iosfwd>
#include <vector>
#include <string>

#include <sdet/sdet_texture_classifier.h>
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
                     std::string const& cat_ids_file = "",
                     float scale_factor = 1.0 / 2048.0 );

#endif // sdet_algo_classify_clouds_h_
