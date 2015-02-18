#include "brad_image_metadata.h"
//:
// \file

#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vul/vul_awk.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vcl_sstream.h>
#include <vcl_cassert.h>

#include <vil/vil_image_resource.h>
#include <vil/vil_load.h>
#include <vil/file_formats/vil_nitf2_image.h>
#include <vpgl/file_formats/vpgl_nitf_rational_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>
#include <vpgl/vpgl_lvcs.h>
#include <vpgl/algo/vpgl_backproject.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_intersection.h>

#include <brad/brad_sun_pos.h>

// Construct using nitf and metadata file
brad_image_metadata::brad_image_metadata(vcl_string const& nitf_filename, vcl_string const& meta_folder)
{
  if (!parse(nitf_filename, meta_folder)) {
    vcl_cerr << "ERROR parsing image metadata\n";
    gsd_ = -1;
  }
  vcl_cout << "!!!! lower left lon: " << lower_left_.x() << " lat: " << lower_left_.y() << '\n';
  vcl_cout << "!!!! upper right lon: " << upper_right_.x() << " lat: " << upper_right_.y() << '\n';

}

// Write brad_image_metadata to stream
vcl_ostream&  operator<<(vcl_ostream& s, brad_image_metadata const& md)
{
   s << "sun_elevation = " << md.sun_elevation_ << '\n'
     << "sun_azimuth = " << md.sun_azimuth_ << '\n'
     << "view_elevation = " << md.view_elevation_ << '\n'
     << "view_azimuth = " << md.view_azimuth_ << '\n'
     << "gain = " << md.gain_ << '\n'
     << "offset = " << md.offset_ << '\n'
     << "sun_irradiance = " << md.sun_irradiance_ << '\n'
     << "number_of_bits = " << md.number_of_bits_ << vcl_endl;

   return s;
}

// Read brad_image_metadata from stream
vcl_istream&  operator>>(vcl_istream& s, brad_image_metadata& md)
{
   vcl_string input;
   while (!s.eof()) {
      s >> input;
      if (input=="sun_elevation") {
         s >> input;
         s >> md.sun_elevation_;
      }
      if (input=="sun_azimuth") {
         s >> input;
         s >> md.sun_azimuth_;
      }
      if (input=="view_elevation") {
         s >> input;
         s >> md.view_elevation_;
      }
      if (input=="view_azimuth") {
         s >> input;
         s >> md.view_azimuth_;
      }
      if (input == "gain") {
         s >> input;
         s >> md.gain_;
      }
      if (input == "offset") {
         s >> input;
         s >> md.offset_;
      }
      if (input == "sun_irradiance") {
         s >> input;
         s >> md.sun_irradiance_;
      }
      if (input == "number_of_bits") {
         s >> input;
         s >> md.number_of_bits_;
      }
   }
   return s;
}


bool brad_image_metadata::parse_from_imd(vcl_string const& filename)
{
  vcl_ifstream ifs( filename.c_str() );
  if (!ifs.good()){
    vcl_cerr << "Error opening file " << filename << vcl_endl;
    return false;
  }
  n_bands_ = 0;
  // now parse the IMD file
  vul_awk awk(ifs);
  double absCalfact = 1.0;
  double effectiveBand = 1.0;
  lower_left_.set(181, 91, 10000);
  upper_right_.set(-181,-91, -10000);
  for (; awk; ++awk)
  {
    vcl_stringstream linestr(awk.line());
    vcl_string tag;
    linestr >> tag;

    if (tag.compare("absCalFactor") == 0) {
      linestr >> tag;  // read =
      linestr >> absCalfact;
      continue;
    }
    if (tag.compare("effectiveBandwidth") == 0) {
      linestr >> tag;  // read =
      linestr >> effectiveBand;
      gains_.push_back(vcl_pair<double, double>(absCalfact/effectiveBand, 0.0));
      continue;
    }
    if (tag.compare("cloudCover") == 0) {
      linestr >> tag;
      linestr >> cloud_coverage_percentage_;
      continue;
    }
    if (tag.compare("productType") == 0) {
      linestr >> tag;
      linestr >> tag;
      if (tag.find("Basic") == vcl_string::npos)
        return false;
    }
    if (tag.compare("meanProductGSD") == 0 || tag.compare("meanCollectedGSD") == 0) {
      linestr >> tag;
      linestr >> gsd_;
      continue;
    }
    if (tag.compare("satId") == 0) {
      linestr >> tag;
      linestr >> satellite_name_;
      satellite_name_ = satellite_name_.substr(satellite_name_.find_first_of("\"")+1, satellite_name_.find_last_of("\"")-1);
      continue;
    }
    if (tag.compare("LLLon") == 0 || tag.compare("URLon") == 0 || tag.compare("ULLon") == 0 || tag.compare("LRLon") == 0) {
      linestr >> tag;
      double x;
      linestr >> x;
      if (lower_left_.x() > x) lower_left_.set(x, lower_left_.y(), lower_left_.z());
      if (upper_right_.x() < x) upper_right_.set(x, upper_right_.y(), upper_right_.z());
      continue;
    }
    if (tag.compare("LLLat") == 0 || tag.compare("URLat") == 0 || tag.compare("ULLat") == 0 || tag.compare("LRLat") == 0) {
      linestr >> tag;
      double y;
      linestr >> y;
      if (lower_left_.y() > y) lower_left_.set(lower_left_.x(), y, lower_left_.z());
      if (upper_right_.y() < y) upper_right_.set(upper_right_.x(), y, upper_right_.z());
      continue;
    }
    if (tag.compare("LLHAE") == 0 || tag.compare("URHAE") == 0 || tag.compare("ULHAE") == 0 || tag.compare("LRHAE") == 0) {  //CAUTION: height above ELLIPSOID (not mean sea level/ geoid)
      linestr >> tag;
      double z;
      linestr >> z;
      if (lower_left_.z() > z) lower_left_.set(lower_left_.x(), lower_left_.y(), z);
      if (upper_right_.z() < z) upper_right_.set(upper_right_.x(), upper_right_.y(), z);
      continue;
    }
    if (tag.compare("bandId") == 0) {
      linestr >> tag;
      vcl_string band_str;
      linestr >> band_str;
      if (band_str.find("P") != vcl_string::npos) {
        band_ = "PAN";
      } else
        band_ = "MULTI";
      continue;
    }
    if (tag.compare("BEGIN_GROUP") == 0) {
      n_bands_++;
      continue;
    }
  }
  n_bands_--; // there is an extra BEGIN_GROUP for some other image info not related to individual bands
  vcl_cout << "  cloud coverage percentage : " << cloud_coverage_percentage_ << " band: " << band_ << " number of bands: " << n_bands_ << vcl_endl;
  gain_ = absCalfact/effectiveBand;
  offset_ = 0.0;
  return true;
}

// only parse the cloud coverage for now
bool brad_image_metadata::parse_from_pvl(vcl_string const& filename)
{
  //vcl_cout << "Parse from PVL file is not implemented yet!\n";
  vcl_ifstream ifs( filename.c_str() );
  if (!ifs.good()){
    vcl_cerr << "Error opening file " << filename << vcl_endl;
    return false;
  }
  n_bands_ = 0;
  lower_left_.set(181, 91, 10000);
  upper_right_.set(-181,-91, -10000);
  // now parse the IMD file
  vul_awk awk(ifs);
  for (; awk; ++awk)
  {
    vcl_stringstream linestr(awk.line());
    vcl_string tag;
    linestr >> tag;

    if (tag.compare("productCloudCoverPercentage") == 0) {
      linestr >> tag;
      linestr >> cloud_coverage_percentage_;
      continue;
    }
    if ((linestr.str().find("BEGIN_GROUP") != vcl_string::npos && linestr.str().find("upperRightCorner") != vcl_string::npos) ||
        (linestr.str().find("BEGIN_GROUP") != vcl_string::npos && linestr.str().find("upperLeftCorner") != vcl_string::npos) ||
        (linestr.str().find("BEGIN_GROUP") != vcl_string::npos && linestr.str().find("lowerRightCorner") != vcl_string::npos) ||
        (linestr.str().find("BEGIN_GROUP") != vcl_string::npos && linestr.str().find("lowerLeftCorner") != vcl_string::npos) )
    {
      vcl_stringstream linestr(awk.line());
      while (linestr.str().find("latitude") == vcl_string::npos) {
         ++awk;
         //linestr = vcl_stringstream(awk.line());
         linestr.clear();
         linestr.str(awk.line());
      }
      vcl_string dummy; linestr >> dummy; linestr >> dummy;
      double y; linestr >> y;
      while (linestr.str().find("longitude") == vcl_string::npos) {
         ++awk;
         //linestr = vcl_stringstream(awk.line());
         linestr.clear();
         linestr.str(awk.line());
      }
      linestr >> dummy; linestr >> dummy;
      double x; linestr >> x;
      while (linestr.str().find("height") == vcl_string::npos) {
         ++awk;
         //linestr = vcl_stringstream(awk.line());
         linestr.clear();
         linestr.str(awk.line());
      }
      linestr >> dummy; linestr >> dummy;
      double z; linestr >> z;
      if (lower_left_.x() > x) lower_left_.set(x, lower_left_.y(), lower_left_.z());
      if (lower_left_.y() > y) lower_left_.set(lower_left_.x(), y, lower_left_.z());
      if (lower_left_.z() > z) lower_left_.set(lower_left_.x(), lower_left_.y(), z);

      if (upper_right_.x() < x) upper_right_.set(x, upper_right_.y(), upper_right_.z());
      if (upper_right_.y() < y) upper_right_.set(upper_right_.x(), y, upper_right_.z());
      if (upper_right_.z() < z) upper_right_.set(upper_right_.x(), upper_right_.y(), z);
      continue;
    }
    if (tag.compare("productSpectralType") == 0) {
      linestr >> tag;
      vcl_string band_str;
      linestr >> band_str;
      if (band_str.find("PAN") != vcl_string::npos)
        band_ = "PAN";
      else
        band_ = "MULTI";
      continue;
    }
    if (tag.compare("pixelSpacing") == 0) {
      linestr >> tag;
      linestr >> gsd_;
      continue;
    }
    if (tag.compare("numberOfSpectralBands") == 0) {
      linestr >> tag;
      linestr >> n_bands_;
      continue;
    }
    if (tag.compare("satelliteName") == 0) {
      linestr >> tag;
      linestr >> satellite_name_;
    }
    if (linestr.str().find("BEGIN_GROUP") != vcl_string::npos && linestr.str().find("radiometry") != vcl_string::npos) {
      ++awk;
      linestr.clear();
      linestr.str(awk.line());

      vcl_string dummy; linestr >> dummy; linestr >> dummy;
      double g; linestr >> g;

       ++awk;
      linestr.clear();
      linestr.str(awk.line());
      linestr >> dummy; linestr >> dummy;
      double off; linestr >> off;

      gains_.push_back(vcl_pair<double, double>(g, off));
    }
  }
  vcl_cout << "cloud coverage percentage : " << cloud_coverage_percentage_ << " band: " << band_ << " number of bands: " << n_bands_ << vcl_endl;
  return true;
}

//: Parse the required params for normalization and other tasks from a text file with a known format,
//  it can be used to calibrate images from any satellite if such files are created for each image of the satellite
//  example file:
// productSpectralType MULTI
// meanGSD 2.824
// LLLat 33.216
// LLLon 44.219
// LLHAE 33.93
// URLat 33.216
// URLon 44.4
// URHAE 34.13
// numberOfSpectralBands 4
// solar_irradiance 1924.59 1843.08 1574.77 1113.71
// gain_offset 0.2359 0 0.1453 0 0.1785 0 0.1353 0
bool brad_image_metadata::parse_from_txt(vcl_string const& filename, vcl_vector<double>& solar_irrads)
{
  vcl_cout << "parsing radiometric calibration and atmospheric normalization parameters from: " << filename << "...\n";
  vcl_ifstream ifs( filename.c_str() );
  if (!ifs.good()){
    vcl_cerr << "Error opening file " << filename << vcl_endl;
    return false;
  }
  n_bands_ = 0;
  lower_left_.set(181, 91, 10000);
  upper_right_.set(-181,-91, -10000);
  double val;

  bool parsed_gain_offset = false, parsed_gains = false, parsed_sun_irradiance = false;
  bool parsed_coverage_percentage_ = false;
  vul_awk awk(ifs);
  for (; awk; ++awk)
  {
    vcl_stringstream linestr(awk.line());
    vcl_string tag;
    linestr >> tag;

    if (tag.compare("CloudCoverPercentage") == 0) {
      linestr >> cloud_coverage_percentage_;
      parsed_coverage_percentage_ = true;
      continue;
    }
    if (tag.compare("SunAngle") == 0) {
      linestr >> sun_azimuth_; linestr >> sun_elevation_;
      continue;
    }
    if (tag.compare("productSpectralType") == 0) {
      vcl_string band_str;
      linestr >> band_str;
      if (band_str.find("PAN") != vcl_string::npos)
        band_ = "PAN";
      else
        band_ = "MULTI";
      continue;
    }
    if (tag.compare("meanGSD") == 0) {
      linestr >> gsd_;
      continue;
    }
    if (tag.compare("LLLat") == 0) {
      linestr >> val;
      lower_left_.set(lower_left_.x(), val, lower_left_.z());
      continue;
    }
    if (tag.compare("LLLon") == 0) {
      linestr >> val;
      lower_left_.set(val, lower_left_.y(), lower_left_.z());
      continue;
    }
    if (tag.compare("LLHAE") == 0) {
      linestr >> val;
      lower_left_.set(lower_left_.x(), lower_left_.y(), val);
      continue;
    }

    if (tag.compare("URLat") == 0) {
      linestr >> val;
      upper_right_.set(upper_right_.x(), val, upper_right_.z());
      continue;
    }
    if (tag.compare("URLon") == 0) {
      linestr >> val;
      upper_right_.set(val, upper_right_.y(), upper_right_.z());
      continue;
    }
    if (tag.compare("URHAE") == 0) {
      linestr >> val;
      upper_right_.set(upper_right_.x(), upper_right_.y(), val);
      continue;
    }
    if (tag.compare("numberOfSpectralBands") == 0) {
      linestr >> n_bands_;
      continue;
    }
    if (tag.compare("solar_irradiance") == 0) {
      if (!n_bands_) {
        vcl_cerr << "n_bands tag should precede solar_irradiance tag! Problems parsing: " << filename << "\n";
        return false;
      }
      solar_irrads.clear();
      for (unsigned i = 0; i < n_bands_; i++) {
        linestr >> val;
        solar_irrads.push_back(val);
      }
      parsed_sun_irradiance = true;
      continue;
    }
    if (tag.compare("gain_offset") == 0) {
      if (!n_bands_) {
        vcl_cerr << "n_bands tag should preceed solar_irradiance tag! Problems parsing: " << filename << "\n";
        return false;
      }
      if (n_bands_ == 1) {
        linestr >> gain_; linestr >> offset_;
        parsed_gain_offset = true;
      } else {
        gains_.clear();
        for (unsigned i = 0; i < n_bands_; i++) {
          linestr >> gain_; linestr >> offset_;
          gains_.push_back(vcl_pair<double, double>(gain_, offset_));
        }
        parsed_gains = true;
      }
      continue;
    }
  }

  if (n_bands_ == 1 && (!parsed_gain_offset || !parsed_sun_irradiance)) {
    vcl_cerr << "the metadata file must include values for gain, offset, and solar irradiance" << vcl_endl;
    return false;
  }
  else if(n_bands_ > 1 && (!parsed_gains || !parsed_sun_irradiance)) {
    vcl_cerr << "the metadata file must include values for gain, offset, and solar irradiance" << vcl_endl;
    return false;
  }

  if(parsed_coverage_percentage_) {
    vcl_cout << "cloud coverage percentage : " << cloud_coverage_percentage_ << " band: " << band_ << " number of bands: " << n_bands_ << vcl_endl;
  }

  return true;
}

//: parse header in nitf image, assumes that metadata files are in the same folder with the image
//  If meta_folder is not empty, they are searched in that folder as well
bool brad_image_metadata::parse(vcl_string const& nitf_filename, vcl_string const& meta_folder)
{
  vil_image_resource_sptr image = vil_load_image_resource(nitf_filename.c_str());
  if (!image)
  {
    vcl_cout << "NITF image load failed!\n";
    return false;
  }

  vcl_string format = image->file_format();
  vcl_string prefix = format.substr(0,4);

  if (prefix != "nitf")
  {
    vcl_cout << "source image is not NITF\n";
    return false;
  }

  //cast to an nitf2_image
  vil_nitf2_image *nitf_image = static_cast<vil_nitf2_image*>(image.ptr());

  vpgl_nitf_rational_camera nitf_cam(nitf_image, false);

  //upper_left_ = nitf_cam.upper_left(); // caution, lat is x and lon is y when read from nitf camera with this method
  upper_right_.set(nitf_cam.upper_right()[1], nitf_cam.upper_right()[0], 0);
  lower_left_.set(nitf_cam.lower_left()[1], nitf_cam.lower_left()[0], 0);
  //lower_right_ = nitf_cam.lower_right();
  vcl_cout << "!!!! lower left lon: " << lower_left_.x() << " lat: " << lower_left_.y() << '\n';
  vcl_cout << "!!!! upper right lon: " << upper_right_.x() << " lat: " << upper_right_.y() << '\n';

  double xoff, yoff, zoff;
  xoff = nitf_cam.offset(vpgl_rational_camera<double>::X_INDX);
  yoff = nitf_cam.offset(vpgl_rational_camera<double>::Y_INDX);
  zoff = nitf_cam.offset(vpgl_rational_camera<double>::Z_INDX);
  cam_offset_.set(xoff, yoff, zoff);

  //get NITF information
  vcl_vector< vil_nitf2_image_subheader* > headers = nitf_image->get_image_headers();
  vil_nitf2_image_subheader* hdr = headers[0];

  double sun_el;
  double sun_az;
  if (!hdr->get_sun_params(sun_el, sun_az)) {
    vcl_cerr << "failed to obtain sun parameters info\n";
    return false;
  }

  sun_elevation_ = sun_el;
  sun_azimuth_ = sun_az;

  int year, month, day, hour, min, sec;
  if (!hdr->get_date_time(year, month, day, hour,  min, sec)) {
    vcl_cerr << "failed to obtain date time info\n";
    return false;
  }
  t_.year = year; t_.month = month; t_.day = day; t_.hour = hour; t_.min = min; t_.sec = sec;

  number_of_bits_ = hdr->get_number_of_bits_per_pixel();



  // compute satellite az,el values for center of image

  double off_u, off_v;
  nitf_cam.image_offset(off_u, off_v);
  // get lat,lon offsets for local euclidean coord. system origin
  double lon0 = nitf_cam.offset(vpgl_rational_camera<double>::X_INDX);
  double lat0 = nitf_cam.offset(vpgl_rational_camera<double>::Y_INDX);
  // get elevation offset value
  double el0 = nitf_cam.offset(vpgl_rational_camera<double>::Z_INDX);
  vpgl_lvcs lvcs(lat0, lon0, el0, vpgl_lvcs::wgs84, 0, 0, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  vpgl_local_rational_camera<double> local_cam(lvcs, nitf_cam);
  // determine direction to camera
  vgl_vector_3d<double> to_camera;
  vpgl_backproject::direction_to_camera(local_cam, vgl_point_3d<double>(0,0,0), to_camera);
  // convert vector to az,el
  const double rad_to_deg = 180.0 / vnl_math::pi;
  // degrees above horizon
  view_elevation_ = vcl_asin(to_camera.z()) * rad_to_deg;
  // degrees east of north
  view_azimuth_ = vcl_atan2(to_camera.x(), to_camera.y()) * rad_to_deg;
  if (view_azimuth_ < 0)
    view_azimuth_ += 360;

  vcl_string dirname = vul_file::dirname(nitf_filename);

  vcl_string img_info = hdr->get_image_source();
  vcl_cout << "img_info: " << img_info << vcl_endl;

  // look for metadata files with known formats recursively in the directory of the image
  // If we find one, check file name to see if it is for the same image, if so parse it
  vcl_string imagename = vul_file::strip_directory(nitf_filename);
  imagename = vul_file::strip_extension(imagename);
  vcl_cout << "imagename: " << imagename << vcl_endl;

  vcl_string in_dir = dirname + "/*.*";
  vcl_string meta_filename = "";
  for (vul_file_iterator fn = in_dir.c_str(); fn; ++fn) {
    vcl_string filename = fn();
    vcl_string name = vul_file::strip_directory(filename);
    name = vul_file::strip_extension(name);
    vcl_string ext = vul_file::extension(filename);
    if (imagename.find(name) != vcl_string::npos &&
        (ext.compare(".IMD") == 0 || ext.compare(".imd") == 0 ||
         ext.compare(".PVL") == 0 || ext.compare(".pvl") == 0)
       ) {
      meta_filename = filename;
      break;
    }
  }
  if (meta_filename.size() == 0 && meta_folder.size() != 0) {
    vcl_cout << " searching " << meta_folder << " for files with extensions .imd, .pvl or .txt" << vcl_endl;
    vcl_string in_dir = meta_folder + "/*.*";
    for (vul_file_iterator fn = in_dir.c_str(); fn; ++fn) {
      vcl_string filename = fn();
      vcl_string name = vul_file::strip_directory(filename);
      name = vul_file::strip_extension(name);
      vcl_string ext = vul_file::extension(filename);
      if (imagename.find(name) != vcl_string::npos &&
          (ext.compare(".IMD") == 0 || ext.compare(".imd") == 0 ||
           ext.compare(".PVL") == 0 || ext.compare(".pvl") == 0 ||
           ext.compare(".TXT") == 0 || ext.compare(".txt") == 0)
         ) {
        meta_filename = filename;
        break;
      }
    }
  }

  // n_bands_, gain_, offset_ (or gains_) and sun_irradiance (or sun_irradiance_values_) will be set
  //
  // TODO it would be nice to refactor this so that there are proper parsed gaurds on variables like
  // has_<VARIABLE>. also, define an order in which information is parsed (e.g., hard coded values, the
  // image subheader (TREs), then a metadata file (which themselves don't have to be mutually exclusive))

  // set gain offset defaults, some satellites' images do not require any adjustment
  gain_ = 1.0f;
  offset_ = 0.0f;
  double solar_irrad = 1500.0;
  vcl_vector<double> solar_irrads;  // for multi-spectral imagery there are multiple values
  bool parsed_sun_irradiance = false;
  // no metadata file provided; try providing some known values
  if (meta_filename.size() == 0) {
    // check if this is IKONOS
    vcl_string type = hdr->get_image_type(); // type mono is band PAN
    unsigned bpp = number_of_bits_;
    if (img_info.compare("IKONOS") == 0 && type.compare("MONO") == 0 && bpp == (unsigned)11) {
      vcl_cout << "Ikonos: bpp " << bpp << " type: " << type << vcl_endl;
      vcl_cout << "An 11-bit Panchromatic IKONOS image, setting gain & offset values according to tech document\n";
      n_bands_ = 1;
      gain_ = (10.0/161.0)/0.403;
      offset_ = 0.0;
    }
    else {
      vcl_cerr << "ERROR: could not set gain and offset for " << imagename << vcl_endl;
      return false;
    }
  }
  else {
    // n_bands_, gain and offset (or gains_) should be set if parsed_fine is true
    vcl_string ext = vul_file::extension(meta_filename);
    bool parsed_fine = false;
    if (ext.compare(".IMD") == 0 || ext.compare(".imd") == 0) {  // IMD files do not specify PAN gain if it is a multi image
      parsed_fine = parse_from_imd(meta_filename);
      if(parsed_fine) {
        if (n_bands_ >= 4) {
          gains_.insert(gains_.begin(), vcl_pair<double, double> (gain_, offset_)); // insert a dummy GAIN to account for PAN gain, this value will never be used
        }
      }
    } else if (ext.compare(".PVL") == 0 || ext.compare(".pvl") == 0) {  // pvl files also specify PAN gain even if it is a multi image (band 1 is PAN, 2-4 are multi)
      parsed_fine = parse_from_pvl(meta_filename);
      if(parsed_fine) { 
        gain_ = gains_[0].first;
        offset_ = gains_[0].second;
      }
    } else if (ext.compare(".TXT") == 0 || ext.compare(".txt") == 0) {
      parsed_fine = parse_from_txt(meta_filename, solar_irrads);
      if(parsed_fine) {
        if (n_bands_ == 1) {
          // solar_irrads will be populated
          assert(solar_irrads.size() > 0);
          solar_irrad = solar_irrads[0];
        } else if (n_bands_ >= 4) {
          gains_.insert(gains_.begin(), vcl_pair<double, double> (gain_, offset_)); // insert a dummy GAIN to account for PAN gain, this 0th value will never be used
        }
        parsed_sun_irradiance = true;
      }
    }
    else {
      vcl_cout << "ERROR unrecognized metadata file format: " << ext << " in name: " << meta_filename << "!\n";
      vcl_cerr << "      Could not set gain and offset for " << imagename << vcl_endl;
      return false;
    }
    if (!parsed_fine) {
      vcl_cerr << " Problems parsing meta-data files!\n";
      vcl_cout << " !!!!!!!!!! satellite name: " << satellite_name_ << " gsd: " << gsd_ << vcl_endl;
      return false;
    }

    for (unsigned i = 0; i < gains_.size(); i++) {
      vcl_cout << " gain: " << gains_[i].first << " off: " << gains_[i].second << vcl_endl;
    }
  }

  // solar irradiance is dependent on sensor because each has a different range of wavelengths they are sensitive to.
  // set solar irradiance to a reasonable default in case we don't have the information
  // "reasonable" is defined here as roughly in the range of the examples we know.
  if (!parsed_sun_irradiance) {
    solar_irrads.resize(n_bands_, 1500.0);

    if (img_info.find("IKONOS") != vcl_string::npos || nitf_filename.find("IK") != vcl_string::npos) {
      solar_irrad = 1375.8;
      satellite_name_ = "IKONOS";
    } else if (img_info.find("GeoEye-1") != vcl_string::npos || img_info.find("GEOEYE1") != vcl_string::npos || satellite_name_.compare("OV-5") == 0 ) { // OZGE TODO: check this one
      sun_irradiance_ = 1617;
      satellite_name_ = "GeoEye-1";
      // CAUTION: the order in this vector, should be the order of the bands in the image (i.e. for geoeye1 plane 0
      //          is blue, plane 1 is green, plane 2 is red and plane 3 is near-IR). this order may be different for
      //          different satellites
      if (n_bands_ == 1) {
        // pass
      } else if (n_bands_ == 4) {
        solar_irrads[0] = 1960; // Blue
        solar_irrads[1] = 1853; // Green
        solar_irrads[2] = 1505; // Red
        solar_irrads[3] = 1039; // near-IR
      } else {
        vcl_cerr << "ERROR unrecognized number of bands: " << n_bands_ << " from NITF " << nitf_filename << vcl_endl;;
        return false;
      }
    } else if (img_info.find("QuickBird") != vcl_string::npos || 
               nitf_filename.find("QB") != vcl_string::npos || 
               nitf_filename.find("QuickBird") != vcl_string::npos || 
               img_info.find("QB02") != vcl_string::npos ||
               satellite_name_.compare("QB02") == 0
               ) {
      sun_irradiance_ = 1381.7;
      satellite_name_ = "QuickBird";
      if (n_bands_ == 1) {
        // pass
      } else if (n_bands_ == 4) {
        solar_irrads[0] = 1924.59; // Blue
        solar_irrads[1] = 1843.08; // Green
        solar_irrads[2] = 1574.77; // Red
        solar_irrads[3] = 1113.71; // near-IR  // these values are from http://grasswiki.osgeo.org/wiki/QuickBird
      } else {
        vcl_cerr << "ERROR unrecognized number of bands: " << n_bands_ << " from NITF " << nitf_filename << vcl_endl;;
        return false;
      }
    } else if (img_info.find("WorldView") != vcl_string::npos || nitf_filename.find("WV") != vcl_string::npos || satellite_name_.compare("WV01") == 0 ) {
      sun_irradiance_ = 1580.814;
      satellite_name_ = "WorldView";
    } else if (img_info.find("WorldView2") != vcl_string::npos || img_info.find("WV02") != vcl_string::npos || satellite_name_.compare("WV02") == 0) {
      sun_irradiance_ = 1580.814;
      satellite_name_ = "WorldView2";
      // CAUTION: the order in this vector, should be the order of the bands in the image (i.e. for wv2 plane 0 is coastal (?), plane 1 is blue, etc.)
      //          this order may be different for different satellites
      if (n_bands_ == 1) {
        // pass
      } else if (n_bands_ == 8) {
        solar_irrads[0] = 1758.2229; // Coastal
        solar_irrads[1] = 1974.2416; // Blue
        solar_irrads[2] = 1856.4104; // Green
        solar_irrads[3] = 1738.4791; // Yellow
        solar_irrads[4] = 1559.4555; // Red
        solar_irrads[5] = 1342.0695; // Red Edge
        solar_irrads[6] = 1069.7302; // NIR1
        solar_irrads[7] = 861.2866;  // NIR2
      } else if (n_bands_ == 4) {
        solar_irrads[0] = 1974.2416; // Blue
        solar_irrads[1] = 1856.4104; // Green
        solar_irrads[2] = 1559.4555; // Red
        solar_irrads[3] = 1069.7302; // NIR1
      } else {
        vcl_cerr << "ERROR unrecognized number of bands: " << n_bands_ << " from NITF " << nitf_filename << vcl_endl;;
        return false;
      }
    } else if (img_info.find("DigitalGlobe") != vcl_string::npos) {
      solar_irrad = 1580.814;
      satellite_name_ = "DigitalGlobe";  // which satellite when the name is DigitalGlobe??
      vcl_cerr << "WARNING satellite name is DigitalGlobe but cannot determine which satellite!! so sun irradiance values are not set properly (esp. for multi-spectral images)" << vcl_endl;
    } else {
      vcl_cerr << "ERROR Could not find known satellite name in: " << img_info << " from NITF " << nitf_filename << vcl_endl;;
      vcl_cerr << "      Could not set solar irradiance for " << imagename << vcl_endl;
      return false;
    }
  }

  // a solar irradiance has been found; scale it using Earth-Sun distance
  double d = brad_sun_distance(year, month, day, hour, min);
  if (n_bands_ == 1) {
    vcl_cout << "solar_irradiance_: " << solar_irrad << " ";
    sun_irradiance_ = solar_irrad/(d*d);
    vcl_cout << " after scaling with Earth-Sun distance: " << sun_irradiance_ << vcl_endl;
  } else {
    assert(n_bands_ == solar_irrads.size());
    for (unsigned ii = 0; ii < solar_irrads.size(); ii++)
      vcl_cout << "solar_irradiance_values_[" << ii << "]: " << solar_irrads[ii] << vcl_endl;
    sun_irradiance_values_.resize(n_bands_, 1500.0);
    for (unsigned bandi = 0; bandi < n_bands_; bandi++)
      sun_irradiance_values_[bandi] = solar_irrads[bandi]/(d*d);
    vcl_cout << " .. after scaling with Earth-Sun distance..: " << d << "\n";
    for (unsigned ii = 0; ii < sun_irradiance_values_.size(); ii++)
      vcl_cout << "sun_irradiance_values_[" << ii << "]: " << sun_irradiance_values_[ii] << vcl_endl;
  }

  vcl_cout << " !!!!!!!!!! satellite name: " << satellite_name_ << " gsd: " << gsd_ << vcl_endl;
  vcl_cout << *this;

  return true;
}

bool brad_image_metadata::same_time(brad_image_metadata& other)
{
  if (this->t_.sec == other.t_.sec &&
    this->t_.min == other.t_.min &&
    this->t_.hour == other.t_.hour &&
    this->t_.day == other.t_.day &&
    this->t_.month == other.t_.month &&
    this->t_.year == other.t_.year)
      return true;
  else
    return false;
}

bool brad_image_metadata::same_day(brad_image_metadata& other)
{
  if (this->t_.day == other.t_.day &&
    this->t_.month == other.t_.month &&
    this->t_.year == other.t_.year)
      return true;
  else
    return false;
}

// return the time difference in collection times in units of minutes
unsigned brad_image_metadata::time_minute_dif(brad_image_metadata& other)
{
  if (this->t_.hour > other.t_.hour) {
    unsigned temp = other.t_.hour+1;
    unsigned hour_dif = this->t_.hour - temp;
    unsigned minute_dif = hour_dif * 60 + this->t_.min + (60-other.t_.min);
    return minute_dif;
  } else if (this->t_.hour < other.t_.hour) {
    unsigned temp = this->t_.hour+1;
    unsigned hour_dif = other.t_.hour - temp;
    unsigned minute_dif = hour_dif * 60 + other.t_.min + (60-this->t_.min);
    return minute_dif;
  } else { // hours are equal
    return (unsigned)vcl_abs(float(this->t_.min - other.t_.min));
  }
}

//: compare the lat, lon bounding boxes. treat as Euclidean coordinate system, good for small boxes
bool brad_image_metadata::same_extent(brad_image_metadata& other)
{
  // some images are rotated 'in-plane', so even though their bounding boxes overlap very well, the offset in the camera (upper left corner in the image) may be different
  if (cam_offset_.x() != other.cam_offset_.x() ||
      cam_offset_.y() != other.cam_offset_.y() ||
      cam_offset_.z() != other.cam_offset_.z())
      return false;

  vgl_box_2d<double> b1(lower_left_.x(), lower_left_.y(), upper_right_.x(), upper_right_.y());
  vgl_box_2d<double> b2(other.lower_left_.x(), other.lower_left_.y(), other.upper_right_.x(), other.upper_right_.y());
  if (vcl_abs(vgl_intersection(b1, b2).area() - b1.area()) < 0.000000001)
    return true;

  return false;
}

//: binary save self to stream
void brad_image_metadata::b_write(vsl_b_ostream& os) const
{
  vsl_b_write(os, version());
  vsl_b_write(os, sun_elevation_);
  vsl_b_write(os, sun_azimuth_);
  vsl_b_write(os, view_elevation_);
  vsl_b_write(os, view_azimuth_);
  vsl_b_write(os, gain_);
  vsl_b_write(os, offset_);
  vsl_b_write(os, sun_irradiance_);
  vsl_b_write(os, t_.day);
  vsl_b_write(os, t_.hour);
  vsl_b_write(os, t_.min);
  vsl_b_write(os, t_.month);
  vsl_b_write(os, t_.year);
  vsl_b_write(os, number_of_bits_);
  vsl_b_write(os, satellite_name_);
  vsl_b_write(os, cloud_coverage_percentage_);
  vsl_b_write(os, upper_right_.x());
  vsl_b_write(os, upper_right_.y());
  vsl_b_write(os, upper_right_.z());
  vsl_b_write(os, lower_left_.x());
  vsl_b_write(os, lower_left_.y());
  vsl_b_write(os, lower_left_.z());
  vsl_b_write(os, band_);
  vsl_b_write(os, n_bands_);
  vsl_b_write(os, cam_offset_.x());
  vsl_b_write(os, cam_offset_.y());
  vsl_b_write(os, cam_offset_.z());
  vsl_b_write(os, gsd_);
  vsl_b_write(os, t_.sec);
}

//: binary load self from stream
void brad_image_metadata::b_read(vsl_b_istream& is)
{
  if (!is) return;
  short ver;
  vsl_b_read(is, ver);
  if (ver == 0 || ver == 1 || ver == 2 || ver == 3) {
    vsl_b_read(is, sun_elevation_);
    vsl_b_read(is, sun_azimuth_);
    vsl_b_read(is, view_elevation_);
    vsl_b_read(is, view_azimuth_);
    vsl_b_read(is, gain_);
    vsl_b_read(is, offset_);
    vsl_b_read(is, sun_irradiance_);
    vsl_b_read(is, t_.day);
    vsl_b_read(is, t_.hour);
    vsl_b_read(is, t_.min);
    vsl_b_read(is, t_.month);
    vsl_b_read(is, t_.year);
    vsl_b_read(is, number_of_bits_);
    vsl_b_read(is, satellite_name_);
    vsl_b_read(is, cloud_coverage_percentage_);
    double x,y,z;
    vsl_b_read(is, x);
    vsl_b_read(is, y);
    vsl_b_read(is, z);
    upper_right_.set(x,y,z);
    vsl_b_read(is, x);
    vsl_b_read(is, y);
    vsl_b_read(is, z);
    lower_left_.set(x,y,z);
    vsl_b_read(is, band_);
    vsl_b_read(is, n_bands_);
  }
  if (ver == 1 || ver == 2 || ver == 3) {
    double x,y,z;
    vsl_b_read(is, x);
    vsl_b_read(is, y);
    vsl_b_read(is, z);
    cam_offset_.set(x,y,z);
  } if (ver == 2 || ver == 3) {
    vsl_b_read(is, gsd_);
  } if (ver == 3) {
    vsl_b_read(is, t_.sec);
  }
  else {
    vcl_cout << "brad_image_metadata -- unknown binary io version " << ver << '\n';
    return;
  }
}
