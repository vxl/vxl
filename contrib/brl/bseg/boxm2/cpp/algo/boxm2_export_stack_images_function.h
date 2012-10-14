#ifndef boxm2_export_stack_images_function_h
#define boxm2_export_stack_images_function_h
//:
// \file

#include <vcl_sstream.h>
#include <vcl_iomanip.h>
#include <vil3d/vil3d_image_view.h>
#include <boxm2/io/boxm2_cache.h>
class boxm2_export_stack_images_function
{
 public:
  static void export_opacity_stack_images(const boxm2_scene_sptr& scene, boxm2_cache_sptr & cache, vcl_string outdir);
  static void export_greyscale_stack_images(const boxm2_scene_sptr& scene, boxm2_cache_sptr & cache, vil3d_image_view<unsigned char> & img3d);
  static void export_color_stack_images(const boxm2_scene_sptr& scene, boxm2_cache_sptr & cache, vil3d_image_view<unsigned char> & img3d);
};



#endif
