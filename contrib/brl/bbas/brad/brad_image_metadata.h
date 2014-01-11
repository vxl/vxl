//-----------------------------------------------------------------------------
//:
// \file
// \brief Collection of image metadata values needed for radiometric calculations
// \author D. E. Crispell
// \date January 27, 2012
//
//----------------------------------------------------------------------------
#ifndef brad_image_metadata_h_
#define brad_image_metadata_h_

#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>

#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vnl/vnl_double_2.h>
#include <vsl/vsl_binary_io.h>
#include <vgl/vgl_point_3d.h>

struct image_time {
  int year, month, day, hour, min;
};

class brad_image_metadata : public vbl_ref_count
{
 public:
  //: Constructor extracts information from NITF header and vendor-specific metadata file
  brad_image_metadata(vcl_string const& nitf_filename, vcl_string const& meta_folder = "");
  //: Default constructor
  brad_image_metadata() : gain_(1.0), offset_(0.0) {}

  // position of sun relative to imaged location
  double sun_elevation_; // degrees above horizon
  double sun_azimuth_;   // degrees east of north

  // position of image sensor relative to imaged location
  double view_elevation_; // degrees above horizon
  double view_azimuth_;   // degrees east of north

  // gain and offset required to convert digital number (DN) to band-averaged radiance value
  double gain_;           // units W m^-2 sr^-1 um^-1 DN^-1
  double offset_;         // units W m^-2 sr^-1 um^-1

  // band-averaged sun irradiance (includes term accounting for Earth-Sun distance)
  double sun_irradiance_; // units W m^-2
  vcl_vector<double> sun_irradiance_values_;  // for multi-spectral imagery, there are multiple values

  image_time t_;
  unsigned number_of_bits_;

  vcl_string satellite_name_;
  double cloud_coverage_percentage_;
  //vnl_double_2 upper_left_;  // warning: upper_left_[0] is latitude, upper_left_[1] is longitude, similarly for the other corners
  //vnl_double_2 upper_right_;
  //vnl_double_2 lower_left_;
  //vnl_double_2 lower_right_;
  vgl_point_3d<double> lower_left_;  // x is lon, y is lat  // lower_left corner of the 'extent' of the satellite image (not necessarily lower left corner of the image, since the image may be rotated in plane)
  vgl_point_3d<double> upper_right_; 
  vgl_point_3d<double> cam_offset_;  // these are the lat, lon, elev coords of upper left corner of the image read from the RPC camera
  vcl_string band_;  // PAN or MULTI
  unsigned n_bands_;  // 1 for PAN, 4 or 8 for MULTI
  vcl_vector<vcl_pair<double, double> > gains_;  // calibrating multispectral imagery requires a different gain, offset for each band
  double gsd_; // ground sampling distance of the image - parse from imd or pvl file

  //: parse header in nitf image, assumes that metadata files are in the same folder with the image
  //  If meta_folder is not empty, they are searched in that folder as well
  bool parse(vcl_string const& nitf_filename, vcl_string const& meta_folder = "");

  bool same_time(brad_image_metadata& other);
  // return the time difference in collection times in units of minutes
  unsigned same_day_time_dif(brad_image_metadata& other);
  
  //: compare the lat, lon bounding boxes. treat as Euclidean coordinate system, good for small boxes
  bool same_extent(brad_image_metadata& other);

  // ===========  binary I/O ================
  short version() const { return 2; }
  void b_write(vsl_b_ostream& os) const;
  void b_read(vsl_b_istream& is);

 protected:
  //: Parse Quickbird IMD file
  bool parse_from_imd(vcl_string const& filename);
  //: Parse GeoEye PVL file
  bool parse_from_pvl(vcl_string const& filename);
};

typedef vbl_smart_ptr<brad_image_metadata> brad_image_metadata_sptr;

//: Write brad_image_metadata to stream
vcl_ostream&  operator<<(vcl_ostream& s, brad_image_metadata const& md);

//: Read brad_image_metadata from stream
vcl_istream&  operator>>(vcl_istream& s, brad_image_metadata& md);

#endif
