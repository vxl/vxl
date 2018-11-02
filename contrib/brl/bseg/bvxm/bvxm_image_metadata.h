#ifndef bvxm_image_metadata_h_
#define bvxm_image_metadata_h_
//:
// \file
// \brief
// \author Thomas Pollard
// \date January 12, 2008

#include <iostream>
#include <string>
#include <utility>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vil/vil_image_view_base.h>
#include <vnl/vnl_vector.h>
#include <vpgl/vpgl_camera_double_sptr.h>

class bvxm_image_metadata
{
 public:

  bvxm_image_metadata()
  : img(nullptr),camera(nullptr),light(),timestamp(-1), name("NONE") {}

  bvxm_image_metadata(vil_image_view_base_sptr img_arg, vpgl_camera_double_sptr camera_arg,
                      vnl_vector<double> light_arg = vnl_vector<double>(), double timestamp_arg = -1,
                      std::string name_arg = "NONE")
  : img(img_arg), camera(camera_arg), light(light_arg), timestamp(timestamp_arg),
    name(std::move(name_arg)) {}

  vil_image_view_base_sptr img;
  vpgl_camera_double_sptr camera;
  vnl_vector<double> light;
  double timestamp;
  std::string name;
};

#endif // bvxm_image_metadata_h_
