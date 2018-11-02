// This is core/vidl/vidl_dshow_istream_params_esf.h
#ifndef vidl_dshow_istream_params_esf_h_
#define vidl_dshow_istream_params_esf_h_
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
//=========================================================================

#include <string>
#include <bitset>
#include <map>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

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
//   vidl_dshow_istream = vidl_dshow_istream(vidl_dshow_istream_params()
//                                             .set_xxx(xxx));
// \endcode
//-------------------------------------------------------------------------
class vidl_dshow_istream_params_esf //: public vidl_iostream_params
{
 public:
  //: Constructor - default
  vidl_dshow_istream_params_esf();

  //: Configure the source filter based on the parameters.
  void configure_filter(const CComPtr<IBaseFilter>& source);

  //: Print a list of parameters and proper value ranges.
  static void print_parameter_help(const std::string& name);

  //: Print a list of parameters and proper value ranges.
  static void print_parameter_help(const CComPtr<IBaseFilter>& filter);

  //: Set properties from a map(string,value).
  vidl_dshow_istream_params_esf& set_properties(
    const std::map<std::string,std::string>& props);

  // Read accessor functions
  // - general params (all dshow_params should have)
  bool register_in_rot()       const { return register_in_rot_;      }
  bool run_when_ready()        const { return run_when_ready_;       }
  std::string save_graph_to()   const { return save_graph_to_;        }
  std::string device_name()     const { return device_name_;          }
  std::string output_filename() const { return output_filename_;      }
  GUID target_output_format()  const { return target_output_format_; }

  // Write accessor functions.
  // - general params (all dshow_params should have)
  vidl_dshow_istream_params_esf& set_register_in_rot(bool);
  vidl_dshow_istream_params_esf& set_run_when_ready(bool);
  vidl_dshow_istream_params_esf& set_save_graph_to(const std::string&);
  vidl_dshow_istream_params_esf& set_device_name(const std::string&);
  vidl_dshow_istream_params_esf& set_output_filename(const std::string&);
  vidl_dshow_istream_params_esf& set_target_output_format(GUID);
  // - IESFProperties specific (see Euresys Source Filter documentation)
  vidl_dshow_istream_params_esf& set_video_standard(ESF_VIDEO_STANDARD,bool);
  vidl_dshow_istream_params_esf& set_resolution(ESF_RESOLUTION,long,long);
  vidl_dshow_istream_params_esf& set_capture_region_size(long,long);
  vidl_dshow_istream_params_esf& set_capture_region_pos(long,long);
  vidl_dshow_istream_params_esf& set_output_format(GUID);
  vidl_dshow_istream_params_esf& set_capture_rate(double);
  vidl_dshow_istream_params_esf& set_brightness(long);
  vidl_dshow_istream_params_esf& set_contrast(long);
  vidl_dshow_istream_params_esf& set_saturation(long);
  // - IESFCompression specific (see Euresys Source Filter documentation)
  vidl_dshow_istream_params_esf& set_bitrate_control(ESF_BITRATECONTROL);
  vidl_dshow_istream_params_esf& set_average_bitrate(long);
  vidl_dshow_istream_params_esf& set_video_quality(long);
  vidl_dshow_istream_params_esf& set_gop_structure(ESF_GOPSTRUCTURE);
  vidl_dshow_istream_params_esf& set_gop_size(long);

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
  std::bitset<esf_property_enum_size> is_property_changed_;

  //: Enable remote graph viewing with GraphEdit.
  bool register_in_rot_;

  //: Immediately run graph after creating it, else wait for run command.
  bool run_when_ready_;

  //: Write the filter graph to filename 'save_graph_to_'.
  std::string save_graph_to_;

  //: Device name.
  std::string device_name_;

  //: If non-empty create file writing section in the filter_graph.
  std::string output_filename_;

  //: Target output format, as returned by current_frame.
  GUID target_output_format_;

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

#endif // vidl_dshow_istream_params_esf_h_
