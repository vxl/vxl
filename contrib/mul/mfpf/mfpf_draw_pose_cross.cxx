#include "mfpf_draw_pose_cross.h"
//:
// \file
// \brief Function to draw a feature point on an image
// \author Tim Cootes

#include <mbl/mbl_draw_line.h>
#include <vgl/vgl_point_2d.h>

//: Draw oriented cross onto image, centred on pose.p()
//  Orientation defined by pose.u(), size by (ru,rv)
void mfpf_draw_pose_cross(vimt_image_2d_of<vxl_byte>& image,
                          const mfpf_pose& pose,
                          double ru, double rv,
                          vxl_byte value,unsigned width)
{
  const vimt_transform_2d& w2im = image.world2im();
  vgl_point_2d<double> p1=pose.p()-ru*pose.u(), p2=pose.p()+ru*pose.u();
  vgl_point_2d<double> p3=pose.p()-rv*pose.v(), p4=pose.p()+rv*pose.v();
  mbl_draw_line(image.image(),w2im(p1),w2im(p2),value,width);
  mbl_draw_line(image.image(),w2im(p3),w2im(p4),value,width);
}

//: Draw oriented cross onto 3-plane image, centred on pose.p()
//  Orientation defined by pose.u(), size by (ru,rv)
void mfpf_draw_pose_cross(vimt_image_2d_of<vxl_byte>& image,
                          const mfpf_pose& pose,
                          double ru, double rv,
                          vxl_byte r, vxl_byte g, vxl_byte b)
{
  const vimt_transform_2d& w2im = image.world2im();
  vgl_point_2d<double> p1=pose.p()-ru*pose.u(), p2=pose.p()+ru*pose.u();
  vgl_point_2d<double> p3=pose.p()-rv*pose.v(), p4=pose.p()+rv*pose.v();
  mbl_draw_line(image.image(),w2im(p1),w2im(p2),r,g,b);
  mbl_draw_line(image.image(),w2im(p3),w2im(p4),r,g,b);
}
