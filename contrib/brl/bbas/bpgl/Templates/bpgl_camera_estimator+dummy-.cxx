#include <bpgl/bpgl_camera_estimator.hxx>

class dummy {
 public:
  bool apply(vpgl_camera_double_sptr const&, vil_image_view<float>*) { return false; }
};

BPGL_CAMERA_ESTIMATOR_INSTANTIATE(dummy);
