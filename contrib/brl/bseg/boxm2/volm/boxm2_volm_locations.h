//This is brl/bseg/boxm2/volm/boxm2_volm_locations.h
#ifndef boxm2_volm_locations_h_
#define boxm2_volm_locations_h_
//:
// \file
// \brief  A class to represent location hypotheses for volumetric matching
//
// \author
// \date October 11, 2012
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//

#include <vbl/vbl_ref_count.h>
#include <vpgl/vpgl_lvcs_sptr.h>
#include <vpgl/file_formats/vpgl_geo_camera.h>
#include <volm/volm_tile.h>
#include <vil/vil_image_view.h>
#include <vgl/vgl_point_3d.h>

//: a set of 3d points in local coordinates of a scene, corresponds to an output tile.
//  this class knows how to construct an output tile to write probabilities
//  there can be multiple elevantions for a given lat, lon - just add to the 1d vector, best one is written to the output image
class boxm2_volm_loc_hypotheses : public vbl_ref_count
{
 public:
  //: construct the locs_ and lat_, lon_ vectors by generating a hypothesis according to interval amounts. (intervals are in pixels in the output tiles)
  boxm2_volm_loc_hypotheses(vpgl_lvcs_sptr lvcs, volm_tile tile, unsigned interval_i, unsigned interval_j, float altitude, vcl_vector<vil_image_view<float> >& dems, vcl_vector<vpgl_geo_camera*>& cams);

  //: construct the output tile image using the score, score vector's size need to be same as locs_ size
  void generate_output_tile(vcl_vector<float>& scores, int uncertainty_size_i, int uncertainty_size_j, float cut_off, vil_image_view<unsigned int>& out);

  volm_tile tile_;
  vcl_vector<vcl_pair<unsigned, unsigned> > pixels_; // also store the pixels of each location in the tile
  vcl_vector<vgl_point_3d<float> > locs_;
};

#endif  // boxm2_volm_locations_h_
