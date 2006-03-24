// This is brl/bbas/vidl2/vidl2_dshow_istream_params.h
#ifndef vidl2_dshow_istream_params_h_
#define vidl2_dshow_istream_params_h_
//=========================================================================
//:
// \file
// \brief  DirectShow input stream parameter block.
// \author Miguel A. Figueroa-Villanueva (miguelf at ieee dot org)
//
// \verbatim
//  Modifications
//   01/30/2006 - File created. (miguelfv)
// \endverbatim
//=========================================================================

#include <vcl_string.h>
#include <vcl_map.h>

#include <atlbase.h>
#include <dshow.h>

//-------------------------------------------------------------------------
// ***** implementation detail...
//       maybe remove from here and forward declare
// *****
//-------------------------------------------------------------------------
struct null_type {};

template <typename T, typename U = null_type>
struct property_wrap;

template <typename T, typename U>
struct property_wrap
{
  T     key;
  U     value;
  bool  is_changed;

  property_wrap(const T& key_tag = T())
    : key(key_tag)
    , is_changed(false)
  {}
};

template <typename U>
struct property_wrap<U,null_type>
{
  U     value;
  bool  is_changed;

  property_wrap()
    : is_changed(false)
  {}
};

typedef property_wrap<VideoProcAmpProperty,long> vpa_property_wrap;

//-------------------------------------------------------------------------
//: Parameter structure for DirectShow input streams.
//
// Enables the configuration of DirectShow source filters through the
// IAMVideoProcAmp and IAMStreamConfig interfaces.
//
// Example Usage:
// \code
//   vidl2_dshow_istream = vidl2_dshow_istream(vidl2_dshow_istream_params()
//                                             .set_xxx(xxx));
// \endcode
//-------------------------------------------------------------------------
class vidl2_dshow_istream_params //: public vidl2_iostream_params
{
 public:
  //: Constructor - default
  vidl2_dshow_istream_params();

  //: Configure the source filter based on the parameters.
  void configure_filter(const CComPtr<IBaseFilter>& source);

  //: Print a list of parameters and proper value ranges.
  static void print_parameter_help(const vcl_string& name);

  //: Print a list of parameters and proper value ranges.
  static void print_parameter_help(const CComPtr<IBaseFilter>& filter);

  //: Set properties from a map(string,value).
  vidl2_dshow_istream_params& set_properties(
    const vcl_map<vcl_string,vcl_string>& props);

  // Read accessor functions
  // - general params (all dshow_params should have)
  bool register_in_rot()       { return register_in_rot_;      }
  bool run_when_ready()        { return run_when_ready_;       }
  vcl_string save_graph_to()   { return save_graph_to_;        }
  vcl_string device_name()     { return device_name_;          }
  vcl_string output_filename() { return output_filename_;      }
  GUID target_output_format()  { return target_output_format_; }

  // Write accessor functions.
  // - general params (all dshow_params should have)
  vidl2_dshow_istream_params& set_register_in_rot(bool);
  vidl2_dshow_istream_params& set_run_when_ready(bool);
  vidl2_dshow_istream_params& set_save_graph_to(const vcl_string&);
  vidl2_dshow_istream_params& set_device_name(const vcl_string&);
  vidl2_dshow_istream_params& set_output_filename(const vcl_string&);
  vidl2_dshow_istream_params& set_target_output_format(GUID);
  vidl2_dshow_istream_params& set_load_filter_defaults(bool);
  // - IAMVideoProcAmp specific (see DirectShow documentation)
  // - IAMStreamConfig specific (see DirectShow documentation)
  vidl2_dshow_istream_params& set_output_format(int);

 private:
  //: Enable remote graph viewing with GraphEdit.
  bool register_in_rot_;

  //: Immediately run graph after creating it, else wait for run command.
  bool run_when_ready_;

  //: Write the filter graph to filename 'save_graph_to_'.
  vcl_string save_graph_to_;

  //: Device name.
  vcl_string device_name_;

  //: If non-empty create file writing section in the filter_graph.
  vcl_string output_filename_;

  //: Target output format, as returned by current_frame.
  GUID target_output_format_;

  //: Load source filter defaults for values not manually set.
  bool load_filter_defaults_;

  // IAMVideoProcAmp interface.
  vcl_map<vcl_string,vpa_property_wrap> vpa_properties_;
  //long brightness_;
  //long contrast_;
  //long hue_;
  //long saturation_;
  //long sharpness_;
  //long gamma_;
  //long color_enable_;
  //long white_balance_;
  //long backlight_compensation_;
  //long gain_;

  // IAMStreamConfig interface.
  property_wrap<int> output_format_;
  //long output_format_;
};

#endif // vidl2_dshow_istream_params_h_
