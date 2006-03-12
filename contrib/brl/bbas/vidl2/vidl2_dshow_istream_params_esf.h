// This is brl/bbas/vidl2/vidl2_dshow_istream_params_esf.h
#ifndef vidl2_dshow_istream_params_esf_h_
#define vidl2_dshow_istream_params_esf_h_
//=========================================================================
//:
// \file
// \brief  Euresys Source Filter (ESF) DirectShow istream parameter block.
// \author Miguel A. Figueroa-Villanueva (miguelf at ieee dot org)
//
// \verbatim
//  Modifications
//   02/18/2006 - File created. (miguelf)
// \endverbatim
//
//=========================================================================

#include <vcl_string.h>
#include <vcl_bitset.h>
#include <vcl_map.h>

#include <atlbase.h>
#include <dshow.h>

#include <initguid.h>
#include <ESFilter.h>

//-------------------------------------------------------------------------
//: Param struct for DirectShow input streams using Euresys Source Filter.
//
// Enables the configuration of DirectShow Euresys source filters
// (IESFilter) through the IESFProperties and IESFCompression interfaces.
//
// Example Usage:
// \code
//   vidl2_dshow_istream = vidl2_dshow_istream(vidl2_dshow_istream_params()
//                                             .set_xxx(xxx));
// \endcode
//-------------------------------------------------------------------------
class vidl2_dshow_istream_params_esf //: public vidl2_iostream_params
{
 public:
  //: Constructor - default
  vidl2_dshow_istream_params_esf();

  //: Configure the source filter based on the parameters.
  void configure_filter(const CComPtr<IBaseFilter>& source);

  //: Print a list of parameters and proper value ranges.
  static void print_parameter_help(const vcl_string& name);

  //: Print a list of parameters and proper value ranges.
  static void print_parameter_help(const CComPtr<IBaseFilter>& filter);

  //: Set properties from a map(string,value).
  vidl2_dshow_istream_params_esf& set_properties(
    const vcl_map<vcl_string,vcl_string>& props);

  // Read accessor functions
  // - general params (all dshow_params should have)
  vcl_string device_name() { return device_name_; }
  bool register_in_rot() { return register_in_rot_; }

  // Write accessor functions.
  // - general params (all dshow_params should have)
  vidl2_dshow_istream_params_esf& set_device_name(const vcl_string&);
  vidl2_dshow_istream_params_esf& set_register_in_rot(bool);
  // - IESFProperties specific (see Euresys Source Filter documentation)
  vidl2_dshow_istream_params_esf& set_video_standard(ESF_VIDEO_STANDARD,bool);
  vidl2_dshow_istream_params_esf& set_resolution(ESF_RESOLUTION,long,long);
  vidl2_dshow_istream_params_esf& set_capture_region_size(long,long);
  vidl2_dshow_istream_params_esf& set_capture_region_pos(long,long);
  vidl2_dshow_istream_params_esf& set_output_format(GUID);
  vidl2_dshow_istream_params_esf& set_capture_rate(double);
  vidl2_dshow_istream_params_esf& set_brightness(long);
  vidl2_dshow_istream_params_esf& set_contrast(long);
  vidl2_dshow_istream_params_esf& set_saturation(long);
  // - IESFCompression specific (see Euresys Source Filter documentation)
  vidl2_dshow_istream_params_esf& set_bitrate_control(ESF_BITRATECONTROL);
  vidl2_dshow_istream_params_esf& set_average_bitrate(long);
  vidl2_dshow_istream_params_esf& set_video_quality(long);
  vidl2_dshow_istream_params_esf& set_gop_structure(ESF_GOPSTRUCTURE);
  vidl2_dshow_istream_params_esf& set_gop_size(long);

 private:
  enum esf_property
  {
    esf_property_video_standard = 0,
    esf_property_resolution,
    esf_property_capture_region_size,
    esf_property_capture_region_pos,
    esf_property_output_format,
    esf_property_capture_rate,
    esf_property_brightness,
    esf_property_contrast,
    esf_property_saturation,
    esf_property_bitrate_control,
    esf_property_average_bitrate,
    esf_property_video_quality,
    esf_property_gop_structure,
    esf_property_gop_size,
    esf_property_enum_size // needs to be at the end
  };

  //: Flag param to be used in configure_filter.
  vcl_bitset<esf_property_enum_size> is_property_changed_;

  //: Device name.
  vcl_string device_name_;

  //: Enable remote graph viewing with GraphEdit.
  bool register_in_rot_;

  // IESFProperties interface.
  ESF_VIDEO_STANDARD video_standard_;
  bool is_monochrome_;
  ESF_RESOLUTION resolution_;
  long width_;
  long height_;
  long capture_region_size_x_;
  long capture_region_size_y_;
  long capture_region_pos_x_;
  long capture_region_pos_y_;
  GUID output_format_;
  double capture_rate_;
  long brightness_;
  long contrast_;
  long saturation_;

  // IESFCompression interface.
  ESF_BITRATECONTROL bitrate_control_;
  long average_bitrate_;
  long video_quality_;
  ESF_GOPSTRUCTURE gop_structure_;
  long gop_size_;
};

#endif // vidl2_dshow_istream_params_esf_h_
