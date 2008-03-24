#ifndef bvxm_image_metadata_h_
#define bvxm_image_metadata_h_
//:
// \file
// \brief
// \author Thomas Pollard
// \date 1/12/08

#include <vcl_string.h>

#include <vil/vil_image_view_base.h>
#include <vnl/vnl_vector.h>
#include <vpgl/vpgl_camera.h>

class bvxm_image_metadata
{
 public:

  bvxm_image_metadata()
  : img(0),camera(0),light(),timestamp(-1), name("NONE") {}

  bvxm_image_metadata(vil_image_view_base_sptr img_arg, vpgl_camera_double_sptr camera_arg,
                      vnl_vector<double> light_arg = vnl_vector<double>(), double timestamp_arg = -1,
                      vcl_string name_arg = "NONE")
  : img(img_arg), camera(camera_arg), light(light_arg), timestamp(timestamp_arg),
    name(name_arg) {}

  vil_image_view_base_sptr img;
  vpgl_camera_double_sptr camera;
  vnl_vector<double> light;
  double timestamp;
  vcl_string name;
};

#endif // bvxm_image_metadata_h_
