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

#include <utility>
#include <vbl/vbl_ref_count.h>
#include <vpgl/file_formats/vpgl_geo_camera.h>
#include <volm/volm_tile.h>
#include <vil/vil_image_view.h>
#include <vgl/vgl_point_3d.h>
#include <boxm2/boxm2_scene.h>


//: a set of 3d points in local coordinates of a scene, corresponds to an output tile.
//  this class knows how to construct an output tile to write probabilities
//  there can be multiple elevantions for a given lat, lon - just add to the 1d vector, best one is written to the output image
class boxm2_volm_loc_hypotheses : public vbl_ref_count
{
 public:

  boxm2_volm_loc_hypotheses(volm_tile tile) : tile_(std::move(tile)) {}

  //: construct the locs_ and pixels_ vectors by generating a hypothesis according to interval amounts in given the tile.
  //  Intervals are in meters.
  //  Only one elev hypothesis per location for now..
  //  keep only the locations that the scene covers
  void add_dems(const boxm2_scene_sptr& scene, unsigned interval_i, unsigned interval_j, float altitude, std::vector<vil_image_view<float> >& dems, std::vector<vpgl_geo_camera*>& cams);

  //: add a hypothesis given as a global lon, lat and elev for the pixel (i,j) of the tile, pass the pre-computed bounding box
  bool add(const boxm2_scene_sptr& scene, vgl_box_3d<double>& scene_bounding_box, double lon, double lat, double elev, unsigned i, unsigned j);

  //: add a hypothesis given as a global lon, lat and as local coords for the scene
  bool add(double lon, double lat, float cent_x, float cent_y, float cent_z);

  //: construct by reading from a binary file
  boxm2_volm_loc_hypotheses(const std::string& bin_file);

  //: construct the output tile image using the score, score vector's size need to be same as locs_ size
  void generate_output_tile(std::vector<float>& scores, int uncertainty_size_i, int uncertainty_size_j, float cut_off, vil_image_view<unsigned int>& out);

  bool write_hypotheses(const std::string& out_file);

  //: for debugging purposes
  bool write_hypotheses_kml(const boxm2_scene_sptr& scene, const std::string& kml_file);

  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  void b_read(vsl_b_istream &is);

  //: Return IO version number;
  short version() const { return 1; }

 public:
  volm_tile tile_;
  std::vector<std::pair<unsigned, unsigned> > pixels_; // also store the pixels of each location in the tile
  std::vector<vgl_point_3d<float> > locs_;
};

#endif  // boxm2_volm_locations_h_
