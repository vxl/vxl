#include <iostream>
#include <algorithm>
#include "mfpf_draw_matches.h"
//:
// \file
// \brief Function to draw a feature point on an image
// \author Tim Cootes

#include <mfpf/mfpf_point_finder.h>
#include <mfpf/mfpf_draw_pose_lines.h>
#include <mfpf/mfpf_pose_set.h>
#include <vgl/vgl_point_2d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vimt/vimt_convert.h>

//: Draws first n_draw matches (defined by poses) into out_image
//  Shape to draw defined by pf.get_outline(pts)
void mfpf_draw_matches(const mfpf_point_finder& pf,
                       const vimt_image_2d_of<float>& image,
                       const mfpf_pose_set& matches,
                       unsigned n_draw,
                       vimt_image_2d_of<vxl_byte>& out_image)
{
  vimt_convert_cast(image,out_image);

  unsigned nd = std::min(n_draw,unsigned(matches.poses.size()));
  double f = 250.0/nd;
  std::vector<vgl_point_2d<double> > m_pts;
  pf.get_outline(m_pts);
  mfpf_pose scale(0,0,pf.step_size(),0);
  for (unsigned j=0;j<nd;++j)
  {
    mfpf_draw_pose_lines(out_image,matches.poses[j]*scale,
                         m_pts,vxl_byte(255-j*f));
  }
}
