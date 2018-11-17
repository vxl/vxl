#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include "brad_image_metadata.h"
//:
// \file

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_awk.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <cassert>

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
#include <vgl/vgl_area.h>
#include <vgl/vgl_intersection.h>
#include <vgl/io/vgl_io_polygon.h>

#include <brad/brad_sun_pos.h>

#ifdef BRAD_WHERE_BRL_LIB_DIR_H_EXISTS
  #include <brad_where_brl_lib_dir.h>
  const std::string brad_image_metadata::calibration_dir = std::string(BRL_LIB_DIR);
#else
  const std::string brad_image_metadata::calibration_dir = std::string();
#endif

const std::string brad_image_metadata::gain_offset_file_name = std::string("brad_sat_img_calibration_table.txt");
const std::string brad_image_metadata::sun_irradiance_file_name = std::string("brad_sat_img_sun_irradiance_table.txt");

// Construct using nitf and metadata file
brad_image_metadata::brad_image_metadata(std::string const& nitf_filename, std::string const& meta_folder)
{
  std::string ext = vul_file::extension(nitf_filename);
  if (ext.compare(".NTF") == 0 || ext.compare(".ntf") == 0) {
    if (!parse(nitf_filename, meta_folder)) {
      std::cerr << "ERROR parsing image metadata\n";
      gsd_ = -1;
    }
  }
  else {
    if (!parse_from_meta_file(nitf_filename)) {
      std::cerr << "ERROR parsing image metadata from metadata file\n";
      gsd_ = -1;
    }
  }
  if(verbose_){
    std::cout << "!!!! lower left lon: " << lower_left_.x() << " lat: " << lower_left_.y() << '\n';
    std::cout << "!!!! upper right lon: " << upper_right_.x() << " lat: " << upper_right_.y() << '\n';
  }
}

// Write brad_image_metadata to stream
std::ostream&  operator<<(std::ostream& s, brad_image_metadata const& md)
{
   s << "sun_elevation = " << md.sun_elevation_ << '\n'
     << "sun_azimuth = " << md.sun_azimuth_ << '\n'
     << "sun_irradiance = " << md.sun_irradiance_ << '\n'
     << "view_elevation = " << md.view_elevation_ << '\n'
     << "view_azimuth = " << md.view_azimuth_ << '\n'
     << "satellite_name = " << md.satellite_name_ << '\n'
     << "band_type = " << md.band_ << '\n'
     << "band_number = " << md.n_bands_ << '\n'
     << "abscal = ";
   for (double i : md.abscal_) {
     s << i << ", ";
   }
   s << '\n';
   s << "effect_band_width = ";
   for (double i : md.effect_band_width_) {
     s << i << ", ";
   }
   s << '\n';
   s << "number_of_bits = " << md.number_of_bits_ << std::endl;
   return s;
}

// Read brad_image_metadata from stream
std::istream&  operator>>(std::istream& s, brad_image_metadata& md)
{
   std::string input;
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
      if (input=="sun_irradiance") {
         s >> input;
         s >> md.sun_irradiance_;
      }
      if (input=="view_elevation") {
         s >> input;
         s >> md.view_elevation_;
      }
      if (input=="satellite_name") {
         s >> input;
         s >> md.satellite_name_;
      }
      if (input=="band_type") {
         s >> input;
         s >> md.band_;
      }
      if (input=="band_number") {
        s >> input;
        s >> md.n_bands_;
      }
      if (input=="view_azimuth") {
         s >> input;
         s >> md.view_azimuth_;
      }
      if (input == "number_of_bits") {
         s >> input;
         s >> md.number_of_bits_;
      }
      if (input == "abscal") {
        double tmp_val;
        for (unsigned i = 0; i < md.n_bands_; i++) {
          s >> input;
          s >> tmp_val;
          md.abscal_.push_back(tmp_val);
        }
      }
      if (input == "effect_band_width") {
        double tmp_val;
        for (unsigned i = 0; i < md.n_bands_; i++) {
          s >> input;
          s >> tmp_val;
          md.effect_band_width_.push_back(tmp_val);
        }
      }
   }
   return s;
}

// Read band dependent calibration adjustment factors from file
// if satellite name not in the default, it will set default gain/offset, i.e., gain = 1.0, offset = 0.0
bool brad_image_metadata::read_band_dependent_gain_offset()
{
  std::string file_path = this->calibration_dir + std::string("/") + this->gain_offset_file_name;
  std::ifstream ifs(file_path.c_str());
  if (!ifs.good()) {
    std::cerr << "Error opening band dependent calibration factor file: " << file_path << std::endl;
    return false;
  }
  // read the file, starting by ignore header comments
  std::string header;
  for (unsigned i = 0; i < 5; i++)
    std::getline(ifs, header);
  unsigned num_of_satllite;
  std::string tmp;
  ifs >> tmp >> num_of_satllite;
  std::map<std::string, std::map<std::string, std::vector<double> > > gains_map;
  std::map<std::string, std::map<std::string, std::vector<double> > > offset_map;
  std::map<std::string, std::vector<double> > default_gain;
  std::map<std::string, std::vector<double> > default_offset;
  std::vector<double> d_pan_gain(1, 1.0);
  std::vector<double> d_mtl_gain(8, 1.0);
  std::vector<double> d_swir_gain(8, 1.0);
  std::vector<double> d_pan_offset(1, 0.0);
  std::vector<double> d_mtl_offset(8, 0.0);
  std::vector<double> d_swir_offset(8, 0.0);
  default_gain["PAN"] = d_pan_gain;
  default_gain["MULTI"] = d_mtl_gain;
  default_gain["SWIR"] = d_swir_gain;
  default_offset["PAN"] = d_pan_offset;
  default_offset["MULTI"] = d_mtl_offset;
  default_offset["SWIR"] = d_swir_offset;
  std::vector<std::string> sat_names;
  std::vector<std::string> band_types;
  band_types.emplace_back("PAN");
  band_types.emplace_back("MULTI");
  band_types.emplace_back("SWIR");
  for (unsigned i = 0; i < num_of_satllite; i++) {
    std::string sat_name, band_name;
    ifs >> sat_name;
    sat_names.push_back(sat_name);
    double pan_gain, pan_offset;
    // get PAN band gain / offset
    ifs >> band_name >> pan_gain >> pan_offset;
    std::vector<double> pan_gain_vec;
    pan_gain_vec.push_back(pan_gain);
    std::vector<double> pan_offset_vec;
    pan_offset_vec.push_back(pan_offset);
    // get MULTI band gain / offset
    std::vector<double> mtl_gain_vec;
    std::vector<double> mtl_offset_vec;
    for (unsigned bid = 0; bid < 8; bid++) {
      double mtl_gain, mtl_offset;
      ifs >> band_name >> mtl_gain >> mtl_offset;
      mtl_gain_vec.push_back(mtl_gain);
      mtl_offset_vec.push_back(mtl_offset);
    }
    // get SWIR band gain / offset
    std::vector<double> swir_gain_vec;
    std::vector<double> swir_offset_vec;
    for (unsigned bid = 0; bid < 8; bid++) {
      double swir_gain, swir_offset;
      ifs >> band_name >> swir_gain >> swir_offset;
      swir_gain_vec.push_back(swir_gain);
      swir_offset_vec.push_back(swir_offset);
    }
    // put into class
    gains_map[sat_name]["PAN"] = pan_gain_vec;
    gains_map[sat_name]["MULTI"] = mtl_gain_vec;
    gains_map[sat_name]["SWIR"] = swir_gain_vec;

    offset_map[sat_name]["PAN"] = pan_offset_vec;
    offset_map[sat_name]["MULTI"] = mtl_offset_vec;
    offset_map[sat_name]["SWIR"] = swir_offset_vec;
  }
  // check band type, only PAN, SWIR, MULTI is supported
  if (std::find(band_types.begin(), band_types.end(), this->band_) == band_types.end()){
    std::cerr << "Error unsupported band type: " << this->band_
              << ", only PAN/MULTI/SWIR are supported when reading band calibration factor from file !!!\n";
    return false;
  }
  if (std::find(sat_names.begin(), sat_names.end(), this->satellite_name_) == sat_names.end()) {
    for (unsigned i = 0; i < this->n_bands_; i++) {
      this->gains_.push_back(default_gain[this->band_][i]);
    }
  }
  else {
    for (unsigned i = 0; i < this->n_bands_; i++) {
      this->gains_.push_back(gains_map[this->satellite_name_][this->band_][i]);
    }
  }

  if (std::find(sat_names.begin(), sat_names.end(), this->satellite_name_) == sat_names.end()) {
    for (unsigned i = 0; i < this->n_bands_; i++) {
      this->offsets_.push_back(default_offset[this->band_][i]);
    }
  }
  else {
    for (unsigned i = 0; i < this->n_bands_; i++) {
      this->offsets_.push_back(offset_map[this->satellite_name_][this->band_][i]);
    }
  }
  return true;
}

// Read band dependent solar irradiance from file
bool brad_image_metadata::read_band_dependent_solar_irradiance()
{
  std::string file_path = this->calibration_dir + std::string("/") + this->sun_irradiance_file_name;
  std::ifstream ifs(file_path.c_str());
  if (!ifs.good()) {
    std::cerr << "Error opening band dependent solar exoatmospheric irradiance file: " << file_path << std::endl;
    return false;
  }
  // read the file, starting by ignore header comments
  std::string header;
  for (unsigned i = 0; i < 6; i++)
    std::getline(ifs, header);
  unsigned num_of_satellite;
  std::string tmp;
  ifs >> tmp >> num_of_satellite;
  std::map<std::string, std::map<std::string, std::vector<double> > > sun_irrad_map;
  std::vector<std::string> sat_names;
  std::vector<std::string> band_types;
  band_types.emplace_back("PAN");
  band_types.emplace_back("MULTI");
  band_types.emplace_back("SWIR");
  for (unsigned i = 0; i < num_of_satellite; i++)
  {
    std::string sat_name, band_name;
    ifs >> sat_name;
    sat_names.push_back(sat_name);
    double solar_irrad;
    // get PAN band solar irrad
    ifs >> band_name >> solar_irrad;
    std::vector<double> pan_solar_vec;
    pan_solar_vec.push_back(solar_irrad);
    // get MULTI band solar irrad
    std::vector<double> mtl_solar_vec;
    for (unsigned bid = 0; bid < 8; bid++) {
      double mtl_solar;
      ifs >> band_name >> mtl_solar;
      mtl_solar_vec.push_back(mtl_solar);
    }
    // get SWIR band solar irrad
    std::vector<double> swir_solar_vec;
    for (unsigned bid = 0; bid < 8; bid++) {
      double swir_solar;
      ifs >> band_name >> swir_solar;
      swir_solar_vec.push_back(swir_solar);
    }
    sun_irrad_map[sat_name]["PAN"]   = pan_solar_vec;
    sun_irrad_map[sat_name]["MULTI"] = mtl_solar_vec;
    sun_irrad_map[sat_name]["SWIR"]  = swir_solar_vec;
  }
  // check band type, only PAN, SWIR, MULTI is supported
  if (std::find(band_types.begin(), band_types.end(), this->band_) == band_types.end()){
    std::cerr << "Error unsupported band type: " << this->band_
              << ", only PAN/MULTI/SWIR are supported when reading sun irradiance parameters from file !!!\n";
    return false;
  }
  if (std::find(sat_names.begin(), sat_names.end(), this->satellite_name_) == sat_names.end())
    return false;
  for (unsigned i = 0; i < this->n_bands_; i++) {
    this->normal_sun_irradiance_values_.push_back(sun_irrad_map[this->satellite_name_][this->band_][i]);
  }
  return true;
}

bool brad_image_metadata::parse_from_imd(std::string const& filename)
{
  std::ifstream ifs( filename.c_str() );
  if (!ifs.good()){
    std::cerr << "Error opening file " << filename << std::endl;
    return false;
  }

  n_bands_ = 0;
  // now parse the IMD file
  vul_awk awk(ifs);
  double absCalfact = 1.0;
  double effectiveBand = 1.0;
  lower_left_.set(181, 91, 10000);
  upper_right_.set(-181,-91, -10000);
  // ugh, introducting a sequential dependency...need all points in order to add to polygon
  // parse_from_pvl() does this a little cleaner
  std::vector<vgl_point_2d<double> > footprint_corners(4);
  for (; awk; ++awk)
  {
    std::stringstream linestr(awk.line());
    std::string tag;
    linestr >> tag;

    if (tag.compare("absCalFactor") == 0) {
      linestr >> tag;  // read =
      linestr >> absCalfact;
      this->abscal_.push_back(absCalfact);
      continue;
    }
    if (tag.compare("effectiveBandwidth") == 0) {
      linestr >> tag;  // read =
      linestr >> effectiveBand;
      this->effect_band_width_.push_back(effectiveBand);
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
      if (tag.find("Basic") == std::string::npos && tag.find("Stereo") == std::string::npos)
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
      satellite_name_ = satellite_name_.substr(satellite_name_.find_first_of('\"')+1, satellite_name_.find_last_of('\"')-1);
      continue;
    }
    if (tag.compare("LLLon") == 0 || tag.compare("URLon") == 0 || tag.compare("ULLon") == 0 || tag.compare("LRLon") == 0) {
      int corner_pos = 0;
      if(tag.compare("ULLon") == 0) {
        corner_pos = 0;
      } else if(tag.compare("URLon") == 0) {
        corner_pos = 1;
      } else if (tag.compare("LRLon") == 0) {
        corner_pos = 2;
      } else if (tag.compare("LLLon") == 0) {
        corner_pos = 3;
      } else {
        assert(!"Could not place the point in the polygon");
      }

      linestr >> tag;
      double x;
      linestr >> x;
      if (lower_left_.x() > x) lower_left_.set(x, lower_left_.y(), lower_left_.z());
      if (upper_right_.x() < x) upper_right_.set(x, upper_right_.y(), upper_right_.z());

      footprint_corners[corner_pos].x() = x;

      continue;
    }
    if (tag.compare("LLLat") == 0 || tag.compare("URLat") == 0 || tag.compare("ULLat") == 0 || tag.compare("LRLat") == 0) {
      int corner_pos = 0;
      if(tag.compare("ULLat") == 0) {
        corner_pos = 0;
      } else if(tag.compare("URLat") == 0) {
        corner_pos = 1;
      } else if (tag.compare("LRLat") == 0) {
        corner_pos = 2;
      } else if (tag.compare("LLLat") == 0) {
        corner_pos = 3;
      } else {
        assert(!"Could not place the point in the polygon");
      }

      linestr >> tag;
      double y;
      linestr >> y;
      if (lower_left_.y() > y) lower_left_.set(lower_left_.x(), y, lower_left_.z());
      if (upper_right_.y() < y) upper_right_.set(upper_right_.x(), y, upper_right_.z());

      footprint_corners[corner_pos].y() = y;

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
      std::string band_str;
      linestr >> band_str;
      if (band_str.find('P') != std::string::npos) {
        band_ = "PAN";
      } else if (band_str.find('S') != std::string::npos) {
        band_ = "SWIR";
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
  if(verbose_)
    std::cout << "  cloud coverage percentage : " << cloud_coverage_percentage_ << " band: " << band_ << " number of bands: " << n_bands_ << std::endl;

  footprint_ = vgl_polygon<double>(footprint_corners);

  return true;
}

// parse all metadata information from the imd file only, without using nitf image header
// Note that imd file doesn't contain 1. camera offset value; 2. number of effective bits
bool brad_image_metadata::parse_from_imd_only(std::string const& filename)
{
  std::ifstream ifs( filename.c_str() );
  if (!ifs.good()){
    std::cerr << "Error opening file " << filename << std::endl;
    return false;
  }

  n_bands_ = 0;
  cam_offset_.set(0,0,0);
  number_of_bits_ = 11;
  double absCalfact = 1.0;
  double effectiveBand = 1.0;
  lower_left_.set(181, 91, 10000);
  upper_right_.set(-181,-91, -10000);
  std::vector<vgl_point_2d<double> > footprint_corners(4);
  // now parse the IMD file
  vul_awk awk(ifs);
  for (; awk; ++awk)
  {
    std::stringstream linestr(awk.line());
    std::string tag;
    linestr >> tag;
    // absolute CalFactor and effective bandwidth (used to calculate the gains
    if (tag.compare("absCalFactor") == 0) {
      linestr >> tag;  // read =
      linestr >> absCalfact;
      this->abscal_.push_back(absCalfact);
      continue;
    }
    if (tag.compare("effectiveBandwidth") == 0) {
      linestr >> tag;
      linestr >> effectiveBand;
      this->effect_band_width_.push_back(effectiveBand);
      continue;
    }
    // cloud coverage
    if (tag.compare("cloudCover") == 0) {
      linestr >> tag;
      linestr >> cloud_coverage_percentage_;
      continue;
    }
    // check the product type to ensure it is Basic
    if (tag.compare("productType") == 0) {
      linestr >> tag;
      linestr >> tag;
      if (tag.find("Basic") == std::string::npos && tag.find("Stereo") == std::string::npos)
        return false;
    }
    // GSD
    if (tag.compare("meanProductGSD") == 0 || tag.compare("meanCollectedGSD") == 0) {
      linestr >> tag;
      linestr >> gsd_;
      continue;
    }
    // Satellite name
    if (tag.compare("satId") == 0) {
      linestr >> tag;
      linestr >> satellite_name_;
      satellite_name_ = satellite_name_.substr(satellite_name_.find_first_of('\"')+1, satellite_name_.find_last_of('\"')-1);
      continue;
    }
    // image footprint and cover extent
    if (tag.compare("LLLon") == 0 || tag.compare("URLon") == 0 || tag.compare("ULLon") == 0 || tag.compare("LRLon") == 0)
    {
      int corner_pos = 0;
      if(tag.compare("ULLon") == 0) {
        corner_pos = 0;
      } else if(tag.compare("URLon") == 0) {
        corner_pos = 1;
      } else if (tag.compare("LRLon") == 0) {
        corner_pos = 2;
      } else if (tag.compare("LLLon") == 0) {
        corner_pos = 3;
      } else {
        assert(!"Could not place the point in the polygon");
      }
      linestr >> tag;
      double x;
      linestr >> x;
      if (lower_left_.x() > x) lower_left_.set(x, lower_left_.y(), lower_left_.z());
      if (upper_right_.x() < x) upper_right_.set(x, upper_right_.y(), upper_right_.z());
      footprint_corners[corner_pos].x() = x;
      continue;
    }
    if (tag.compare("LLLat") == 0 || tag.compare("URLat") == 0 || tag.compare("ULLat") == 0 || tag.compare("LRLat") == 0)
    {
      int corner_pos = 0;
      if(tag.compare("ULLat") == 0) {
        corner_pos = 0;
      } else if(tag.compare("URLat") == 0) {
        corner_pos = 1;
      } else if (tag.compare("LRLat") == 0) {
        corner_pos = 2;
      } else if (tag.compare("LLLat") == 0) {
        corner_pos = 3;
      } else {
        assert(!"Could not place the point in the polygon");
      }
      linestr >> tag;
      double y;
      linestr >> y;
      if (lower_left_.y() > y) lower_left_.set(lower_left_.x(), y, lower_left_.z());
      if (upper_right_.y() < y) upper_right_.set(upper_right_.x(), y, upper_right_.z());
      footprint_corners[corner_pos].y() = y;
      continue;
    }
    if (tag.compare("LLHAE") == 0 || tag.compare("URHAE") == 0 || tag.compare("ULHAE") == 0 || tag.compare("LRHAE") == 0)
    {  //CAUTION: height above ELLIPSOID (not mean sea level/ geoid)
      linestr >> tag;
      double z;
      linestr >> z;
      if (lower_left_.z() > z) lower_left_.set(lower_left_.x(), lower_left_.y(), z);
      if (upper_right_.z() < z) upper_right_.set(upper_right_.x(), upper_right_.y(), z);
      continue;
    }
    // band type
    if (tag.compare("bandId") == 0) {
      linestr >> tag;
      std::string band_str;
      linestr >> band_str;
      if (band_str.find('P') != std::string::npos) {
        band_ = "PAN";
      } else if (band_str.find('S') != std::string::npos) {
        band_ = "SWIR";
      } else
        band_ = "MULTI";
      continue;
    }
    // count the band number
    if (tag.compare("BEGIN_GROUP") == 0) {
      n_bands_++;
      continue;
    }
    // Sun elevation angle
    if (tag.compare("meanSunEl") == 0) {
      linestr >> tag;
      linestr >> sun_elevation_;
      continue;
    }
    // Sun azimuth angle
    if (tag.compare("meanSunAz") == 0) {
      linestr >> tag;
      linestr >> sun_azimuth_;
      continue;
    }
    // View elevation angle
    if (tag.compare("meanSatEl") == 0) {
      linestr >> tag;
      linestr >> view_elevation_;
      continue;
    }
    // View azimuth angle
    if (tag.compare("meanSatAz") == 0) {
      linestr >> tag;
      linestr >> view_azimuth_;
      continue;
    }
    // Time
    if (tag.compare("firstLineTime") == 0) {
      std::string time_str;
      linestr >> tag;
      linestr >> time_str;
      std::string s_year, s_month, s_date, s_hour, s_minute, s_second;
      s_year = time_str.substr(0,4);    s_month = time_str.substr(5,2);     s_date = time_str.substr(8,2);
      s_hour = time_str.substr(11,2);  s_minute = time_str.substr(14,2);  s_second = time_str.substr(17,2);
      t_.year  = std::atoi(s_year.c_str());
      t_.month = std::atoi(s_month.c_str());
      t_.day   = std::atoi(s_date.c_str());
      t_.hour  = std::atoi(s_hour.c_str());
      t_.min   = std::atoi(s_minute.c_str());
      t_.sec   = std::atoi(s_second.c_str());
      continue;
    }
  }
  n_bands_--; // there is an extra BEGIN_GROUP for some other image info not related to individual bands
  if(verbose_)
    std::cout << "  cloud coverage percentage : " << cloud_coverage_percentage_ << " band: " << band_ << " number of bands: " << n_bands_ << std::endl;

  footprint_ = vgl_polygon<double>(footprint_corners);
  return true;
}

// parse all metadata information from the pvl file only, without using nitf image header
bool brad_image_metadata::parse_from_pvl_only(std::string const& filename)
{
  std::ifstream ifs( filename.c_str() );
  if (!ifs.good()){
    std::cerr << "Error opening file " << filename << std::endl;
    return false;
  }
  n_bands_ = 0;
  double cam_xoff, cam_yoff, cam_zoff;
  lower_left_.set(181, 91, 10000);
  upper_right_.set(-181,-91, -10000);
  std::vector<vgl_point_2d<double> > footprint_corners(4);
  std::vector<double> pvl_abscal;

  // start parsing
  vul_awk awk(ifs);
  for (; awk; ++awk)
  {
    std::stringstream linestr(awk.line());
    std::string tag;
    linestr >> tag;
    // cloud cover percentage
    if (tag.compare("productCloudCoverPercentage") == 0) {
      linestr >> tag;
      linestr >> cloud_coverage_percentage_;
      continue;
    }
    // image footprint
    if ((linestr.str().find("BEGIN_GROUP") != std::string::npos && linestr.str().find("upperRightCorner") != std::string::npos) ||
        (linestr.str().find("BEGIN_GROUP") != std::string::npos && linestr.str().find("upperLeftCorner") != std::string::npos) ||
        (linestr.str().find("BEGIN_GROUP") != std::string::npos && linestr.str().find("lowerRightCorner") != std::string::npos) ||
        (linestr.str().find("BEGIN_GROUP") != std::string::npos && linestr.str().find("lowerLeftCorner") != std::string::npos) )
    {
      int corner_pos = 0;
      // ugh
      if (linestr.str().find("upperLeftCorner") != std::string::npos) {
        corner_pos = 0;
      } else if (linestr.str().find("upperRightCorner") != std::string::npos) {
        corner_pos = 1;
      } else if (linestr.str().find("lowerRightCorner") != std::string::npos) {
        corner_pos = 2;
      } else if (linestr.str().find("lowerLeftCorner") != std::string::npos) {
        corner_pos = 3;
      } else {
        assert(!"Could not place the point in the polygon");
      }

      std::stringstream linestr(awk.line());
      while (linestr.str().find("latitude") == std::string::npos) {
         ++awk;
         //linestr = std::stringstream(awk.line());
         linestr.clear();
         linestr.str(awk.line());
      }
      std::string dummy; linestr >> dummy; linestr >> dummy;
      double y; linestr >> y;
      while (linestr.str().find("longitude") == std::string::npos) {
         ++awk;
         //linestr = std::stringstream(awk.line());
         linestr.clear();
         linestr.str(awk.line());
      }
      linestr >> dummy; linestr >> dummy;
      double x; linestr >> x;
      while (linestr.str().find("height") == std::string::npos) {
         ++awk;
         //linestr = std::stringstream(awk.line());
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

      footprint_corners[corner_pos] = vgl_point_2d<double>(x,y);
      continue;
    }
    // band type
    if (tag.compare("productSpectralType") == 0) {
      linestr >> tag;
      std::string band_str;
      linestr >> band_str;
      if (band_str.find("PAN") != std::string::npos)
        band_ = "PAN";
      else
        band_ = "MULTI";
      continue;
    }
    // GSD
    if (tag.compare("pixelSpacing") == 0) {
      linestr >> tag;
      linestr >> gsd_;
      continue;
    }
    // number of bands
    if (tag.compare("numberOfSpectralBands") == 0) {
      linestr >> tag;
      linestr >> n_bands_;
      continue;
    }
    // satellite name, could be GeoEye, WV, QB, WV02
    if (tag.compare("satelliteName") == 0) {
      linestr >> tag;
      linestr >> satellite_name_;
      satellite_name_ = satellite_name_.substr(satellite_name_.find_first_of('\"')+1, satellite_name_.find_last_of('\"')-1);
      continue;
    }
    // gain and offset
    if (linestr.str().find("BEGIN_GROUP") != std::string::npos && linestr.str().find("radiometry") != std::string::npos) {
      ++awk;
      linestr.clear();
      linestr.str(awk.line());

      std::string dummy; linestr >> dummy; linestr >> dummy;
      double g; linestr >> g;

       ++awk;
      linestr.clear();
      linestr.str(awk.line());
      linestr >> dummy; linestr >> dummy;
      double off; linestr >> off;
      pvl_abscal.push_back(g);
      continue;
    }
    // sun elevation angle
    if (tag.compare("firstLineSunElevationAngle") == 0) {
      linestr >> tag;
      linestr >> sun_elevation_;
      continue;
    }
    // sun azimuth angle
    if (tag.compare("firstLineSunAzimuthAngle") == 0) {
      linestr >> tag;
      linestr >> sun_azimuth_;
      continue;
    }
    // view elevation angle
    if (tag.compare("firstLineElevationAngle") == 0) {
      linestr >> tag;
      linestr >> view_elevation_;
      continue;
    }
    // view azimuth angle
    if (tag.compare("firstLineAzimuthAngle") == 0) {
      linestr >> tag;
      linestr >> view_azimuth_;
      continue;
    }
    // camera offset
    if (tag.compare("longitudeOffset") == 0) {
      linestr >> tag;
      linestr >> cam_xoff;
      continue;
    }
    if (tag.compare("latitudeOffset") == 0) {
      linestr >> tag;
      linestr >> cam_yoff;
      continue;
    }
    if (tag.compare("heightOffset") == 0) {
      linestr >> tag;
      linestr >> cam_zoff;
      continue;
    }
    // number of effective bits
    if (tag.compare("bitsPerPixel") == 0) {
      linestr >> tag;
      linestr >> number_of_bits_;
      continue;
    }
    // image time
    if (tag.compare("firstLineAcquisitionDateTime") == 0) {
      std::string time_str;
      linestr >> tag;
      linestr >> time_str;
      std::string s_year, s_month, s_date, s_hour, s_minute, s_second;
      s_year = time_str.substr(0,4);    s_month = time_str.substr(5,2);     s_date = time_str.substr(8,2);
      s_hour = time_str.substr(11,2);  s_minute = time_str.substr(14,2);  s_second = time_str.substr(17,2);
      t_.year  = std::atoi(s_year.c_str());
      t_.month = std::atoi(s_month.c_str());
      t_.day   = std::atoi(s_date.c_str());
      t_.hour  = std::atoi(s_hour.c_str());
      t_.min   = std::atoi(s_minute.c_str());
      t_.sec   = std::atoi(s_second.c_str());
    }
  }
  // set the camera offset
  footprint_ = vgl_polygon<double>(footprint_corners);
  cam_offset_.set(cam_xoff, cam_yoff, cam_zoff);
  // set the abscal and effect_band_width from loaded gain/offset
  // if image_type is PAN (where band number is 1), first value is for PAN
  // note that pvl files also specify PAN gain even if it is a multi image (band 1 is PAN, 2-4 are multi)
  if (this->n_bands_ == 1) {
    this->abscal_.push_back(pvl_abscal[0]);
    this->effect_band_width_.push_back(1.0);
  }
  else {
    for (unsigned i = 1; i < pvl_abscal.size(); i++) // ignore the first value (for PAN band)
    {
      this->abscal_.push_back(pvl_abscal[i]);
      this->effect_band_width_.push_back(1.0);
    }
  }

  return true;
}

// only parse the cloud coverage for now
bool brad_image_metadata::parse_from_pvl(std::string const& filename)
{
  //std::cout << "Parse from PVL file is not implemented yet!\n";
  std::ifstream ifs( filename.c_str() );
  if (!ifs.good()){
    std::cerr << "Error opening file " << filename << std::endl;
    return false;
  }
  n_bands_ = 0;
  lower_left_.set(181, 91, 10000);
  upper_right_.set(-181,-91, -10000);

  std::vector<vgl_point_2d<double> > footprint_corners(4);
  std::vector<double> pvl_abscal;
  // now parse the IMD file
  vul_awk awk(ifs);
  for (; awk; ++awk)
  {
    std::stringstream linestr(awk.line());
    std::string tag;
    linestr >> tag;

    if (tag.compare("productCloudCoverPercentage") == 0) {
      linestr >> tag;
      linestr >> cloud_coverage_percentage_;
      continue;
    }
    if ((linestr.str().find("BEGIN_GROUP") != std::string::npos && linestr.str().find("upperRightCorner") != std::string::npos) ||
        (linestr.str().find("BEGIN_GROUP") != std::string::npos && linestr.str().find("upperLeftCorner") != std::string::npos) ||
        (linestr.str().find("BEGIN_GROUP") != std::string::npos && linestr.str().find("lowerRightCorner") != std::string::npos) ||
        (linestr.str().find("BEGIN_GROUP") != std::string::npos && linestr.str().find("lowerLeftCorner") != std::string::npos) )
    {
      int corner_pos = 0;
      // ugh
      if (linestr.str().find("upperLeftCorner") != std::string::npos) {
        corner_pos = 0;
      } else if (linestr.str().find("upperRightCorner") != std::string::npos) {
        corner_pos = 1;
      } else if (linestr.str().find("lowerRightCorner") != std::string::npos) {
        corner_pos = 2;
      } else if (linestr.str().find("lowerLeftCorner") != std::string::npos) {
        corner_pos = 3;
      } else {
        assert(!"Could not place the point in the polygon");
      }

      std::stringstream linestr(awk.line());
      while (linestr.str().find("latitude") == std::string::npos) {
         ++awk;
         //linestr = std::stringstream(awk.line());
         linestr.clear();
         linestr.str(awk.line());
      }
      std::string dummy; linestr >> dummy; linestr >> dummy;
      double y; linestr >> y;
      while (linestr.str().find("longitude") == std::string::npos) {
         ++awk;
         //linestr = std::stringstream(awk.line());
         linestr.clear();
         linestr.str(awk.line());
      }
      linestr >> dummy; linestr >> dummy;
      double x; linestr >> x;
      while (linestr.str().find("height") == std::string::npos) {
         ++awk;
         //linestr = std::stringstream(awk.line());
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

      footprint_corners[corner_pos] = vgl_point_2d<double>(x,y);

      continue;
    }
    if (tag.compare("productSpectralType") == 0) {
      linestr >> tag;
      std::string band_str;
      linestr >> band_str;
      if (band_str.find("PAN") != std::string::npos)
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
    if (linestr.str().find("BEGIN_GROUP") != std::string::npos && linestr.str().find("radiometry") != std::string::npos) {
      ++awk;
      linestr.clear();
      linestr.str(awk.line());

      std::string dummy; linestr >> dummy; linestr >> dummy;
      double g; linestr >> g;

       ++awk;
      linestr.clear();
      linestr.str(awk.line());
      linestr >> dummy; linestr >> dummy;
      double off; linestr >> off;
      pvl_abscal.push_back(g);
    }
  }

  footprint_ = vgl_polygon<double>(footprint_corners);
  // set the abscal and effect_band_width from loaded gain/offset
  // if image_type is PAN (where band number is 1), first value is for PAN
  // note that pvl files also specify PAN gain even if it is a multi image (band 1 is PAN, 2-4 are multi)
  if (this->n_bands_ == 1) {
    this->abscal_.push_back(pvl_abscal[0]);
    this->effect_band_width_.push_back(1.0);
  }
  else {
    for (unsigned i = 1; i < pvl_abscal.size(); i++) // ignore the first value (for PAN band)
    {
      this->abscal_.push_back(pvl_abscal[i]);
      this->effect_band_width_.push_back(1.0);
    }
  }
  if(verbose_)
    std::cout << "cloud coverage percentage : " << cloud_coverage_percentage_ << " band: " << band_ << " number of bands: " << n_bands_ << std::endl;
  return true;
}

//: Parse the required params for normalization and other tasks from a text file with a known format,
//  it can be used to calibrate images from any satellite if such files are created for each image of the satellite
//  example file:
// satelliteName GeoEye-1
// productSpectralType MULTI
// absCalFactor 0.864
// effectiveBandwidth 0.896
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
// TODO extend with LRLat,LRLon, LRHAE, ULLat, ULLon, ULHAE
bool brad_image_metadata::parse_from_txt(std::string const& filename)
{
  if(verbose_)
    std::cout << "parsing radiometric calibration and atmospheric normalization parameters from: " << filename << "...\n";
  std::ifstream ifs( filename.c_str() );
  if (!ifs.good()){
    std::cerr << "Error opening file " << filename << std::endl;
    return false;
  }
  n_bands_ = 0;
  lower_left_.set(181, 91, 10000);
  upper_right_.set(-181,-91, -10000);
  double val;

  bool parsed_gain_offset = false, parsed_sun_irradiance = false;
  bool parsed_coverage_percentage_ = false;
  std::vector<double> gain_vec;
  std::vector<double> offset_vec;
  std::vector<double> abscal;
  std::vector<double> effect_band;
  std::vector<double> solar_irrads;
  vul_awk awk(ifs);
  for (; awk; ++awk)
  {
    std::stringstream linestr(awk.line());
    std::string tag;
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
    if (tag.compare("absCalFactor") == 0) {
      if (!this->n_bands_) {
        std::cerr << "n_bands tag should precded absCalFactor tag!  Problem parsing text metadata: " << filename << "!!\n";
        return false;
      }
      abscal.clear();
      for (unsigned i = 0; i < this->n_bands_; i++) {
        linestr >> val;
        abscal.push_back(val);
      }
      continue;
    }
    if (tag.compare("effectiveBandwidth") == 0 ) {
      if (!this->n_bands_) {
        std::cerr << "n_bands tag should precded absCalFactor tag!  Problem parsing text metadata: " << filename << "!!\n";
        return false;
      }
      effect_band.clear();
      for (unsigned i = 0; i < this->n_bands_; i++) {
        linestr >> val;
        effect_band.push_back(val);
      }
      continue;
    }
    if (tag.compare("satelliteName") == 0) {
      linestr >> satellite_name_;
      continue;
    }
    if (tag.compare("productSpectralType") == 0) {
      std::string band_str;
      linestr >> band_str;
      if (band_str.find("PAN") != std::string::npos)
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
        std::cerr << "n_bands tag should precede solar_irradiance tag! Problems parsing: " << filename << "\n";
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
        std::cerr << "n_bands tag should preceed solar_irradiance tag! Problems parsing: " << filename << "\n";
        return false;
      }
      for (unsigned i = 0; i < n_bands_; i++) {
          double gain, offset;
          linestr >> gain; linestr >> offset;
          gain_vec.push_back(gain);
          offset_vec.push_back(offset);
      }
      parsed_gain_offset = true;
      continue;
    }
  }
  this->gains_ = gain_vec;
  this->offsets_ = offset_vec;
  this->abscal_ = abscal;
  this->effect_band_width_ = effect_band;
  this->normal_sun_irradiance_values_ = solar_irrads;

  // check size of absCal, effectBandWidth, gain/offset and solar irradiance to band number
  if (this->gains_.size() != this->n_bands_ || this->offsets_.size() != this->n_bands_) {
    std::cerr << "ERROR: mismatch between calibration factor gain/offset length " << this->gains_.size() << "/"
              << this->offsets_.size()
              << " w.r.t. image band number " << this->n_bands_
              << ". Problems parsing: " << filename
              << "!!\n";
    parsed_gain_offset = false;
    return false;
  }
  if (this->normal_sun_irradiance_values_.size() != this->n_bands_) {
    std::cerr << "ERROR: mismatch between image sun irradiance value length " << this->normal_sun_irradiance_values_.size()
              << " w.r.t. image band number " << this->n_bands_ << ". Problems parsing: " << filename
              << "!!\n";
    parsed_sun_irradiance = false;
    return false;
  }
  if (this->abscal_.size() != this->n_bands_) {
    std::cerr << "ERROR: mismatch between image absCalFactor/EffectBandWidth value length " << this->abscal_.size() << "/"
              << this->effect_band_width_.size()
              << " w.r.t. image band number " <<  this->n_bands_
              << ". Problems parsing: " << filename
              << "!!\n";
    return false;
  }
  if(parsed_coverage_percentage_) {
    if(verbose_)
      std::cout << "cloud coverage percentage : " << cloud_coverage_percentage_ << " band: " << band_ << " number of bands: " << n_bands_ << std::endl;
  }

  return true;
}

//: parse header in nitf image, assumes that metadata files are in the same folder with the image
//  If meta_folder is not empty, they are searched in that folder as well
bool brad_image_metadata::parse(std::string const& nitf_filename, std::string const& meta_folder)
{
  vil_image_resource_sptr image = vil_load_image_resource(nitf_filename.c_str());
  if (!image)
  {
    std::cout << "NITF image load failed!\n";
    return false;
  }

  std::string format = image->file_format();
  std::string prefix = format.substr(0,4);

  if (prefix != "nitf")
  {
    std::cout << "source image is not NITF\n";
    return false;
  }

  //cast to an nitf2_image
  auto *nitf_image = static_cast<vil_nitf2_image*>(image.ptr());

  vpgl_nitf_rational_camera nitf_cam(nitf_image, false);

  //upper_left_ = nitf_cam.upper_left(); // CAUTION: lat is x and lon is y when read from nitf camera with this method
  upper_right_.set(nitf_cam.upper_right()[nitf_cam.LON], nitf_cam.upper_right()[nitf_cam.LAT], 0);
  lower_left_.set(nitf_cam.lower_left()[nitf_cam.LON],   nitf_cam.lower_left()[nitf_cam.LAT], 0);
  //lower_right_ = nitf_cam.lower_right();
  if(verbose_)
  {
    std::cout << "!!!! lower left lon: " << lower_left_.x() << " lat: " << lower_left_.y() << '\n';
    std::cout << "!!!! upper right lon: " << upper_right_.x() << " lat: " << upper_right_.y() << '\n';
  }

  vpgl_nitf_rational_camera::geopt_coord LON = vpgl_nitf_rational_camera::LON;
  vpgl_nitf_rational_camera::geopt_coord LAT = vpgl_nitf_rational_camera::LAT;
  vgl_polygon<double> footprint(1);
  footprint.push_back(nitf_cam.upper_left()[LON], nitf_cam.upper_left()[LAT]);
  footprint.push_back(nitf_cam.upper_right()[LON],nitf_cam.upper_right()[LAT]);
  footprint.push_back(nitf_cam.lower_right()[LON],nitf_cam.lower_right()[LAT]);
  footprint.push_back(nitf_cam.lower_left()[LON], nitf_cam.lower_left()[LAT]);
  footprint_ = footprint;

  double xoff, yoff, zoff;
  xoff = nitf_cam.offset(vpgl_rational_camera<double>::X_INDX);
  yoff = nitf_cam.offset(vpgl_rational_camera<double>::Y_INDX);
  zoff = nitf_cam.offset(vpgl_rational_camera<double>::Z_INDX);
  cam_offset_.set(xoff, yoff, zoff);

  //get NITF information
  std::vector< vil_nitf2_image_subheader* > headers = nitf_image->get_image_headers();
  vil_nitf2_image_subheader* hdr = headers[0];

  double sun_el;
  double sun_az;
  if (!hdr->get_sun_params(sun_el, sun_az)) {
    std::cerr << "failed to obtain sun parameters info\n";
    return false;
  }

  sun_elevation_ = sun_el;
  sun_azimuth_ = sun_az;

  int year, month, day, hour, min, sec;
  if (!hdr->get_date_time(year, month, day, hour,  min, sec)) {
    std::cerr << "failed to obtain date time info\n";
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
  view_elevation_ = std::asin(to_camera.z()) * rad_to_deg;
  // degrees east of north
  view_azimuth_ = std::atan2(to_camera.x(), to_camera.y()) * rad_to_deg;
  if (view_azimuth_ < 0)
    view_azimuth_ += 360;

  std::string dirname = vul_file::dirname(nitf_filename);

  std::string img_info = hdr->get_image_source();
  if(verbose_)
    std::cout << "img_info: " << img_info << std::endl;

  // look for metadata files with known formats recursively in the directory of the image
  // If we find one, check file name to see if it is for the same image, if so parse it
  std::string imagename = vul_file::strip_directory(nitf_filename);
  imagename = vul_file::strip_extension(imagename);
  if(verbose_)
    std::cout << "imagename: " << imagename << std::endl;

  std::string in_dir = dirname + "/*.*";
  std::string meta_filename = "";
  for (vul_file_iterator fn = in_dir.c_str(); fn; ++fn) {
    std::string filename = fn();
    std::string name = vul_file::strip_directory(filename);
    name = vul_file::strip_extension(name);
    std::string ext = vul_file::extension(filename);
    if (imagename.find(name) != std::string::npos &&
        (ext.compare(".IMD") == 0 || ext.compare(".imd") == 0 ||
         ext.compare(".PVL") == 0 || ext.compare(".pvl") == 0)
       ) {
      meta_filename = filename;
      break;
    }
  }
  if (meta_filename.size() == 0 && meta_folder.size() != 0) {
    if(verbose_)
      std::cout << " searching " << meta_folder << " for files with extensions .imd, .pvl or .txt" << std::endl;
    std::string in_dir = meta_folder + "/*.*";
    for (vul_file_iterator fn = in_dir.c_str(); fn; ++fn) {
      std::string filename = fn();
      std::string name = vul_file::strip_directory(filename);
      name = vul_file::strip_extension(name);
      std::string ext = vul_file::extension(filename);
      if (imagename.find(name) != std::string::npos &&
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

  // Parse from metadata file for further improvement
  // Also set band dependent adjustment factors gain and offset, could either obtained from hardcoded table or specified from
  // user prepared text metadata file
  std::vector<double> solar_irrads; // for multi-spectral imagery there are multiple values
  bool parsed_sun_irradiance = false;
  bool parsed_gain_offset = false;
  if (meta_filename.size() == 0)  // no metadata file available; try providing some known value for IKNOOS PAN imagery
  {
    std::string type = hdr->get_image_type();  // type mono is band PAN
    if (img_info.compare("IKNOOS") == 0 && type.compare("MONO") == 0 && this->number_of_bits_ == (unsigned)11) {
      this->n_bands_ = 1;
      this->band_ = "PAN";
      this->satellite_name_ = "IKNOOS";
      this->abscal_.push_back((10.0/161.0) / 0.403);
      this->abscal_.push_back(1.0);
      // read band dependent adjustment factors from hard-coded file
      if (!this->read_band_dependent_gain_offset()) {
        std::cerr << "ERROR: can not read band dependent adjustment factors gain and offset!!!" << std::endl;
        return false;
      }
    }
    else {
      std::cerr << "ERROR: can not any find metadata file (pvl, imd, txt) for image: " << imagename << std::endl;
      return false;
    }
  }
  else  // read values from metadata file and band dependent gain/offset
  {
    std::string ext = vul_file::extension(meta_filename);
    bool parsed_fine = false;
    if (ext.compare(".IMD") == 0 || ext.compare(".imd") == 0)  // parse from IMD file
    {
      parsed_fine = this->parse_from_imd(meta_filename);
    }
    else if (ext.compare(".PVL") == 0 || ext.compare(".pvl") == 0)  // parse from PVL file
    {
      parsed_fine = this->parse_from_pvl(meta_filename);
    }
    else if (ext.compare(".TXT") == 0 || ext.compare(".txt") == 0)  // parse from TXT file
    {
      // note that the band denpendent gain/offset are now read from user prepared txt file
      parsed_fine = this->parse_from_txt(meta_filename);
      if (!parsed_fine) {
        std::cerr << "ERROR parsing metadata text file: " << meta_filename << " failed!!!\n";
        return false;
      }
      parsed_sun_irradiance = true;
      parsed_gain_offset = true;
    }
    else {
      std::cerr << "ERROR unrecognized metadata file format: " << ext << " in satellite name: " << imagename << "!!!\n";
      std::cerr << "  Only pvl, imd or user-prepared txt allowed!!!" << std::endl;
      return false;
    }
    if (!parsed_fine) {
      std::cerr << "ERROR parsing meta data file " << meta_filename << " failed!!!\n";
      return false;
    }
  }

  // regularize satellite name
  if (img_info.find("IKONOS") != std::string::npos || nitf_filename.find("IK") != std::string::npos)
    this->satellite_name_ = "IKONOS";
  else if (img_info.find("GeoEye-1") != std::string::npos || img_info.find("GEOEYE1") != std::string::npos ||
           satellite_name_.find("OV-5") != std::string::npos || satellite_name_.find("GE01") != std::string::npos)
    this->satellite_name_ = "GeoEye-1";
  else if (img_info.find("QuickBird") != std::string::npos ||
           nitf_filename.find("QB") != std::string::npos ||
           nitf_filename.find("QuickBird") != std::string::npos ||
           img_info.find("QB02") != std::string::npos ||
           satellite_name_.find("QB02") != std::string::npos)
    this->satellite_name_ = "QuickBird";
  else if (img_info.find("WorldView") != std::string::npos  || img_info.find("WV01") != std::string::npos ||
           satellite_name_.find("WV01") != std::string::npos)
    this->satellite_name_ = "WorldView";
  else if (img_info.find("WorldView2") != std::string::npos || img_info.find("WV02") != std::string::npos ||
           satellite_name_.find("WV02") != std::string::npos)
    this->satellite_name_ = "WorldView2";
  else if (img_info.find("WorldView3") != std::string::npos || img_info.find("WV03") != std::string::npos ||
           satellite_name_.find("WV03") != std::string::npos)
    this->satellite_name_ = "WorldView3";
  else if (img_info.find("DigitalGlobe") != std::string::npos)
    this->satellite_name_ = "DigitalGlobe";

  // read gain offset from default gain/offset table
  // Default Gain/Offset are from DigitalGlobe tech spec website and should updated annually
  // (http://www.digitalglobe.com/resources/technical-information)
  // this table is stored in source/contrib/brl/bbas/brad/brad_sat_img_calibration_table.txt or
  //                      compile/lib/brad_sat_img_calibration_table.txt
  if (!parsed_gain_offset)
    if (!this->read_band_dependent_gain_offset()) {
      std::cerr << "ERROR: Parse gain offset value from default table failed for satellite: " << this->satellite_name_
                << ", can not find this satellite in default gain/offset table!!!\n";
      return false;
    }
  // screen output radiometric correction parameters
  if (this->verbose_)
    this->print_out_radiometric_parameter();

  // read default solar irradiance
  // same as default gain/offset, values are from annual published DigitalGlobe tech spec
  // (http://www.digitalglobe.com/resources/technical-information)
  if (!parsed_sun_irradiance)
    if (!this->read_band_dependent_solar_irradiance()) {
      std::cerr << "ERROR: Parse solar irradiance value from default table failed for satellite: " << this->satellite_name_
                << ", can not find this satellite in default solar irradiance table!!!\n";
      return false;
    }

  if (this->verbose_) {
    this->print_out_solar_irradiance();
  }

  // check consistancy of gain, offset, sun irradiance w.r.t number of bands
  if (this->gains_.size() != this->n_bands_ || this->offsets_.size() != this->n_bands_) {
    std::cerr << "ERROR: mismatch between calibration factor gain/offset length " << this->gains_.size() << "/"
              << this->offsets_.size()
              << " w.r.t. image band number " << this->n_bands_ << "!!\n";
    return false;
  }
  if (this->normal_sun_irradiance_values_.size() != this->n_bands_) {
    std::cerr << "ERROR: mismatch between image sun irradiance value length " << this->normal_sun_irradiance_values_.size()
              << " w.r.t. image band number " << this->n_bands_ << "!!\n";
    return false;
  }
  if (this->abscal_.size() != this->n_bands_ || this->effect_band_width_.size() != this->n_bands_) {
    std::cerr << "ERROR: mismatch between image absCalFactor/EffectBandWidth value length " << this->abscal_.size() << "/"
              << this->effect_band_width_.size()
              << " w.r.t. image band number " <<  this->n_bands_ << "!!\n";
    return false;
  }

  // normalize the soalr irradiance by earth-sun distance
  double d = brad_sun_distance(year, month, day, hour, min);
  double d_sqr = d*d;
  auto vit = this->normal_sun_irradiance_values_.begin();
  for (; vit != this->normal_sun_irradiance_values_.end(); ++vit)
    *vit /= d_sqr;

  // set PAN band sun_irradiance
  if (this->band_ == "PAN" && this->n_bands_ == 1)
    this->sun_irradiance_ = this->normal_sun_irradiance_values_[0];
  else
    this->sun_irradiance_ = 0.0;

  if (this->verbose_) {
    std::cerr << "scaled solar irradiance (by earth-sun distance)" << std::endl;
    this->print_out_solar_irradiance();
  }

  if(verbose_){
    std::cout << " !!!!!!!!!! satellite name: " << satellite_name_ << " gsd: " << gsd_ << std::endl;
    std::cout << *this;
  }
  return true;
}

// parse image metadata from metadata text file only, without using image header (only consider IMD and PVL for now)
bool brad_image_metadata::parse_from_meta_file(std::string const& meta_file)
{
  if (!vul_file::exists(meta_file))
    return false;
  std::string ext = vul_file::extension(meta_file);

  bool parsed_fine = false;
  if (ext.compare(".IMD") == 0 || ext.compare(".imd") == 0)       // parse IMD file
  {
    parsed_fine = parse_from_imd_only(meta_file);
  }
  else if (ext.compare(".PVL") == 0 || ext.compare(".pvl") == 0)  // parse PVL file
  {
    parsed_fine = parse_from_pvl_only(meta_file);
  }
  else
  {
    std::cout << "ERROR unrecognized metadata file format: " << ext << " in name: " << meta_file << "!\n";
    return false;
  }

  // regularize satellite name
  if (this->satellite_name_.find("IKONOS") != std::string::npos)
    this->satellite_name_ = "IKONOS";
  else if (satellite_name_.find("OV-5") != std::string::npos || satellite_name_.find("GeoEye-1") != std::string::npos ||
           satellite_name_.find("GEOEYE1") != std::string::npos || satellite_name_.find("GE01") != std::string::npos)
    this->satellite_name_ = "GeoEye-1";
  else if (satellite_name_.find("QB02") != std::string::npos || satellite_name_.find("QuickBird") != std::string::npos)
    this->satellite_name_ = "QuickBird";
  else if (satellite_name_.find("WV01") != std::string::npos || satellite_name_.find("WorldView") != std::string::npos)
    this->satellite_name_ = "WorldView";
  else if (satellite_name_.find("WV02") != std::string::npos || satellite_name_.find("WorldView2") != std::string::npos)
    this->satellite_name_ = "WorldView2";
  else if (satellite_name_.find("WorldView3") != std::string::npos || satellite_name_.find("WV03") != std::string::npos)
    this->satellite_name_ = "WorldView3";
  else if (this->satellite_name_.find("DigitalGlobe") != std::string::npos)
    this->satellite_name_ = "DigitalGlobe";

  // read gain offset from default gain/offset table
  // Default Gain/Offset are from DigitalGlobe tech spec website and should updated annually
  // (http://www.digitalglobe.com/resources/technical-information)
  // this table is stored in source/contrib/brl/bbas/brad/brad_sat_img_calibration_table.txt or
  //                      compile/lib/brad_sat_img_calibration_table.txt
  if (!this->read_band_dependent_gain_offset()) {
    std::cerr << "ERROR: Parse gain offset value from default table failed for satellite: " << this->satellite_name_
                << ", can not find this satellite in default gain/offset table!!!\n";
    return false;
  }
  // screen output radiometric correction parameters
  if (this->verbose_)
    this->print_out_radiometric_parameter();

  // read default solar irradiance
  // same as default gain/offset, values are from annual published DigitalGlobe tech spec
  // (http://www.digitalglobe.com/resources/technical-information)

    if (!this->read_band_dependent_solar_irradiance()) {
      std::cerr << "ERROR: Parse solar irradiance value from default table failed for satellite: " << this->satellite_name_
                << ", can not find this satellite in default solar irradiance table!!!\n";
      return false;
    }

  if (this->verbose_) {
    this->print_out_solar_irradiance();
  }

  // check consistancy of gain, offset, sun irradiance w.r.t number of bands
  if (this->gains_.size() != this->n_bands_ || this->offsets_.size() != this->n_bands_) {
    std::cerr << "ERROR: mismatch between calibration factor gain/offset length " << this->gains_.size() << "/"
              << this->offsets_.size()
              << " w.r.t. image band number " << this->n_bands_ << "!!\n";
    return false;
  }
  if (this->normal_sun_irradiance_values_.size() != this->n_bands_) {
    std::cerr << "ERROR: mismatch between image sun irradiance value length " << this->normal_sun_irradiance_values_.size()
              << " w.r.t. image band number " << this->n_bands_ << "!!\n";
    return false;
  }
  if (this->abscal_.size() != this->n_bands_ || this->effect_band_width_.size() != this->n_bands_) {
    std::cerr << "ERROR: mismatch between image absCalFactor/EffectBandWidth value length " << this->abscal_.size() << "/"
              << this->effect_band_width_.size()
              << " w.r.t. image band number " <<  this->n_bands_ << "!!\n";
    return false;
  }

  // normalize the soalr irradiance by earth-sun distance
  double d = brad_sun_distance(this->t_.year, this->t_.month, this->t_.day, this->t_.hour, this->t_.min);
  double d_sqr = d*d;
  auto vit = this->normal_sun_irradiance_values_.begin();
  for (; vit != this->normal_sun_irradiance_values_.end(); ++vit)
    *vit /= d_sqr;

  // set PAN band sun_irradiance
  if (this->band_ == "PAN" && this->n_bands_ == 1)
    this->sun_irradiance_ = this->normal_sun_irradiance_values_[0];
  else
    this->sun_irradiance_ = 0.0;

  if (this->verbose_) {
    std::cout << "scaled solar irradiance (by earth-sun distance)" << std::endl;
    this->print_out_solar_irradiance();
  }

  if(verbose_){
    std::cout << " !!!!!!!!!! satellite name: " << satellite_name_ << " gsd: " << gsd_ << std::endl;
    std::cout << *this;
  }

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
    return (unsigned)std::abs(float(this->t_.min - other.t_.min));
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
  if (std::abs(vgl_area(vgl_intersection(b1, b2)) - vgl_area(b1)) < 0.000000001)
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
  vsl_b_write(os, abscal_);
  vsl_b_write(os, effect_band_width_);
  vsl_b_write(os, gains_);
  vsl_b_write(os, offsets_);
  vsl_b_write(os, normal_sun_irradiance_values_);
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
  vsl_b_write(os, footprint_);
}

//: binary load self from stream
void brad_image_metadata::b_read(vsl_b_istream& is)
{
  if (!is) return;
  short ver;
  vsl_b_read(is, ver);
  if(ver < 5) {
    std::cout << "brad_image_metadata -- out-dated binary io version " << ver << '\n';
    return;
  }

  if (ver == 5) {
    vsl_b_read(is, sun_elevation_);
    vsl_b_read(is, sun_azimuth_);
    vsl_b_read(is, view_elevation_);
    vsl_b_read(is, view_azimuth_);
    vsl_b_read(is, abscal_);
    vsl_b_read(is, effect_band_width_);
    vsl_b_read(is, gains_);
    vsl_b_read(is, offsets_);
    vsl_b_read(is, normal_sun_irradiance_values_);
    vsl_b_read(is, sun_irradiance_);
    vsl_b_read(is, t_.day);
    vsl_b_read(is, t_.hour);
    vsl_b_read(is, t_.min);
    vsl_b_read(is, t_.month);
    vsl_b_read(is, t_.year);
    vsl_b_read(is, number_of_bits_);
    vsl_b_read(is, satellite_name_);
    vsl_b_read(is, cloud_coverage_percentage_);
    // could be done with vgl_io_point_3d, but this file is versioned
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
}

void brad_image_metadata::print_out_radiometric_parameter()
{
  std::cout << "Satellite absolute calibration factor: " << std::endl;
  std::cout << "  AbsCalFactor: [";
  for (unsigned i = 0; i < this->n_bands_; i++) {
    std::cout << this->abscal_[i] << ' ';
  }
  std::cout << "]\n";
  std::cout << "  EffectBandWidth: [";
  for (unsigned i = 0; i < this->n_bands_; i++) {
    std::cout << this->effect_band_width_[i] << ' ';
  }
  std::cout << "]" << std::endl;
  std::cout << "Band dependent relative adjustment factors -- GAIN: (" << this->band_ << "): " << std::endl;
  auto vit = this->gains_.begin();
  for (; vit != this->gains_.end(); ++vit)
    std::cout << *vit << ' ';
  std::cout << "]\n";

  std::cout << "Band dependent relative adjustment factors -- OFFSET (" << this->band_ << "): " << std::endl;
  vit = this->offsets_.begin();
  for (; vit != this->offsets_.end(); ++vit)
    std::cout << *vit << ' ';
  std::cout << "]\n";

  return;
}

void brad_image_metadata::print_out_solar_irradiance()
{
  std::cout << "Satellite normalized solar irradiance: " << std::endl;
  for (double & normal_sun_irradiance_value : this->normal_sun_irradiance_values_)
    std::cout << normal_sun_irradiance_value << ' ';
  std::cout << "]\n";
  return;
}
