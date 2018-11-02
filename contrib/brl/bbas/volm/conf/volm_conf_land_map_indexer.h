// This is brl/bbas/volm/conf/volm_conf_land_map_indexer.h
#ifndef volm_conf_land_map_indexer_h_
#define volm_conf_land_map_indexer_h_
//:
// \file Given a certain region and location density, this class creates a list of locations points, with its land_id, along the land region boundary.
//       For example, if a water region is given, a list of location points along this water region will be created as put into database
//       Note that the location are defined by the lon/lat values
//       The data resource can be from land map such as NLCD, GeoCover, or the region defined by its boundary
//
// \author Yi Dong
// \data August 14, 2014
// \verbatim
//   Yi Dong     SEP--2014    added height value for each location point
// \endverbatim
#include <iostream>
#include <iomanip>
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
#include <vil/vil_save.h>
#include <vil/vil_image_view.h>
#include <vsl/vsl_binary_io.h>
#include <volm/volm_category_io.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_box_2d.h>
#include <vpgl/vpgl_lvcs.h>
#include <vpgl/file_formats/vpgl_geo_camera.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_polygon.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vgl/io/vgl_io_box_2d.h>
#include <vgl/io/vgl_io_point_3d.h>
#include <vsl/vsl_map_io.h>

typedef std::map<unsigned char, std::vector<vgl_point_3d<double> > > volm_conf_loc_map;

class volm_conf_land_map_indexer;
typedef vbl_smart_ptr<volm_conf_land_map_indexer> volm_conf_land_map_indexer_sptr;

class volm_conf_land_map_indexer : public vbl_ref_count
{
public:
  // ================ constructor ===================
  //: default constructor
  volm_conf_land_map_indexer() = default;
  //: constructor from a given bounding box
  volm_conf_land_map_indexer(vgl_box_2d<double> const& bbox, double const& density);
  volm_conf_land_map_indexer(vgl_box_2d<float>  const& bbox, float  const& density);

  //: constructor from a min location point and max location point
  volm_conf_land_map_indexer(double const& min_lon, double const& max_lon, double const& min_lat, double const& max_lat, double const& density);
  volm_conf_land_map_indexer(float  const& min_lon, float  const& max_lon, float  const& min_lat, float  const& max_lat, float  const& density);

  //: constructor from a binary file
  volm_conf_land_map_indexer(std::string const& bin_file);

  //: destructor
  ~volm_conf_land_map_indexer() override = default;

  // ================ access ===================
  vgl_box_2d<double> bbox() const { return bbox_; }
  double density() const { return density_; }
  vpgl_lvcs_sptr lvcs() const { return lvcs_; }
  volm_conf_loc_map& land_locs() { return land_locs_; }

  // ================ method ===================
  //: list of land type in the database
  std::vector<unsigned char> land_types() const;

  //: bounding box string
  std::string box_string() const;

  //: number of land types in current database
  unsigned nland_type() const { return (unsigned)land_locs_.size(); }

  //: number of location points inside current region
  unsigned nlocs() const;

  //: number of location points that have certain land type
  unsigned nlocs(unsigned char const& land_id) const;

  //: add a location point (won't added into land_locs_ if input location is out of the bbox)
  bool add_locations(vgl_point_3d<double> const& loc, unsigned char const& land_id);

  //: add location points from a input image and its camera (assuming height value is -1.0 for all edge points from image)
  bool add_locations(vil_image_view<vxl_byte> const& image, vpgl_geo_camera* camera, std::string const& img_type = "nlcd");

  //: add location points from a list of points with their land properties (assuming height value is constant for all points in the list)
  bool add_locations(std::vector<vgl_point_2d<double> > const& locs, unsigned char const& land, double const& height = -1.0, double const& density = -1.0);

  //: add location points from boundary of a region with their land properties (assuming height value is constant for the polygon)
  bool add_locations(vgl_polygon<double> const& poly, unsigned char const& land, double const& height = -1.0);

  //: add location points from a line network by searching all the intersections inside the network (assuming height value is -1.0)
  bool add_locations(std::vector<std::vector<vgl_point_2d<double> > > const& lines, std::vector<unsigned char> const& lines_prop);

  //: upsample the location list to desired density (assuming input point locations are defined by wgs84)
  void upsample_location_list(std::vector<vsol_point_2d_sptr> const& in_locs, std::vector<vsol_point_2d_sptr>& out_locs, double const& density = -1.0);

  //: upsample the region boundary to desired density (assuming input point locations are defined by  wgs84)
  void upsample_region_boundary(vgl_polygon<double> const& poly, std::vector<vsol_point_2d_sptr>& out_locs, double const& density = -1.0);

  //: write_out the binary file for storage (won't write if the database is empty...)
  bool write_out_bin(std::string const& bin_file) const;

  // ================ visualization ===================
  //: write locations having certain land category into kml
  bool write_out_kml(std::string const& kml_file, unsigned char const& land_id, double const& size = 1E-5, bool const& is_write_as_dot = false) const;

  //: write all locations into kml file.  Note the result kml can be very large
  bool write_out_kml(std::string const& kml_file, double const& size = 1E-5, bool const& is_write_as_dot = false) const;

  // ================  binary I/O ===================
  //: version
  unsigned version() const { return 1; }

  //: binary IO write
  void b_write(vsl_b_ostream& os) const;

  //: binary IO read
  void b_read(vsl_b_istream& is);

private:
  //: region bounding box
  vgl_box_2d<double> bbox_;

  //: location density
  double density_;

  //: lvcs with wgs84 and lower_left as origin
  vpgl_lvcs_sptr lvcs_;

  //: map of boundary locations with their land type as key
  volm_conf_loc_map land_locs_;

  //: write a vector of locations with their land id into kml
  void write_out_kml_locs(std::ofstream& ofs, std::vector<vgl_point_3d<double> > const& locations, unsigned char land_id, double const& size,  bool const& is_write_as_dot) const;

};

#endif // volm_conf_land_map_indexer_h_
