#ifndef bstm_util_h
#define bstm_util_h
//:
// \file
#include <iostream>
#include <bstm/bstm_scene.h>
#include <bstm/io/bstm_cache.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_point_3d.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>

//: Utility class with static methods
class bstm_util {
public:
  static vnl_vector_fixed<float, 3>
  expected_color(vnl_vector_fixed<unsigned char, 8> mog);

  static bool query_point(bstm_scene_sptr &scene,
                          bstm_cache_sptr &cache,
                          const vgl_point_3d<double> &point,
                          unsigned time,
                          float &prob,
                          float &intensity);

  static bool query_point_color(bstm_scene_sptr &scene,
                                bstm_cache_sptr &cache,
                                const vgl_point_3d<double> &point,
                                unsigned time,
                                float &prob,
                                vnl_vector_fixed<unsigned char, 3> &rgb);

  // verifies that a scene has a valid appearance, spits out data type and
  // appearance type size
  static bool verify_appearance(const std::vector<std::string> &apps,
                                const std::vector<std::string> &valid_types,
                                std::string &data_type,
                                int &appTypeSize);
  static bool verify_appearance(const bstm_scene &scene,
                                const std::vector<std::string> &valid_types,
                                std::string &data_type,
                                int &appTypeSize);

  static vil_image_view_base_sptr
  prepare_input_image(const vil_image_view_base_sptr& loaded_image,
                      bool force_grey = true);
  static vil_image_view_base_sptr prepare_input_image(std::string filename,
                                                      bool force_grey = true) {
    vil_image_view_base_sptr loaded_image = vil_load(filename.c_str());
    return bstm_util::prepare_input_image(loaded_image, force_grey);
  }
};

#endif // bstm_util_h
