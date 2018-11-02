#ifndef bstm_cams_and_box_to_scene_h
#define bstm_cams_and_box_to_scene_h
//:
// \file
#include <iostream>
#include <cstdlib>
#include <bstm/bstm_scene.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_box_3d.h>
#include <vil/vil_image_view.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

typedef vpgl_perspective_camera<double> CamType;

//: Main bstm_cams_and_box_to_scene function
//  Takes in bundle.out file and image directory that created img_dir
void bstm_util_cams_and_box_to_scene (std::vector<CamType>& cams,
                                       vgl_box_3d<double>   bbox,
                                       bstm_scene&         uscene,
                                       unsigned time_steps = 32,
                                        int nblks = 5);

//does this block ijk have an observation?
bool bstm_util_has_observation(int i, int j, vgl_vector_3d<unsigned> pixPerBlock, vil_image_view<vxl_byte>& cntimg);

//does this block ijk have a high enough percent of counts
bool bstm_util_has_percent_views(int i,
                                  int j,
                                  double percent,
                                  vgl_vector_3d<unsigned> pixPerBlock,
                                  vil_image_view<vxl_byte>& cntimg,
                                  int num_views);

#endif // bstm_cams_and_box_to_scene_h
