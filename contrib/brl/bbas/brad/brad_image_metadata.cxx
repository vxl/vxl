#include "brad_image_metadata.h"
//:
// \file

#include <vul/vul_file.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vul/vul_awk.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vcl_sstream.h>

#include <vil/vil_image_resource.h>
#include <vil/vil_load.h>
#include <vil/file_formats/vil_nitf2_image.h>
#include <vpgl/file_formats/vpgl_nitf_rational_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>
#include <vpgl/vpgl_lvcs.h>
#include <vpgl/algo/vpgl_backproject.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>

#include <brad/brad_sun_pos.h>

void parse_from_imd(vcl_string filename, brad_image_metadata_sptr& md)
{
  vcl_ifstream ifs( filename.c_str() );
  //: now parse the IMD file
  vul_awk awk(ifs);
  double absCalfact = 1.0;
  double effectiveBand = 1.0;
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
      continue;
    }
  }
  vcl_cout << "abs: " << absCalfact << " eff: " << effectiveBand << vcl_endl;
  md->gain_ = absCalfact/effectiveBand;
  md->offset_ = 0.0;
  return;
}

void parse_from_pvl(vcl_string filename, brad_image_metadata_sptr& md)
{
  vcl_cout << "Parse from PVL file is not implemented yet!\n";
  return;
}

//: parse header in nitf image, assumes that metadata files are in the same folder with the image
//   if meta_folder is not empty, they are searched in that folder as well
bool parse(vcl_string& nitf_filename, brad_image_metadata_sptr& md, vcl_string meta_folder)
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

  //get NITF information
  vcl_vector< vil_nitf2_image_subheader* > headers = nitf_image->get_image_headers();
  vil_nitf2_image_subheader* hdr = headers[0];

  double sun_el;
  double sun_az;
  if (!hdr->get_sun_params(sun_el, sun_az)) {
    vcl_cerr << "failed to obtain sun parameters info\n";
    return false;
  }
  
  md->sun_elevation_ = sun_el;
  md->sun_azimuth_ = sun_az;

  int year, month, day, hour, min;
  if (!hdr->get_date_time(year, month, day, hour,  min)) {
    vcl_cerr << "failed to obtain date time info\n";
    return false;
  }
  md->t_.year = year; md->t_.month = month; md->t_.day = day; md->t_.hour = hour; md->t_.min = min;

  md->number_of_bits_ = hdr->get_number_of_bits_per_pixel();

  double solar_irrad = 1.0;
  vcl_string img_info = hdr->get_image_source();
  if (img_info.find("IKONOS") != vcl_string::npos || nitf_filename.find("IK") != vcl_string::npos) 
    solar_irrad = 1375.8;
  else if (img_info.find("GeoEye-1") != vcl_string::npos)  // OZGE TODO: check this one
    solar_irrad = 1617;
  else if (img_info.find("QuickBird") != vcl_string::npos || nitf_filename.find("QB") != vcl_string::npos)
    solar_irrad = 1381.7;
  else if (img_info.find("WorldView") != vcl_string::npos || nitf_filename.find("WV") != vcl_string::npos)
    solar_irrad = 1580.814;
  else 
    vcl_cerr << "Cannot find satellite name for: " << img_info << " in NITF: " << nitf_filename;
  vcl_cout << "solar_irrad: " << solar_irrad << vcl_endl;
  //: set sun irradiance using Eart-Sun distance
  double d = brad_sun_distance(year, month, day, hour, min);
  md->sun_irradiance_ = solar_irrad/(d*d);

  // compute satellite az,el values for center of image 
  vpgl_nitf_rational_camera nitf_cam(nitf_image, false);
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
  md->view_elevation_ = vcl_asin(to_camera.z()) * rad_to_deg;
  // degrees east of north
  md->view_azimuth_ = vcl_atan2(to_camera.x(), to_camera.y()) * rad_to_deg;
  if (md->view_azimuth_ < 0)
    md->view_azimuth_ += 360;
 
  vcl_string dirname = vul_file::dirname(nitf_filename);

  //: set gain offset defaults, some satellites' images do not require any adjustment
  md->gain_ = 1.0f;
  md->offset_ = 0.0f; 

  //: look for metadata files with known formats recursively in the directory of the image
  //  if we find one, check file name to see if it is for the same image, if so parse it
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
    vcl_cout << " searching " << meta_folder << vcl_endl;
    vcl_string in_dir = meta_folder + "/*.*";
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
  } 

  if (meta_filename.size() == 0) {
    // check if this is IKONOS
    vcl_string type = hdr->get_image_type(); // type mono is band PAN
    unsigned bpp = md->number_of_bits_;
    vcl_cout << "Ikonos: bpp " << bpp << " type: " << type << vcl_endl;
    if (img_info.compare("IKONOS") == 0 && type.compare("MONO") == 0 && bpp == (unsigned)11) {
      vcl_cout << "An 11-bit Panchromatic IKONOS image, setting gain & offset values according to tech document\n";
      md->gain_ = (10.0/161.0)/0.403;
      md->offset_ = 0.0;
    } else {
      vcl_cout << "could not set gain and offset for " << imagename << vcl_endl;
    }
    vcl_cout << *md;
    return true;
  }
  
  vcl_string ext = vul_file::extension(meta_filename);
  if (ext.compare(".IMD") == 0 || ext.compare(".imd") == 0)
    parse_from_imd(meta_filename, md);
  else if (ext.compare(".PVL") == 0 || ext.compare(".pvl") == 0)
    parse_from_pvl(meta_filename, md);
  else
    vcl_cout << "unknown meta file format: " << ext << " in name: " << meta_filename << "!\n";

  vcl_cout << *md;

  return true;
}

