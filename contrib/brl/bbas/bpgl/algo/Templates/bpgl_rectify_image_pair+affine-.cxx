// Instantiaion of bpgl_rectify_image_pair
#include <bpgl/algo/bpgl_rectify_image_pair.hxx>
#include <vpgl/vpgl_affine_camera.h>
typedef vpgl_affine_camera<double> acam;
BPGL_RECTIFY_IMAGE_PAIR_INSTANTIATE(acam);
