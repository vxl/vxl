#include <vil/vil_image_view.txx>
#include <boxm/boxm_line_samples.h>

typedef boxm_line_samples<float> sample;
VIL_IMAGE_VIEW_INSTANTIATE(sample);
