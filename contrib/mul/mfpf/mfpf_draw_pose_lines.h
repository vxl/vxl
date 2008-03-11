#ifndef mfpf_draw_pose_lines_h_
#define mfpf_draw_pose_lines_h_

//:
// \file
// \brief Function to draw lines on an image
// \author Tim Cootes

#include <vxl_config.h>  // for vxl_byte
#include <vimt/vimt_image_2d_of.h>
#include <mfpf/mfpf_pose.h>
#include <vcl_vector.h>

//: Draw an open polygon by jointing pose(ref_pts[i]) to pose(ref_pts[i+1])
void mfpf_draw_pose_lines(vimt_image_2d_of<vxl_byte>& image,
                          const mfpf_pose& pose,
                          const vcl_vector<vgl_point_2d<double> >& ref_pts,
                          vxl_byte value);

//: Draw an open polygon by jointing pose(ref_pts[i]) to pose(ref_pts[i+1])
void mfpf_draw_pose_lines(vimt_image_2d_of<vxl_byte>& image,
                             const mfpf_pose& pose,
                             const vcl_vector<vgl_point_2d<double> >& ref_pts,
                             vxl_byte r, vxl_byte g, vxl_byte b);

#endif // mfpf_draw_pose_lines_h_
