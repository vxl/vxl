// This is core/vpgl/file_formats/vpgl_nitf_rational_camera.cxx
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <cctype>
#include "vpgl_nitf_rational_camera.h"
//:
// \file
// \brief instance a nitf_rational camera from nitf header information.
// \author Jim Green
// \date Dec 2006

#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#include "vil/vil_load.h"
// for calls to get nitf_rational parameters from vil
#include <vil/file_formats/vil_nitf2_image.h>

// Conversion from igeolo string format to doubles
static int
to_int(const char * in_string, int size)
{
  int value = 0;
  while (size--)
    value = (value * 10) + (*in_string++ - '0');
  return value;
}

//: converts one of lat or lon string to a double
static int
geostr_to_double(const char * in_string, double * val, vpgl_nitf_rational_camera::geopt_coord c)
{
  //  int invalid = 1;
  int length;
  int deg, min;
  float fsec;
  const char * orig = in_string;

  // here are lat/lon dependent variables
  char sposdir, cposdir, snegdir, cnegdir;
  int maxval;

  if (c == vpgl_nitf_rational_camera::LAT)
  {
    sposdir = 'n';
    cposdir = 'N';
    snegdir = 's';
    cnegdir = 'S';
    maxval = 90;
  }
  else
  {
    sposdir = 'e';
    cposdir = 'E';
    snegdir = 'w';
    cnegdir = 'W';
    maxval = 180;
  }

  while ((*in_string == ' ') || (*in_string == '\t'))
    ++in_string;

  for (length = 0; std::isdigit(*in_string) && length < 15; ++length)
    ++in_string;
  if (length > 14)
    return 0;

  // three different formats accepted
  // DDDMMSS.S[d] where [d]=nNsSeEwW
  if (length > 3)
  {
    if (length < 5)
      return 0;

    // get the minutes
    if ((min = to_int(in_string - 4, 2)) >= 60 || min < 0)
      return 0;

    // get the degrees
    if ((deg = to_int(in_string - length, length - 4)) > maxval || deg < 0)
      return 0;

    // get the seconds (float)
    in_string -= 2;
    char * temp = new char[2];
    for (length = 0; (*in_string == '.' || std::isdigit(*in_string)) && length < 15; ++length)
      ++in_string;
    if (length > 14)
      return 0;

    std::strncpy(temp, in_string - length, length);
    if ((fsec = (float)std::stod(temp)) >= 60.0f || fsec < 0.0f)
      return 0;
    delete[] temp;

    // skip to the direction
    while ((*in_string == ' ') || (*in_string == '\t'))
      ++in_string;

    // calculate the value
    *val = deg;
    *val += ((double)(min)) / (60.0);
    *val += ((double)(fsec)) / (3600.0);

    // adjust for the direction
    if (*in_string == sposdir || *in_string == cposdir)
    {
    }
    else if (*in_string == snegdir || *in_string == cnegdir)
    {
      *val = -(*val);
    }
    else
      return 0;

    ++in_string;

    return static_cast<int>(in_string - orig);
  }
  else // DDDdMM'SS"[d]  where [d]=nNsSeEwW
    if (*in_string == 'd')
    {
      // get the degrees
      if (length > 3)
        return 0;
      if ((deg = to_int(in_string - length, length)) > maxval || deg < 0)
        return 0;

      // go past 'd' and spaces
      ++in_string;
      while ((*in_string == ' ') || (*in_string == '\t'))
        ++in_string;

      // get the minutes
      for (length = 0; std::isdigit(*in_string) && length < 15; ++in_string, ++length) /*nothing*/
        ;
      if (length > 14)
        return 0;
      if (length > 2)
        return 0;
      if ((min = to_int(in_string - length, length)) >= 60 || min < 0)
        return 0;

      // go past ''' and spaces
      ++in_string;
      while ((*in_string == ' ') || (*in_string == '\t'))
        ++in_string;

      // get the seconds (float)
      char * temp = new char[2];
      for (length = 0; (*in_string == '.' || std::isdigit(*in_string)) && length < 15; ++length)
        ++in_string;
      if (length > 14)
        return 0;

      std::strncpy(temp, in_string - length, length);
      if ((fsec = (float)std::stod(temp)) >= 60.0f || fsec < 0.0f)
        return 0;
      delete[] temp;

      // go past '"' and any spaces to the direction
      ++in_string;
      while ((*in_string == ' ') || (*in_string == '\t'))
        ++in_string;

      // calculate value
      *val = deg;
      *val += ((double)(min)) / (60.0);
      *val += ((double)(fsec)) / (3600.0);

      // adjust for the direction
      if (*in_string == sposdir || *in_string == cposdir)
      {
      }
      else if (*in_string == snegdir || *in_string == cnegdir)
      {
        *val = -(*val);
      }
      else
        return 0;

      ++in_string;

      return static_cast<int>(in_string - orig);
    }
    else // DDD.DDDD
      if (*in_string == ' ' || *in_string == '-' || *in_string == '+' || *in_string == '.' || *in_string == '\0')
      {
        char * temp = new char[2];
        in_string = orig;

        // go past any spaces
        while ((*in_string == ' ') || (*in_string == '\t'))
          ++in_string;

        // calculate length of float
        for (length = 0;
             (*in_string == '+' || *in_string == '-' || *in_string == '.' || std::isdigit(*in_string)) && length < 15;
             ++length)
          ++in_string;
        if (length > 14)
          return 0;

        // calculate value of float
        std::strncpy(temp, in_string - length, length);
        *val = std::stod(temp);
        if (std::fabs(*val) > float(maxval))
          return 0;
        delete[] temp;

        ++in_string;

        return static_cast<int>(in_string - orig);
      }
      else
        return 0;
}

//: converts a latlon in_string to doubles
int
vpgl_nitf_rational_camera::geostr_to_latlon(const char * str, double * lat, double * lon)
{
  int latstrlen = geostr_to_double(str, lat, vpgl_nitf_rational_camera::LAT);
  if (latstrlen == 0)
    return 0;
  str += latstrlen;
  int lonstrlen = geostr_to_double(str, lon, vpgl_nitf_rational_camera::LON);
  if (lonstrlen == 0)
    return 0;

  return latstrlen + lonstrlen;
}
void
vpgl_nitf_rational_camera::geostr_to_latlon_v2(const std::string & str, std::vector<std::pair<double, double>> & coords)
{
  // separate string into four corner sections: (lat, lon), (lat, lon), (lat, lon), (lat, lon)
  std::string::const_iterator sit = str.begin();
  std::vector<double> latlons;
  size_t inc_lat = 7, inc_lon = 8, inc;
  geopt_coord lat_code = vpgl_nitf_rational_camera::LAT;
  geopt_coord lon_code = vpgl_nitf_rational_camera::LON;
  // initial condition
  size_t start = 0;
  inc = inc_lat;
  geopt_coord code = lat_code;
  for (size_t k = 0; k < 8; ++k)
  {
    // extract lat or lon substring
    std::string section;
    double val = 0.0;
    for (size_t cnt = 0; cnt < inc; ++cnt, ++sit)
      section.push_back(*sit);
    // convert to decimal degrees
    geostr_to_double(section.c_str(), &val, code);
    latlons.push_back(val);
    // advance to next substring
    start += inc;
    if (inc == inc_lat)
      inc = inc_lon;
    else
      inc = inc_lat;
    if (code == lat_code)
      code = lon_code;
    else
      code = lat_code;
  }
  // form pairs
  for (size_t i = 0; i < 8; i += 2)
    coords.emplace_back(latlons[i + 1], latlons[i]);
}

//: Read from a nitf image
bool
vpgl_nitf_rational_camera::read(vil_nitf2_image * nitf_image, bool verbose)
{
  std::vector<vil_nitf2_image_subheader *> headers = nitf_image->get_image_headers();
  vil_nitf2_image_subheader * hdr = headers[0];

  // initialize the array
  double tre_data[90];
  for (double & i : tre_data)
    i = 0;

  // read information
  bool success = hdr->get_rpc_params(nitf_rational_type_, image_id_, image_igeolo_, tre_data);
  if (!success)
  {
    std::cerr << "Failed to get rational camera parameters from nitf image in"
              << " vgpl_nitf_rational_camera\n";
    return false;
  }

  // extract corner coordinates from image_geolo field
  // example 324158N1171117W324506N1171031W324428N1170648W324120N1170734W
  double ULlat, ULlon;
  double URlat, URlon;
  double LLlat, LLlon;
  double LRlat, LRlon;

  geostr_to_latlon(image_igeolo_.c_str(), &ULlat, &ULlon);
  geostr_to_latlon(image_igeolo_.c_str() + 15, &URlat, &URlon);
  geostr_to_latlon(image_igeolo_.c_str() + 30, &LRlat, &LRlon);
  geostr_to_latlon(image_igeolo_.c_str() + 45, &LLlat, &LLlon);

  ul_[LAT] = ULlat;
  ul_[LON] = ULlon;
  ur_[LAT] = URlat;
  ur_[LON] = URlon;
  ll_[LAT] = LLlat;
  ll_[LON] = LLlon;
  lr_[LAT] = LRlat;
  lr_[LON] = LRlon;

  // set coefficients
  vnl_matrix_fixed<double, 4, 20> coeffs;
  for (unsigned i = 0; i < 20; i++)
  {
    coeffs[NEU_V][i] = tre_data[i];
    coeffs[DEN_V][i] = tre_data[i + 20];
    coeffs[NEU_U][i] = tre_data[i + 40];
    coeffs[DEN_U][i] = tre_data[i + 60];
  }
  auto input_order = vpgl_rational_order_func::from_string(nitf_rational_type_);
  this->set_coefficients(coeffs, input_order);

  // set scale/offset
  this->set_scale_offsets(tre_data[88],
                          tre_data[83], // x
                          tre_data[87],
                          tre_data[82], // y
                          tre_data[89],
                          tre_data[84], // z
                          tre_data[86],
                          tre_data[81], // u
                          tre_data[85],
                          tre_data[80] // v
  );

  // correction offset
  double correction_u_off, correction_v_off;
  success = hdr->get_image_offset(correction_u_off, correction_v_off);

  if (success)
  {
    scale_offsets_[U_INDX].set_offset(scale_offsets_[U_INDX].offset() - correction_u_off);
    scale_offsets_[V_INDX].set_offset(scale_offsets_[V_INDX].offset() - correction_v_off);
  }

  // verbose report
  if (verbose)
    this->print();

  // cleanup
  return true;
}

//: Read from a nitf image file
bool
vpgl_nitf_rational_camera::read(const std::string & nitf_image_path, bool verbose)
{
  // first open the nitf image
  vil_image_resource_sptr image = vil_load_image_resource(nitf_image_path.c_str());
  if (!image)
  {
    std::cerr << "Image load failed in vpgl_nitf_rational_camera_constructor\n";
    return false;
  }
  std::string format = image->file_format();
  std::string prefix = format.substr(0, 4);
  if (prefix != "nitf")
  {
    std::cerr << "not a nitf image in vpgl_nitf_rational_camera_constructor\n";
    return false;
  }
  // cast to an nitf2_image
  auto * nitf_image = (vil_nitf2_image *)image.ptr();

  // read information
  return this->read(nitf_image, verbose);
}

vpgl_nitf_rational_camera::vpgl_nitf_rational_camera(vil_nitf2_image * nitf_image, bool verbose)
{
  this->read(nitf_image, verbose);
}

vpgl_nitf_rational_camera::vpgl_nitf_rational_camera(const std::string & nitf_image_path, bool verbose)
{
  this->read(nitf_image_path, verbose);
}

// print all camera information
void
vpgl_nitf_rational_camera::print(std::ostream & ostr, vpgl_rational_order output_order) const
{
  // print basic information
  ostr << " nitf_rational_type " << nitf_rational_type_ << std::endl
       << " Image Id " << image_id_ << std::endl
       << " IGEOLO " << image_igeolo_ << std::endl;

  // print rational camera in PVL format
  this->write_pvl(ostr, output_order);

  // print corners & projections
  double z_off = this->offset(Z_INDX);
  double u, v;

  this->project(ul_[LON], ul_[LAT], z_off, u, v);
  ostr << "upper left: " << std::endl
       << "  lon/lat = " << ul_[LON] << "/" << ul_[LAT] << std::endl
       << "  row/col = " << v << "/" << u << std::endl;

  this->project(ur_[LON], ur_[LAT], z_off, u, v);
  ostr << "upper right: " << std::endl
       << "  lon/lat = " << ur_[LON] << "/" << ur_[LAT] << std::endl
       << "  row/col = " << v << "/" << u << std::endl;

  this->project(lr_[LON], lr_[LAT], z_off, u, v);
  ostr << "lower right: " << std::endl
       << "  lon/lat = " << lr_[LON] << "/" << lr_[LAT] << std::endl
       << "  row/col = " << v << "/" << u << std::endl;

  this->project(ll_[LON], ll_[LAT], z_off, u, v);
  ostr << "lower left: " << std::endl
       << "  lon/lat = " << ll_[LON] << "/" << ll_[LAT] << std::endl
       << "  row/col = " << v << "/" << u << std::endl;
}
