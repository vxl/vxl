//This is brl/bbas/volm/volm_loc_hyp.h
#ifndef volm_loc_hyp_h_
#define volm_loc_hyp_h_
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
#include <boxm2/boxm2_scene.h>

//: a set of 3d points as lat,lon,elev
//  stored as a map of lon values mapping to a map of lat values mapping to elev values --> this is for fast access later
//  this class knows how to read kml polygons to construct hypotheses with a specified density,
//  e.g. every X milliarcseconds, on the equator, 10m is 1000/3 milliarcseconds
//  WARNING: for now there is only 1 elevation for a given lat, lon - TODO: need to support more elevations
class volm_loc_hyp : public vbl_ref_count
{
 public:

  //: construct empty
  volm_loc_hyp() : cnt_(0), current_(0) {}

  //: construct by reading from a binary file
  volm_loc_hyp(vcl_string bin_file);

  //: add the location to the set
  bool add(float lat, float lon, float elev);

  //: add all the points with elev higher than zero in this dem which are also within the given poly
  //  for geocam's loaded from tfw files adjust_cam = false, 
  //  however, if geocam is created explicitly using hemisphere and direction specifications for lat, lon then an adjustment is needed 
  //  (e.g. for LIDAR tiles where cam is constructed using the naming convention)
  void add(vgl_polygon<double>& poly, vil_image_view<float>& dem, vpgl_geo_camera* geocam, int inc_i, int inc_j, bool adjust_cam = false,  char hemi = 'N', char dir = 'W');
  
  unsigned size() { return cnt_; }

  bool write_hypotheses(vcl_string out_file);

  //: return false when all hyps are returned
  bool get_next(vgl_point_3d<float>& h);

  //: return false when all hyps are returned, return in a sequence which started from start and incremented by skip, e.g. i = start; i < cnt_; i+=skip
  bool get_next(unsigned start, unsigned skip, vgl_point_3d<float>& h);
  
  //: get the hypothesis closest to the given and its id if get_next method were to be used, very dummy for now, TODO: use upper_bound and lower_bound to limit search in lat map
  bool get_closest(double lat, double lon, vgl_point_3d<float>& h, unsigned& hyp_id);

  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  void b_read(vsl_b_istream &is);

  //: Return IO version number;
  short version() const { return 1; }

 public:
  vcl_map<float, vcl_map<float, float> > locs_;  // map latitudes to maps of longitudes and elev (for now there is only one elevation for lat, lon)
  unsigned cnt_;
  unsigned current_;
  vcl_map<float, vcl_map<float, float> >::const_iterator current_lat_iter_;
  vcl_map<float, float>::const_iterator current_lon_iter_;
};

#endif  // volm_loc_hyp_h_
