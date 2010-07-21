#include <bvpgl/bvpgl_camera_estimator.txx>

class dummy {
 public:
  bool apply(vpgl_camera_double_sptr const&, vil_image_view<float>*) { return false; }
};

BVPGL_CAMERA_ESTIMATOR_INSTANTIATE(dummy);
