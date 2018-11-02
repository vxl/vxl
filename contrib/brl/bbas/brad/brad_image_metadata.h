//-----------------------------------------------------------------------------
//:
// \file
// \brief Collection of image metadata values needed for radiometric calculations
// \author D. E. Crispell
// \date January 27, 2012
//
// \verbatim
//  Modifications
//   Yi Dong Nov, 2017 -- Major update to adopt new image calibration mechanism
// \endverbatim
//----------------------------------------------------------------------------
#ifndef brad_image_metadata_h_
#define brad_image_metadata_h_

#include <string>
#include <iostream>
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_double_2.h>
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_vector_io.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_polygon.h>

struct image_time {
  int year, month, day, hour, min, sec;
};

class brad_image_metadata : public vbl_ref_count
{
 public:
  //: Constructor extracts information from NITF header and vendor-specific metadata file
  brad_image_metadata(std::string const& nitf_filename, std::string const& meta_folder = "");

  //: Default constructor
  brad_image_metadata() :
    n_bands_(0),
    gsd_(-1.0),
    verbose_(false)
  {}

  // position of sun relative to imaged location
  double sun_elevation_; // degrees above horizon
  double sun_azimuth_;   // degrees east of north

  // position of image sensor relative to imaged location
  double view_elevation_; // degrees above horizon
  double view_azimuth_;   // degrees east of north

  // band dependent abscalfactor and effectivebandwidth for image absolute radiometric correction (read from metadata file!!)
  // the length should be same as band number
  std::vector<double> abscal_;
  std::vector<double> effect_band_width_;

  // band dependent adjustment factors that are given by image vendor, updated annually
  std::vector<double> gains_;   // units W m^-2 sr^-1 um^-1 DN^-1, indexed by band type
  std::vector<double> offsets_; // units W m^-2 sr^-1 um^-1

  // band-averaged sun irradiance (includes term accounting for Earth-Sun distance)
  std::vector<double> normal_sun_irradiance_values_; // normalized solar irradiance value (divided by Earth-Sun distance in AU^2 unit)
  double sun_irradiance_; // units W m^-2
  //std::vector<double> sun_irradiance_values_;  // for multi-spectral imagery, there are multiple values

  image_time t_;
  unsigned number_of_bits_;

  std::string satellite_name_;
  double cloud_coverage_percentage_;
  //vnl_double_2 upper_left_;  // warning: upper_left_[0] is latitude, upper_left_[1] is longitude, similarly for the other corners
  //vnl_double_2 upper_right_;
  //vnl_double_2 lower_left_;
  //vnl_double_2 lower_right_;
  vgl_polygon<double> footprint_;  // the (lon,lat) corner coordinates of the image. there is no 2.5D vgl_polygon, so ignore the height...
  vgl_point_3d<double> lower_left_;  // x is lon, y is lat  // lower_left corner of the 'extent' of the satellite image (not necessarily lower left corner of the image, since the image may be rotated in plane)
  vgl_point_3d<double> upper_right_;
  vgl_point_3d<double> cam_offset_;  // these are the lat, lon, elev coords of upper left corner of the image read from the RPC camera
  std::string band_;  // PAN or MULTI
  unsigned n_bands_;  // 1 for PAN, 4 or 8 for MULTI

  double gsd_; // ground sampling distance of the image - parse from imd or pvl file
  bool verbose_;
  void set_verbose(bool verbose){verbose_ = verbose;}
  //: parse header in nitf image, assumes that metadata files are in the same folder with the image
  //  If meta_folder is not empty, they are searched in that folder as well
  bool parse(std::string const& nitf_filename, std::string const& meta_folder = "");
  //: parse metadata file only
  bool parse_from_meta_file(std::string const& meta_filename);

  bool same_time(brad_image_metadata& other);
  bool same_day(brad_image_metadata& other);
  // return the time difference in collection times in hour and minutes
  unsigned time_minute_dif(brad_image_metadata& other);

  void print_time() { std::cout << "Year: " << t_.year << " Month: " << t_.month << " Day: " << t_.day << " hour: " << t_.hour << " min: " << t_.min << " sec: " << t_.sec << "; "; }

  void print_out_radiometric_parameter();

  void print_out_solar_irradiance();
  //: compare the lat, lon bounding boxes. treat as Euclidean coordinate system, good for small boxes
  bool same_extent(brad_image_metadata& other);

  //: Read band dependent calibration adjustment factors from file
  bool read_band_dependent_gain_offset();

  //: Read band dependetn solar exoatmospheric irradiance from file
  bool read_band_dependent_solar_irradiance();

  // ===========  binary I/O ================
  short version() const { return 5; }
  void b_write(vsl_b_ostream& os) const;
  void b_read(vsl_b_istream& is);

 protected:
  //: Parse Quickbird IMD file
  bool parse_from_imd(std::string const& filename);

  //: Parse QuickBird, WorldView IMD file to obtain all metadata without using image header
  bool parse_from_imd_only(std::string const& filename);

  //: Parse GeoEye PVL file
  bool parse_from_pvl(std::string const& filename);

  //: Parse GeoEye PVL file to obtain all metadata without using image header
  bool parse_from_pvl_only(std::string const& filename);

  //: Parse the required params for normalization from a text file with a known format,
  //  it can be used to calibrate images from any satellite if such files are created for each image of the satellite
  bool parse_from_txt(std::string const& filename);


private:
  // -------- The calibration adjustmnet factors table (update annually) ----------------------------
  //   (Ref: http://www.digitalglobe.com/resources/technical-information)
  static const std::string gain_offset_file_name;    // ("brad_sat_img_calibration_table.txt")
  static const std::string sun_irradiance_file_name; // ("brad_sat_img_sub_irradiance_table.txt")
  static const std::string calibration_dir;          // (VXL_LIB_DIR)

};

typedef vbl_smart_ptr<brad_image_metadata> brad_image_metadata_sptr;

//: Write brad_image_metadata to stream
std::ostream&  operator<<(std::ostream& s, brad_image_metadata const& md);

//: Read brad_image_metadata from stream
std::istream&  operator>>(std::istream& s, brad_image_metadata& md);

#endif
