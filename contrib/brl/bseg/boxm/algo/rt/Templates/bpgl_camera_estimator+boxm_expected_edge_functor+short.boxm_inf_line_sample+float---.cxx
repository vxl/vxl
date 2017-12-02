#include <bpgl/bpgl_camera_estimator.hxx>
#include <boxm/algo/rt/boxm_expected_edge_functor.h>
#include <boxm/sample/boxm_inf_line_sample.h>

typedef boxm_inf_line_sample<float> sample;
typedef boxm_expected_edge_functor<short,sample > functor;
BPGL_CAMERA_ESTIMATOR_INSTANTIATE(functor);
