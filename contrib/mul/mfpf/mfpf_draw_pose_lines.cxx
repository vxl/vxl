//:
// \file
// \brief Function to draw a feature point on an image
// \author Tim Cootes

#include <mfpf/mfpf_draw_pose_lines.h>
#include <mbl/mbl_draw_line.h>

//: Draw an open polygon by jointing pose(ref_pts[i]) to pose(ref_pts[i+1])
void mfpf_draw_pose_lines(vimt_image_2d_of<vxl_byte>& image,
                          const mfpf_pose& pose,
                          const vcl_vector<vgl_point_2d<double> >& ref_pts,
                          vxl_byte value)
{
  const vimt_transform_2d& w2im = image.world2im();
  vgl_point_2d<double> p = w2im(pose(ref_pts[0]));
  for (unsigned i=1;i<ref_pts.size();++i)
  {
    vgl_point_2d<double> q = w2im(pose(ref_pts[i]));
    mbl_draw_line(image.image(),p,q,value);
    p=q;
  }
}

//: Draw an open polygon by jointing pose(ref_pts[i]) to pose(ref_pts[i+1])
void mfpf_draw_pose_lines(vimt_image_2d_of<vxl_byte>& image,
                          const mfpf_pose& pose,
                          const vcl_vector<vgl_point_2d<double> >& ref_pts,
                          vxl_byte r, vxl_byte g, vxl_byte b)
{
  const vimt_transform_2d& w2im = image.world2im();
  vgl_point_2d<double> p = w2im(pose(ref_pts[0]));
  for (unsigned i=1;i<ref_pts.size();++i)
  {
    vgl_point_2d<double> q = w2im(pose(ref_pts[i]));
    mbl_draw_line(image.image(),p,q,r);
    mbl_draw_line(image.image(),p,q,g);
    mbl_draw_line(image.image(),p,q,b);
    p=q;
  }
}

