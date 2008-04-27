#ifndef mfpf_draw_matches_h_
#define mfpf_draw_matches_h_
//:
// \file
// \brief Function to draw a representation of each match onto an image
// \author Tim Cootes

#include <vxl_config.h>  // for vxl_byte
#include <vimt/vimt_image_2d_of.h>

struct mfpf_pose_set;
class mfpf_point_finder;

//: Draws first n_draw matches (defined by poses) into out_image
//  Shape to draw defined by pf.get_outline(pts).
//  Matches assumed to be sorted by fit quality.  Best is drawn
//  in white (255), subsequent matches use increasingly dark
//  shade of grey.
void mfpf_draw_matches(const mfpf_point_finder& pf,
                       const vimt_image_2d_of<float>& image,
                       const mfpf_pose_set& matches,
                       unsigned m_draw,
                       vimt_image_2d_of<vxl_byte>& out_image);

#endif // mfpf_draw_matches_h_
