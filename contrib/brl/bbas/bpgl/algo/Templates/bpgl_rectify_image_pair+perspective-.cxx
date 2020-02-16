// Instantiaion of bpgl_rectify_image_pair
#include <bpgl/algo/bpgl_rectify_image_pair.hxx>
#include <vpgl/vpgl_perspective_camera.h>
typedef vpgl_perspective_camera<double> pcam;
BPGL_RECTIFY_IMAGE_PAIR_INSTANTIATE(pcam);

