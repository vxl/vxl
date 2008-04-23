#ifndef mfpf_draw_pose_cross_h_
#define mfpf_draw_pose_cross_h_

//:
// \file
// \brief Function to draw a cross on an image
// \author Tim Cootes

#include <vxl_config.h>  // for vxl_byte
#include <vimt/vimt_image_2d_of.h>
#include <mfpf/mfpf_pose.h>

//: Draw oriented cross onto image, centred on pose.p()
//  Orientation defined by pose.u(), size by (ru,rv)
void mfpf_draw_pose_cross(vimt_image_2d_of<vxl_byte>& image,
                             const mfpf_pose& pose,
                             double ru, double rv,
                             vxl_byte value,unsigned width=1);

//: Draw oriented cross onto 3-plane image, centred on pose.p()
//  Orientation defined by pose.u(), size by (ru,rv)
void mfpf_draw_pose_cross(vimt_image_2d_of<vxl_byte>& image,
                             const mfpf_pose& pose,
                             double ru, double rv,
                             vxl_byte r, vxl_byte g, vxl_byte b);

#endif // mfpf_draw_pose_cross_h_
