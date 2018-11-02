#ifndef boxm2_cams_and_box_to_scene_h
#define boxm2_cams_and_box_to_scene_h
//:
// \file
#include <iostream>
#include <cstdlib>
#include <boxm2/boxm2_scene.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_box_3d.h>
#include <vil/vil_image_view.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

typedef vpgl_perspective_camera<double> CamType;

//: Main boxm2_cams_and_box_to_scene function
//  Takes in bundle.out file and image directory that created img_dir
void boxm2_util_cams_and_box_to_scene (std::vector<CamType>& cams,
                                       vgl_box_3d<double>   bbox,
                                       boxm2_scene&         uscene,
                                       int nblks = 8);

//does this block ijk have an observation?
bool boxm2_util_has_observation(int i, int j, vgl_vector_3d<unsigned> pixPerBlock, vil_image_view<vxl_byte>& cntimg);

//does this block ijk have a high enough percent of counts
bool boxm2_util_has_percent_views(int i,
                                  int j,
                                  double percent,
                                  vgl_vector_3d<unsigned> pixPerBlock,
                                  vil_image_view<vxl_byte>& cntimg,
                                  int num_views);

#endif // boxm2_cams_and_box_to_scene_h
