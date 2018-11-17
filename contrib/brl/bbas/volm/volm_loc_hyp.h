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
//   Jan 17, 2013, Ozge C. Ozcanli -- changed the class to a simple container where the order of hyps are is the insertion order
// \endverbatim
//

#include <vbl/vbl_ref_count.h>
#include <vpgl/file_formats/vpgl_geo_camera.h>
#include <volm/volm_tile.h>
#include <vil/vil_image_view.h>
#include <vgl/vgl_point_3d.h>
#include <boxm2/boxm2_scene.h>

//: a set of 3d points as lat,lon,elev.
//  this class knows how to read kml polygons to construct hypotheses with a specified density,
//  e.g. every X milliarcseconds, on the equator, 10m is 1000/3 milliarcseconds
//  WARNING: for now there is only 1 elevation for a given lat, lon - TODO: need to support more elevations
class volm_loc_hyp : public vbl_ref_count
{
 public:

  //: construct empty
  volm_loc_hyp() : current_(0) {}

  //: construct by reading from a binary file
  volm_loc_hyp(const std::string& bin_file);

  //: add the location to the set
  bool add(double lat, double lon, double elev);

  //: add all the points with elev higher than zero in this dem which are also within the given poly.
  //  for geocam's loaded from tfw files adjust_cam = false,
  //  however, if geocam is created explicitly using hemisphere and direction specifications for lat, lon then an adjustment is needed
  //  (e.g. for LIDAR tiles where cam is constructed using the naming convention)
  void add(vgl_polygon<double>& poly, vil_image_view<float>& dem, vpgl_geo_camera* geocam, int inc_i, int inc_j, bool adjust_cam = false,  char hemi = 'N', char dir = 'W');

  //unsigned size() { return cnt_; }
  unsigned size() { return (unsigned)locs_.size(); }

  bool write_hypotheses(const std::string& out_file);

  //: return false when all hyps are returned
  bool get_next(vgl_point_3d<double>& h);

  //: return false when all hyps are returned, return in a sequence which started from start and incremented by skip, e.g. i = start; i < cnt_; i+=skip
  bool get_next(unsigned start, unsigned skip, vgl_point_3d<double>& h);

  //: get the hypothesis closest to the given and its id if get_next method were to be used, very dummy for now, TODO: use upper_bound and lower_bound to limit search in lat map
  double get_closest(double lat, double lon, vgl_point_3d<double>& h, unsigned& hyp_id);

  //: check if there exists a hyp closer than the given size and return its id if there is
  //  warning: assumes that size is small enough so that euclidean distance approximates geodesic distance well
  bool exist(double lat, double lon, double size, unsigned& id);

  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  void b_read(vsl_b_istream &is);

  //: Return IO version number;
  short version() const { return 2; }

  //: release mem
  void release_memory() { locs_.clear(); }

  //: create a kml file
  void write_to_kml(const std::string& out_file, double size, bool const& write_as_dot = false);

 public:
  // lon is x, lat is y
  std::vector<vgl_point_3d<double> > locs_;
  unsigned current_;
};

#endif  // volm_loc_hyp_h_
