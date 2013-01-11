#ifndef bstm_util_h
#define bstm_util_h
//:
// \file
#include <vcl_iostream.h>
#include <vgl/vgl_point_3d.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <bstm/bstm_scene.h>
#include <bstm/io/bstm_cache.h>

//: Utility class with static methods
class bstm_util
{
  public:

    static vnl_vector_fixed<float,3>  expected_color( vnl_vector_fixed<unsigned char, 8>  mog);

    static bool query_point(bstm_scene_sptr& scene, bstm_cache_sptr& cache, const vgl_point_3d<double>& point, unsigned time, float& prob, float& intensity);

    static bool query_point_color(bstm_scene_sptr& scene, bstm_cache_sptr& cache,
                                  const vgl_point_3d<double>& point, unsigned time,
                                  float& prob, vnl_vector_fixed< unsigned char, 3>& rgb);

    //verifies that a scene has a valid appearance, spits out data type and appearance type size
    static bool verify_appearance(bstm_scene& scene, const vcl_vector<vcl_string>&valid_types, vcl_string& data_type, int& appTypeSize );

    static vil_image_view_base_sptr prepare_input_image(vil_image_view_base_sptr loaded_image, bool force_grey=true);
    static vil_image_view_base_sptr prepare_input_image(vcl_string filename, bool force_grey=true) {
      vil_image_view_base_sptr loaded_image = vil_load(filename.c_str());
      return bstm_util::prepare_input_image(loaded_image,force_grey);
    }
};

#endif // bstm_util_h
