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
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_load.h>
// for calls to get nitf_rational parameters from vil
#include <vil/file_formats/vil_nitf2_image.h>

// Conversion from igeolo string format to doubles
static int to_int (const char* in_string,int size)
{
  int value = 0;
  while (size--)
    value = (value*10) + (*in_string++ - '0');
  return value;
}

//: converts one of lat or lon string to a double
static int geostr_to_double(const char* in_string, double* val, vpgl_nitf_rational_camera::geopt_coord c)
{
  //  int invalid = 1;
  int length;
  int deg, min;
  float fsec;
  const char* orig = in_string;

  //here are lat/lon dependent variables
  char sposdir, cposdir, snegdir, cnegdir;
  int maxval;

  if (c == vpgl_nitf_rational_camera::LAT)
  {
    sposdir='n';
    cposdir='N';
    snegdir='s';
    cnegdir='S';
    maxval=90;
  }
  else
  {
    sposdir='e';
    cposdir='E';
    snegdir='w';
    cnegdir='W';
    maxval=180;
  }

  while ((*in_string == ' ') || (*in_string == '\t'))
    ++in_string;

  for (length=0; std::isdigit (*in_string) && length<15; ++length)
    ++in_string;
  if (length>14) return 0;

  //three different formats accepted
  //DDDMMSS.S[d] where [d]=nNsSeEwW
  if (length>3)
  {
    if (length < 5)
      return 0;

    //get the minutes
    if ((min = to_int (in_string-4, 2)) >= 60 || min<0)
      return 0;

    //get the degrees
    if ((deg = to_int (in_string-length, length-4)) > maxval || deg<0)
      return 0;

    //get the seconds (float)
    in_string-=2;
    char* temp = new char[2];
    for (length=0;
         (*in_string=='.' || std::isdigit (*in_string)) && length<15;
         ++length)
      ++in_string;
    if (length>14) return 0;

    std::strncpy(temp,in_string-length,length);
    if ( (fsec = (float)std::atof(temp)) >= 60.0f || fsec<0.0f)
      return 0;
    delete [] temp;

    //skip to the direction
    while ((*in_string == ' ') || (*in_string == '\t'))
      ++in_string;

    //calculate the value
    *val = deg;
    *val += ((double)(min))/(60.0);
    *val += ((double)(fsec))/(3600.0);

    //adjust for the direction
    if ( *in_string==sposdir || *in_string==cposdir) {}
    else if ( *in_string==snegdir || *in_string==cnegdir) {*val = -(*val);}
    else return 0;

    ++in_string;

    return static_cast<int>(in_string - orig);
  }
  else //DDDdMM'SS"[d]  where [d]=nNsSeEwW
  if (*in_string == 'd')
  {
    //get the degrees
    if (length > 3)
      return 0;
    if ((deg = to_int (in_string-length, length)) > maxval || deg<0)
      return 0;

    //go past 'd' and spaces
    ++in_string;
    while ((*in_string == ' ') || (*in_string == '\t'))
      ++in_string;

    //get the minutes
    for (length=0; std::isdigit (*in_string) && length<15; ++in_string, ++length) /*nothing*/;
    if (length>14) return 0;
    if (length > 2)
      return 0;
    if ((min = to_int (in_string-length, length)) >= 60 || min<0)
      return 0;

    //go past ''' and spaces
    ++in_string;
    while ((*in_string == ' ') || (*in_string == '\t'))
      ++in_string;

    //get the seconds (float)
    char* temp= new char[2];
    for (length=0;
         (*in_string=='.' || std::isdigit (*in_string)) && length<15;
         ++length)
      ++in_string;
    if (length>14) return 0;

    std::strncpy(temp,in_string-length,length);
    if ( (fsec = (float)std::atof(temp)) >= 60.0f || fsec<0.0f)
      return 0;
    delete [] temp;

    //go past '"' and any spaces to the direction
    ++in_string;
    while ((*in_string == ' ') || (*in_string == '\t'))
      ++in_string;

    //calculate value
    *val = deg;
    *val += ((double)(min))/(60.0);
    *val += ((double)(fsec))/(3600.0);

    //adjust for the direction
    if ( *in_string==sposdir || *in_string==cposdir) {}
    else if ( *in_string==snegdir || *in_string==cnegdir) {*val = -(*val);}
    else return 0;

    ++in_string;

    return static_cast<int>(in_string - orig);
  }
  else //DDD.DDDD
  if (*in_string == ' ' || *in_string == '-' || *in_string == '+'
      || *in_string == '.' || *in_string == '\0')
  {
    char* temp= new char[2];
    in_string=orig;

    //go past any spaces
    while ((*in_string == ' ') || (*in_string == '\t'))
      ++in_string;

    //calculate length of float
    for (length=0;
         (*in_string=='+' ||*in_string=='-' || *in_string=='.' ||
          std::isdigit (*in_string)) && length<15;
         ++length)
      ++in_string;
    if (length>14) return 0;

    //calculate value of float
    std::strncpy(temp,in_string-length,length);
    *val = std::atof(temp);
    if (std::fabs(*val)>float(maxval)) return 0;
    delete [] temp;

    ++in_string;

    return static_cast<int>(in_string - orig);
  }
  else
    return 0;
}

//: converts a latlon in_string to doubles
static int geostr_to_latlon(const char* str, double* lat, double* lon)
{
  int latstrlen=geostr_to_double(str,lat,vpgl_nitf_rational_camera::LAT);
  if (latstrlen == 0) return 0;
  str += latstrlen;
  int lonstrlen=geostr_to_double(str,lon,vpgl_nitf_rational_camera::LON);
  if ( lonstrlen == 0) return 0;

  return latstrlen+lonstrlen;
}

// Coefficient ordering possibilities
// NITF_RATIONAL00B - commercial + airborne
void vpgl_nitf_rational_camera::set_order_b(int* ord)
{
  ord[0]  = 11; //  0, xxx);
  ord[1]  = 14; //  1, xxy);
  ord[2]  = 17; //  2, xxz);
  ord[3]  =  7; //  3, xx );
  ord[4]  = 12; //  4, xyy);
  ord[5]  = 10; //  5, xyz);
  ord[6]  =  4; //  6, xy );
  ord[7]  = 13; //  7, xzz);
  ord[8]  =  5; //  8, xz );
  ord[9]  =  1; //  9, x  );
  ord[10] = 15; // 10, yyy);
  ord[11] = 18; // 11, yyz);
  ord[12] =  8; // 12, yy );
  ord[13] = 16; // 13, yzz);
  ord[14] =  6; // 14, yz );
  ord[15] =  2; // 15, y  );
  ord[16] = 19; // 16, zzz);
  ord[17] =  9; // 17, zz );
  ord[18] =  3; // 18, z  );
  ord[19] =  0; // 19, 1  );
}

bool vpgl_nitf_rational_camera::
init(vil_nitf2_image* nitf_image, bool verbose)
{
  std::vector< vil_nitf2_image_subheader* > headers = nitf_image->get_image_headers();
  vil_nitf2_image_subheader* hdr = headers[0];

  double tre_data[90];
  // initialize the array
  for (double & i : tre_data) i = 0;


  bool success =
    hdr->get_rpc_params(nitf_rational_type_, image_id_, image_igeolo_,  tre_data);
  if (!success)
  {
    std::cout << "Failed to get rational camera parameters from nitf image in"
             << " vgpl_nitf_rational_camera\n";
    return false;
  }

  if (verbose)
    std::cout << " nitf_rational type " << nitf_rational_type_ << '\n'
             << " Image Id " << image_id_ << '\n'
             << " IGEOLO " << image_igeolo_ << '\n';
  // example 324158N1171117W324506N1171031W324428N1170648W324120N1170734W
  double ULlat, ULlon;
  double URlat, URlon;
  double LLlat, LLlon;
  double LRlat, LRlon;

  // Extract them from the image_igeolo field
  geostr_to_latlon (image_igeolo_.c_str(),    &ULlat, &ULlon);
  geostr_to_latlon (image_igeolo_.c_str()+15, &URlat, &URlon);
  geostr_to_latlon (image_igeolo_.c_str()+30, &LRlat, &LRlon);
  geostr_to_latlon (image_igeolo_.c_str()+45, &LLlat, &LLlon);

  ul_[LAT]=ULlat;   ul_[LON]=ULlon;
  ur_[LAT]=URlat;   ur_[LON]=URlon;
  ll_[LAT]=LLlat;   ll_[LON]=LLlon;
  lr_[LAT]=LRlat;   lr_[LON]=LRlon;

  if (verbose)
    std::cout << "ULlon " << ULlon << " ULlat " << ULlat << '\n'
             << "URlon " << URlon << " URlat " << URlat << '\n'
             << "LRlon " << LRlon << " LRlat " << LRlat << '\n'
             << "LLlon " << LLlon << " lLlat " << LLlat << '\n';
  int ord[20];
  // set order of coefficients depending on call parameter "coef_ordering" = coefficient order
  if (nitf_rational_type_ == "RPC00A")
    set_order_b(ord);
  else if (nitf_rational_type_ == "RPC00B")
    set_order_b(ord);
  else
  {
    std::cout << "Unknown rational type from nitf image in"
             << " vgpl_nitf_rational_camera\n";
    return false;
  }


  // apply the 80 coefficients to the std::vectors to instance the vpgl_rational_camera
  for (int i=0; i<20; i++)
  {
    rational_coeffs_[2][i] = tre_data[ord[i]];
    rational_coeffs_[3][i] = tre_data[ord[i] + 20];
    rational_coeffs_[0][i] = tre_data[ord[i] + 40];
    rational_coeffs_[1][i] = tre_data[ord[i] + 60];
  }
  // also fill in the scale & offset normalization parameters
  scale_offsets_[X_INDX].set_scale(tre_data[88]);
  scale_offsets_[X_INDX].set_offset(tre_data[83]);
  scale_offsets_[Y_INDX].set_scale(tre_data[87]);
  scale_offsets_[Y_INDX].set_offset(tre_data[82]);
  scale_offsets_[Z_INDX].set_scale(tre_data[89]);
  scale_offsets_[Z_INDX].set_offset(tre_data[84]);
  scale_offsets_[U_INDX].set_scale(tre_data[86]);
  scale_offsets_[U_INDX].set_offset(tre_data[81]);
  scale_offsets_[V_INDX].set_scale(tre_data[85]);
  scale_offsets_[V_INDX].set_offset(tre_data[80]);

  double correction_u_off,correction_v_off;
  success=hdr->get_correction_offset(correction_u_off,correction_v_off);

  if (success)
  {
      scale_offsets_[U_INDX].set_offset(scale_offsets_[U_INDX].offset()-correction_u_off);
      scale_offsets_[V_INDX].set_offset(scale_offsets_[V_INDX].offset()-correction_v_off);
  }
  return true;
}

vpgl_nitf_rational_camera::vpgl_nitf_rational_camera() = default;


vpgl_nitf_rational_camera::
vpgl_nitf_rational_camera(std::string const& nitf_image_path,
                          bool verbose)
{
  //first open the nitf image
  vil_image_resource_sptr image =
    vil_load_image_resource(nitf_image_path.c_str());
  if (!image)
  {
    std::cout << "Image load failed in vpgl_nitf_rational_camera_constructor\n";
    return ;
  }
  std::string format = image->file_format();
  std::string prefix = format.substr(0,4);
  if (prefix != "nitf")
  {
    std::cout << "not a nitf image in vpgl_nitf_rational_camera_constructor\n";
    return;
  }
  //cast to an nitf2_image
  auto* nitf_image = (vil_nitf2_image*)image.ptr();
  //Get and set the information
  if (!this->init(nitf_image, verbose))
    return;
  vpgl_scale_offset<double> z = scale_offsets_[Z_INDX];
  double z_off = z.offset();
  if (verbose)
  {
    double ul_u=0, ul_v=0, ur_u=0, ur_v=0, ll_u=0, ll_v=0, lr_u=0, lr_v=0;
    // Project upper left corner
    this->project(ul_[LON], ul_[LAT], z_off, ul_u, ul_v);
    std::cout << "Upper left image corner(" << ul_u << ' ' << ul_v << ")\n";
    // Project upper right corner
    this->project(ur_[LON], ur_[LAT], z_off, ur_u, ur_v);
    std::cout << "Upper right image corner(" << ur_u << ' ' << ur_v << ")\n";
    // Project lower left corner
    this->project(ll_[LON], ll_[LAT], z_off, ll_u, ll_v);
    std::cout << "Lower left image corner(" << ll_u << ' ' << ll_v << ")\n";
    // Project lower right corner
    this->project(lr_[LON], lr_[LAT], z_off, lr_u, lr_v);
    std::cout << "Lower right image corner(" << lr_u << ' ' << lr_v << ")\n";
  }
}

vpgl_nitf_rational_camera::
vpgl_nitf_rational_camera(vil_nitf2_image* nitf_image, bool verbose)
{
  //Get and set the information
  if (!this->init(nitf_image, verbose))
    return;

  if (verbose)
    std::cout << *this;
  vpgl_scale_offset<double> z = scale_offsets_[Z_INDX];
  double z_off = z.offset();
  if (verbose)
  {
    double ul_u=0, ul_v=0, ur_u=0, ur_v=0, ll_u=0, ll_v=0, lr_u=0, lr_v=0;
    // Project upper left corner
    this->project(ul_[LON], ul_[LAT], z_off, ul_u, ul_v);
    std::cout << "Upper left image corner(" << ul_u << ' ' << ul_v << ")\n";
    // Project upper right corner
    this->project(ur_[LON], ur_[LAT], z_off, ur_u, ur_v);
    std::cout << "Upper right image corner(" << ur_u << ' ' << ur_v << ")\n";
    // Project lower left corner
    this->project(ll_[LON], ll_[LAT], z_off, ll_u, ll_v);
    std::cout << "Lower left image corner(" << ll_u << ' ' << ll_v << ")\n";
    // Project lower right corner
    this->project(lr_[LON], lr_[LAT], z_off, lr_u, lr_v);
    std::cout << "Lower right image corner(" << lr_u << ' ' << lr_v << ")\n";
  }
}
