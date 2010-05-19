#include <vil/vil_image_view.txx>
#include <boxm/sample/boxm_inf_line_sample.h>

typedef boxm_inf_line_sample<float> sample;
VIL_IMAGE_VIEW_INSTANTIATE(sample);
