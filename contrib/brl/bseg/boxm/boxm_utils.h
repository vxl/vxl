#ifndef boxm_utils_h_
#define boxm_utils_h_

#include <vgl/vgl_box_3d.h>
#include <vpgl/vpgl_camera.h>

bool is_cube_visible(vgl_box_3d<double> const& bbox, 
                vpgl_camera_double_sptr const& camera, 
                unsigned int img_ni, unsigned int img_nj, 
                bool do_front_test = true);

#endif