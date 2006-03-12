// This is brl/bbas/vidl2/vidl2_dshow_istream_params_esf.cxx
//=========================================================================
//:
// \file
// \brief  Euresys Source Filter (ESF) DirectShow istream parameter block.
//
// See vidl2_dshow_istream_params_esf.h for details.
//
//=========================================================================

#include <vidl2/vidl2_dshow_istream_params_esf.h>
#include <vidl2/vidl2_dshow.h>

#include <vcl_iostream.h>
#include <vcl_iomanip.h>
#include <vcl_sstream.h>
#include <vcl_utility.h>

#include <vbl/vbl_triple.h>

#if 0 // ***** This shouldn't be here.... maybe in vbl_functors.h or something.
//-----------------------------------------------------------------------------
//: Functor for finding a value in a std::map.
//   E.g., find_if(map.begin(), map.end(), second_equal_to<int>(val))
template <class T> struct second_equal_to
{
  T val_;
  second_equal_to(const T& val) : val_(val) {}
  bool operator()(const vcl_pair<vcl_string,T>& x) const
  {
    return x.second == val_;
  }
};
#endif // 0

//-------------------------------------------------------------------------
// Private helpers.
//-------------------------------------------------------------------------
namespace
{
  const char* esf_video_standards[] = {"ESF_STANDARD_PAL",
                                       "ESF_STANDARD_NTSC"};

  const char* esf_resolutions[] = {"ESF_RESOLUTION_FRAME",
                                   "ESF_RESOLUTION_FIELD",
                                   "ESF_RESOLUTION_CIF",
                                   "ESF_RESOLUTION_QCIF",
                                   "ESF_RESOLUTION_CUSTOM"};

  const char* esf_bitratecontrols[] = {"ESF_BITRATECONTROL_CBR",
                                       "ESF_BITRATECONTROL_VBR"};

  const char* esf_gopstructures[] = {"ESF_GOPSTRUCTURE_IONLY",
                                     "ESF_GOPSTRUCTURE_IPONLY",
                                     "ESF_GOPSTRUCTURE_IPB"};

  // widths for formatting help (iomanip setw)
  const int w1 = 25;
  const int w2 = 8;

  inline void print_4_column_row(const vcl_string& str,
                                 long val, long min, long max)
  {
    vcl_cout << vcl_setw(w1) << str
             << vcl_setw(w2) << val
             << vcl_setw(w2) << min
             << vcl_setw(w2) << max << '\n';
  }

  inline void print_5_column_row(const vcl_string& str,
                                 long val, long min, long max, long def)
  {
    vcl_cout << vcl_setw(w1) << str
             << vcl_setw(w2) << val
             << vcl_setw(w2) << min
             << vcl_setw(w2) << max
             << vcl_setw(w2) << def << '\n';
  }

  void print_video_standard_help(const CComPtr<IESFProperties>& props)
  {
    ESF_VIDEO_STANDARD val;
    bool mono;

    props->GetVideoStandard(&val, &mono);
    vcl_cout << vcl_setw(w1) << "video_standard" << ' '
             << esf_video_standards[val] << '\n';

    vcl_cout << vcl_setw(w1) << "  Supported video standards\n"
             << vcl_setw(w1) << "  -------------------------\n";
    for (int i = 0; i < sizeof(esf_video_standards) / sizeof(char*); i++)
    {
      vcl_cout << vcl_setw(w1) << i << ' '
               << esf_video_standards[i] << '\n';
    }

    vcl_cout << vcl_setw(w1) << "is_monochrome"
             << vcl_setw(w2) << (mono ? "true" : "false") << '\n';
  }

  void print_resolution_help(const CComPtr<IESFProperties>& props)
  {
    //vcl_map<vcl_string,ESF_RESOLUTION> res_enum = esf_resolutions();

    ESF_RESOLUTION val;
    long width, height;

    props->GetResolution(&val, &width, &height);
    vcl_cout << vcl_setw(w1) << "resolution "
             << esf_resolutions[val]
             << " (" << width << 'x' << height << ")\n";

    // print options
    vcl_cout << vcl_setw(w1) << "  Supported resolutions\n"
             << vcl_setw(w1) << "  ---------------------\n";
    for (int i = 0; i < sizeof(esf_resolutions) / sizeof(char*); i++)
    {
      vcl_cout << vcl_setw(w1) << i << ' '
               << esf_resolutions[i] << '\n';
    }

    long min_x, max_x, min_y, max_y;
    props->GetCustomResolutionRange(&min_x, &max_x, &min_y, &max_y);
    vcl_cout << vcl_setw(w1) << "    custom range ("
             << min_x << 'x' << min_y << ") -> ("
             << max_x << 'x' << max_y << ")\n";
  }

  void print_capture_region_size_help(const CComPtr<IESFProperties>& props)
  {
    long val_x, min_x, max_x;
    long val_y, min_y, max_y;
    props->GetCaptureRegionSize(&val_x, &val_y);
    props->GetCaptureRegionSizeRange(&min_x, &max_x, &min_y, &max_y);
    print_4_column_row("capture_region_size_x", val_x, min_x, max_x);
    print_4_column_row("capture_region_size_y", val_y, min_y, max_y);
  }

  void print_capture_region_pos_help(const CComPtr<IESFProperties>& props)
  {
    long val_x, min_x, max_x;
    long val_y, min_y, max_y;
    props->GetCaptureRegionPos(&val_x, &val_y);
    props->GetCaptureRegionPosRange(&min_x, &max_x, &min_y, &max_y);
    print_4_column_row("capture_region_pos_x", val_x, min_x, max_x);
    print_4_column_row("capture_region_pos_y", val_y, min_y, max_y);
  }

  void print_output_format_help(const CComPtr<IESFProperties>& props)
  {
    GUID val;

    props->GetOutputFormat(&val);
    vcl_cout << vcl_setw(w1) << "output_format" << ' '
             << vidl2_dshow::get_guid_name(val) << '\n';

    GUID *table = 0;
    unsigned long count = 0;
    if (SUCCEEDED(props->GetOutputFormatsList(&count, &table)))
    {
      // Process list
      vcl_cout << vcl_setw(w1) << "  Supported output formats\n"
               << vcl_setw(w1) << "  ------------------------\n"
               << vcl_setw(w1) << "0 GUID_NULL (AUTO)\n";

      for (unsigned int i = 0; i < count; i++)
      {
        vcl_cout << vcl_setw(w1) << i+1 << ' '
                 << vidl2_dshow::get_guid_name(table[i]) << '\n';
      }

      CoTaskMemFree(table);
    }
  }

  void print_capture_rate_help(const CComPtr<IESFProperties>& props)
  {
    double val, min, max, def;
    props->GetCaptureRate(&val);
    props->GetCaptureRateRange(&min, &max, &def);
    vcl_cout << vcl_setw(w1) << "capture_rate"
             << vcl_setw(w2) << val
             << vcl_setw(w2) << min
             << vcl_setw(w2) << max
             << vcl_setw(w2) << def << '\n';
  }

  void print_brightness_help(const CComPtr<IESFProperties>& props)
  {
    long val, min, max, def;
    props->GetBrightness(&val);
    props->GetBrightnessRange(&min, &max, &def);
    print_5_column_row("brightness", val, min, max, def);
  }

  void print_contrast_help(const CComPtr<IESFProperties>& props)
  {
    long val, min, max, def;
    props->GetContrast(&val);
    props->GetContrastRange(&min, &max, &def);
    print_5_column_row("contrast", val, min, max, def);
  }

  void print_saturation_help(const CComPtr<IESFProperties>& props)
  {
    long val, min, max, def;
    props->GetSaturation(&val);
    props->GetSaturationRange(&min, &max, &def);
    print_5_column_row("saturation", val, min, max, def);
  }

  void print_board_help(const CComPtr<IESFProperties>& props)
  {
    long val, min, max;

    props->GetBoard(&val);
    props->GetBoardRange(&min, &max);
    print_4_column_row("(N/A) board", val, min, max);
  }

  void print_connector_help(const CComPtr<IESFProperties>& props)
  {
    long val, min, max;
    props->GetConnector(&val);
    props->GetConnectorRange(&min, &max);
    print_4_column_row("(N/A) connector", val, min, max);
  }

  void print_individual_control_help(const CComPtr<IESFProperties>& props)
  {
    bool val;

    props->GetIndividualControl(&val);
    vcl_cout << vcl_setw(w1) << "(N/S) individual_control"
             << vcl_setw(w2) << (val ? "true" : "false") << '\n';
  }

  void print_bitrate_control_help(const CComPtr<IESFCompression>& props)
  {
    ESF_BITRATECONTROL val;

    vcl_cout << vcl_setw(w1) << "bitrate_control ";
    if (SUCCEEDED(props->GetBitrateControl(&val)))
    {
      vcl_cout << esf_bitratecontrols[val];
    }
    vcl_cout << '\n';

    vcl_cout << vcl_setw(w1) << "  Supported bitrate controls\n"
             << vcl_setw(w1) << "  --------------------------\n";
    for (int i = 0; i < sizeof(esf_bitratecontrols) / sizeof(char*); i++)
    {
      vcl_cout << vcl_setw(w1) << i << ' '
               << esf_bitratecontrols[i] << '\n';
    }
  }

  void print_average_bitrate_help(const CComPtr<IESFCompression>& props)
  {
    long val, min, max, def;
    if (SUCCEEDED(props->GetAverageBitrate(&val)) &&
        SUCCEEDED(props->GetAverageBitrateRange(&min, &max, &def)))
    {
      print_5_column_row("average_bitrate", val, min, max, def);
    }
    else { vcl_cout << vcl_setw(w1) << "average_bitrate\n"; }
  }

  void print_video_quality_help(const CComPtr<IESFCompression>& props)
  {
    long val, min, max, def;
    if (SUCCEEDED(props->GetVideoQuality(&val)) &&
        SUCCEEDED(props->GetVideoQualityRange(&min, &max, &def)))
    {
      print_5_column_row("video_quality", val, min, max, def);
    }
    else { vcl_cout << vcl_setw(w1) << "video_quality\n"; }
  }

  void print_gop_size_help(const CComPtr<IESFCompression>& props)
  {
    long val, min, max, def;
    if (SUCCEEDED(props->GetGopSize(&val)) &&
        SUCCEEDED(props->GetGopSizeRange(&min, &max, &def)))
    {
      print_5_column_row("gop_size", val, min, max, def);
    }
    else { vcl_cout << vcl_setw(w1) << "gop_size\n"; }
  }

  void print_gop_structure_help(const CComPtr<IESFCompression>& props)
  {
    ESF_GOPSTRUCTURE val;

    vcl_cout << vcl_setw(w1) << "gop_structure ";
    if (SUCCEEDED(props->GetGopStructure(&val)))
    {
      vcl_cout << esf_gopstructures[val];
    }
    vcl_cout << '\n';

    vcl_cout << vcl_setw(w1) << "  Supported GOP structures\n"
             << vcl_setw(w1) << "  ------------------------\n";
    for (int i = 0; i < sizeof(esf_gopstructures) / sizeof(char*); i++)
    {
      vcl_cout << vcl_setw(w1) << i << ' '
               << esf_gopstructures[i] << '\n';
    }
  }

  template <typename T> struct from_string_to
  {
    T operator()(const vcl_string& str) const
    {
      T val;

      vcl_istringstream iss(str);
      iss >> val;

      return val;
    }
  };

  template <> struct from_string_to<bool>
  {
    bool operator()(const vcl_string& str) const
    {
      if      (str == "t" || str == "true"  || str == "1")
      {
        return true;
      }
      else if (str == "f" || str == "false" || str == "0")
      {
        return false;
      }
      else
      {
        vidl2_exception_error(vidl2_dshow_exception(
          "can't convert boolean param string to bool."
          + DSHOW_GET_ERROR_TEXT(E_FAIL)));

        return false; // all paths must return...
      }
    }
  };

  template <> struct from_string_to<ESF_RESOLUTION>
  {
    ESF_RESOLUTION operator()(const vcl_string& str) const
    {
      if      (str == "ESF_RESOLUTION_FRAME")
      {
        return ESF_RESOLUTION_FRAME;
      }
      else if (str == "ESF_RESOLUTION_FIELD")
      {
        return ESF_RESOLUTION_FIELD;
      }
      else if (str == "ESF_RESOLUTION_CIF")
      {
        return ESF_RESOLUTION_CIF;
      }
      else if (str == "ESF_RESOLUTION_QCIF")
      {
        return ESF_RESOLUTION_QCIF;
      }
      else if (str == "ESF_RESOLUTION_CUSTOM")
      {
        return ESF_RESOLUTION_CUSTOM;
      }
      else
      {
        vidl2_exception_error(vidl2_dshow_exception(
          "can't convert resolution param string to ESF_RESOLUTION."
          + DSHOW_GET_ERROR_TEXT(E_FAIL)));

        return ESF_RESOLUTION_FRAME; // all paths must return...
      }
    }
  };

  template <> struct from_string_to<ESF_VIDEO_STANDARD>
  {
    ESF_VIDEO_STANDARD operator()(const vcl_string& str) const
    {
      if      (str == "ESF_STANDARD_PAL" || str == "PAL")
      {
        return ESF_STANDARD_PAL;
      }
      else if (str == "ESF_STANDARD_NTSC" || str == "NTSC")
      {
        return ESF_STANDARD_NTSC;
      }
      else
      {
        vidl2_exception_error(vidl2_dshow_exception(
          "can't convert video standard param string to ESF_VIDEO_STANDARD."
          + DSHOW_GET_ERROR_TEXT(E_FAIL)));

        return ESF_STANDARD_PAL; // all paths must return...
      }
    }
  };

  template <> struct from_string_to<ESF_BITRATECONTROL>
  {
    ESF_BITRATECONTROL operator()(const vcl_string& str) const
    {
      if      (str == "ESF_BITRATECONTROL_CBR" || str == "CBR")
      {
        return ESF_BITRATECONTROL_CBR;
      }
      else if (str == "ESF_BITRATECONTROL_VBR" || str == "VBR")
      {
        return ESF_BITRATECONTROL_VBR;
      }
      else
      {
        vidl2_exception_error(vidl2_dshow_exception(
          "can't convert bitrate control param string to ESF_BITRATECONTROL."
          + DSHOW_GET_ERROR_TEXT(E_FAIL)));

        return ESF_BITRATECONTROL_CBR; // all paths must return...
      }
    }
  };

  template <> struct from_string_to<ESF_GOPSTRUCTURE>
  {
    ESF_GOPSTRUCTURE operator()(const vcl_string& str) const
    {
      if      (str == "ESF_GOPSTRUCTURE_IONLY" || str == "IONLY")
      {
        return ESF_GOPSTRUCTURE_IONLY;
      }
      else if (str == "ESF_GOPSTRUCTURE_IPONLY" || str == "IPONLY")
      {
        return ESF_GOPSTRUCTURE_IPONLY;
      }
      else if (str == "ESF_GOPSTRUCTURE_IPB" || str == "IPB")
      {
        return ESF_GOPSTRUCTURE_IPB;
      }
      else
      {
        vidl2_exception_error(vidl2_dshow_exception(
          "can't convert gop structure param string to ESF_GOPSTRUCTURE."
          + DSHOW_GET_ERROR_TEXT(E_FAIL)));

        return ESF_GOPSTRUCTURE_IONLY; // all paths must return...
      }
    }
  };

  template <> struct from_string_to<GUID>
  {
    GUID operator()(const vcl_string& str) const
    {
      if      (str == "MEDIASUBTYPE_DX50" || str == "DX50")
      {
        return vidl2_dshow::get_guid_from_fourcc("DX50");
      }
      else if (str == "WMMEDIASUBTYPE_MP4S" || str == "MP4S")
      {
        return vidl2_dshow::get_guid_from_fourcc("MP4S");
      }
      else if (str == "MEDIASUBTYPE_MJPG" || str == "MJPG")
      {
        return MEDIASUBTYPE_MJPG;
      }
      else if (str == "MEDIASUBTYPE_YUY2" || str == "YUY2")
      {
        return MEDIASUBTYPE_YUY2;
      }
      else if (str == "MEDIASUBTYPE_YV12" || str == "YV12")
      {
        return MEDIASUBTYPE_YV12;
      }
      else if (str == "MEDIASUBTYPE_IYUV" || str == "IYUV")
      {
        return MEDIASUBTYPE_IYUV;
      }
      else if (str == "MEDIASUBTYPE_Y41P" || str == "Y41P")
      {
        return MEDIASUBTYPE_Y41P;
      }
      else if (str == "MEDIASUBTYPE_YVU9" || str == "YVU9")
      {
        return MEDIASUBTYPE_YVU9;
      }
      else if (str == "MEDIASUBTYPE_RGB24" || str == "RGB24")
      {
        return MEDIASUBTYPE_RGB24;
      }
      else if (str == "MEDIASUBTYPE_RGB555" || str == "RGB555")
      {
        return MEDIASUBTYPE_RGB555;
      }
      else if (str == "MEDIASUBTYPE_RGB565" || str == "RGB565")
      {
        return MEDIASUBTYPE_RGB565;
      }
      else if (str == "MEDIASUBTYPE_RGB8" || str == "RGB8")
      {
        return MEDIASUBTYPE_RGB8;
      }
      else if (str == "MEDIASUBTYPE_ARGB32" || str == "ARGB32")
      {
        return MEDIASUBTYPE_ARGB32;
      }
      else if (str == "GUID_NULL" || str == "AUTO")
      {
        return GUID_NULL;
      }
      else
      {
        vidl2_exception_error(vidl2_dshow_exception(
          "can't convert output format param string to GUID."
          + DSHOW_GET_ERROR_TEXT(E_FAIL)));

        return GUID_NULL; // all paths must return...
      }
    }
  };

  template <typename T1, typename T2>
  struct from_string_to_pair
  {
    vcl_pair<T1,T2> operator()(const vcl_string& str) const
    {
      vcl_string buf;
      vcl_istringstream iss(str);

      vcl_pair<T1,T2> pair;

      iss >> buf;
      pair.first = from_string_to<T1>()(buf);

      iss >> buf;
      pair.second = from_string_to<T2>()(buf);

      return pair;
    }
  };

  template <typename T1, typename T2, typename T3>
  struct from_string_to_triple
  {
    vbl_triple<T1,T2,T3> operator()(const vcl_string& str) const
    {
      vcl_string buf;
      vcl_istringstream iss(str);

      vbl_triple<T1,T2,T3> triple;

      iss >> buf;
      triple.first = from_string_to<T1>()(buf);

      iss >> buf;
      triple.second = from_string_to<T2>()(buf);

      iss >> buf;
      triple.third = from_string_to<T3>()(buf);

      return triple;
    }
  };
} // unnamed namespace

//-------------------------------------------------------------------------
// Implementation.
//-------------------------------------------------------------------------
//: Constructor - default
vidl2_dshow_istream_params_esf::vidl2_dshow_istream_params_esf(void)
  : register_in_rot_(false)
{}

//: Configure the filter based on the parameters.
void vidl2_dshow_istream_params_esf
::configure_filter(const CComPtr<IBaseFilter>& source)
{
  CComPtr<IESFProperties> esf_properties;
  source->QueryInterface(
    IID_IESFProperties, reinterpret_cast<void**>(&esf_properties));

  //if (is_property_changed("video_standard"))
  if (is_property_changed_.test(esf_property_video_standard))
  {
    DSHOW_ERROR_IF_FAILED(
      esf_properties->SetVideoStandard(video_standard_, is_monochrome_));
  }

  //if (is_property_changed("resolution"))
  if (is_property_changed_.test(esf_property_resolution))
  {
    vcl_cout << resolution_ << width_ << height_ << vcl_endl;
    DSHOW_ERROR_IF_FAILED(
      esf_properties->SetResolution(resolution_, width_, height_));
  }

  if (is_property_changed_.test(esf_property_capture_region_size))
  {
    DSHOW_ERROR_IF_FAILED(
      esf_properties->SetCaptureRegionSize(capture_region_size_x_,
                                           capture_region_size_y_));
  }

  if (is_property_changed_.test(esf_property_capture_region_pos))
  {
    DSHOW_ERROR_IF_FAILED(
      esf_properties->SetCaptureRegionPos(capture_region_pos_x_,
                                          capture_region_pos_y_));
  }

  if (is_property_changed_.test(esf_property_output_format))
  {
    DSHOW_ERROR_IF_FAILED(esf_properties->SetOutputFormat(output_format_));
  }

  if (is_property_changed_.test(esf_property_capture_rate))
  {
    DSHOW_ERROR_IF_FAILED(esf_properties->SetCaptureRate(capture_rate_));
  }

  if (is_property_changed_.test(esf_property_brightness))
  {
    DSHOW_ERROR_IF_FAILED(esf_properties->SetBrightness(brightness_));
  }

  if (is_property_changed_.test(esf_property_contrast))
  {
    DSHOW_ERROR_IF_FAILED(esf_properties->SetContrast(contrast_));
  }

  if (is_property_changed_.test(esf_property_saturation))
  {
    DSHOW_ERROR_IF_FAILED(esf_properties->SetSaturation(saturation_));
  }


  CComPtr<IESFCompression> esf_compression;
  source->QueryInterface(
    IID_IESFCompression, reinterpret_cast<void**>(&esf_compression));

  if (is_property_changed_.test(esf_property_bitrate_control))
  {
    DSHOW_ERROR_IF_FAILED(
      esf_compression->SetBitrateControl(bitrate_control_));
  }

  if (is_property_changed_.test(esf_property_average_bitrate))
  {
    DSHOW_ERROR_IF_FAILED(esf_compression->SetAverageBitrate(average_bitrate_));
  }

  if (is_property_changed_.test(esf_property_video_quality))
  {
    DSHOW_ERROR_IF_FAILED(esf_compression->SetVideoQuality(video_quality_));
  }

  if (is_property_changed_.test(esf_property_gop_structure))
  {
    DSHOW_ERROR_IF_FAILED(esf_compression->SetGopStructure(gop_structure_));
  }

  if (is_property_changed_.test(esf_property_gop_size))
  {
    DSHOW_ERROR_IF_FAILED(esf_compression->SetGopSize(gop_size_));
  }
}

void vidl2_dshow_istream_params_esf
::print_parameter_help(const vcl_string& name)
{
  vidl2_dshow::initialize_com();

  CComPtr<IMoniker> moniker = vidl2_dshow::get_capture_device_moniker(name);

  CComPtr<IBaseFilter> filter;
  DSHOW_ERROR_IF_FAILED(moniker->BindToObject(
    0, 0, IID_IBaseFilter, reinterpret_cast<void**>(&filter)));

  print_parameter_help(filter);
}

void vidl2_dshow_istream_params_esf
::print_parameter_help(const CComPtr<IBaseFilter>& filter)
{
  vcl_cout << "\n\nEuresys MultiCam DirectShow (IESFilter) Parameters\n"
           << "--------------------------------------------------\n"
           << "1. IESFProperties interface:\n\n";

  // IESFProperties interface.
  CComPtr<IESFProperties> esf_properties;
  filter->QueryInterface(
    IID_IESFProperties, reinterpret_cast<void**>(&esf_properties));

  print_video_standard_help     (esf_properties);
  vcl_cout << '\n';

  print_resolution_help         (esf_properties);
  vcl_cout << '\n';

  print_output_format_help      (esf_properties);
  vcl_cout << '\n';

  // 5 column row header
  vcl_cout << vcl_setw(w1) << ' '
           << vcl_setw(w2) << "curr"
           << vcl_setw(w2) << "min"
           << vcl_setw(w2) << "max"
           << vcl_setw(w2) << "default\n"
           << vcl_setw(w1) << ' ';
  char prev = vcl_cout.fill('-');
  vcl_cout << vcl_setw(4*w2) << "-\n";
  vcl_cout.fill(prev);

  print_capture_region_size_help(esf_properties);
  print_capture_region_pos_help (esf_properties);
  print_capture_rate_help       (esf_properties);
  print_brightness_help         (esf_properties);
  print_contrast_help           (esf_properties);
  print_saturation_help         (esf_properties);
  print_board_help              (esf_properties);
  print_connector_help          (esf_properties);
  print_individual_control_help (esf_properties);
  vcl_cout << '\n';

#if 0
  // 4 column row header
  vcl_cout << vcl_setw(w1) << ' '
           << vcl_setw(w2) << "curr"
           << vcl_setw(w2) << "min"
           << vcl_setw(w2) << "max\n"
           << vcl_setw(w1) << ' ';
  prev = vcl_cout.fill('-');
  vcl_cout << vcl_setw(3*w2) << "-\n";
  vcl_cout.fill(prev);
#endif // 0

  // IESFCompression interface.
  CComPtr<IESFCompression> esf_compression;
  filter->QueryInterface(
    IID_IESFCompression, reinterpret_cast<void**>(&esf_compression));

  vcl_cout << "2. IESFCompression interface:\n\n";

  print_bitrate_control_help(esf_compression);
  vcl_cout << '\n';

  // 5 column row header
  vcl_cout << vcl_setw(w1) << ' '
           << vcl_setw(w2) << "curr"
           << vcl_setw(w2) << "min"
           << vcl_setw(w2) << "max"
           << vcl_setw(w2) << "default\n"
           << vcl_setw(w1) << ' ';
  prev = vcl_cout.fill('-');
  vcl_cout << vcl_setw(4*w2) << "-\n";
  vcl_cout.fill(prev);

  print_average_bitrate_help(esf_compression);
  print_video_quality_help  (esf_compression);
  print_gop_size_help       (esf_compression);
  vcl_cout << '\n';

  print_gop_structure_help  (esf_compression);
  vcl_cout << '\n' << vcl_endl;
}

//: Set properties from a map(string,value).
// \sa mul/mbl/mbl_read_props.h
vidl2_dshow_istream_params_esf&
vidl2_dshow_istream_params_esf
::set_properties(const vcl_map<vcl_string,vcl_string>& props)
{
  vcl_map<vcl_string,vcl_string>::const_iterator iter;
  for (iter = props.begin(); iter != props.end(); iter++)
  {
    if      (iter->first == "register_in_rot")
    {
      set_register_in_rot(from_string_to<bool>()(iter->second));
    }
    else if (iter->first == "video_standard")
    {
      vcl_pair<ESF_VIDEO_STANDARD,bool> vs
        = from_string_to_pair<ESF_VIDEO_STANDARD,bool>()(iter->second);
      set_video_standard(vs.first, vs.second);
    }
    else if (iter->first == "resolution")
    {
      vbl_triple<ESF_RESOLUTION,long,long> res
        = from_string_to_triple<ESF_RESOLUTION,long,long>()(iter->second);
      set_resolution(res.first, res.second, res.third);
    }
    else if (iter->first == "capture_region_size")
    {
      vcl_pair<long,long> size
        = from_string_to_pair<long,long>()(iter->second);
      set_capture_region_size(size.first, size.second);
    }
    else if (iter->first == "capture_region_pos")
    {
      vcl_pair<long,long> pos
        = from_string_to_pair<long,long>()(iter->second);
      set_capture_region_pos(pos.first, pos.second);
    }
    else if (iter->first == "output_format")
    {
      set_output_format(from_string_to<GUID>()(iter->second));
    }
    else if (iter->first == "capture_rate")
    {
      set_capture_rate(from_string_to<double>()(iter->second));
    }
    else if (iter->first == "brightness")
    {
      set_brightness(from_string_to<long>()(iter->second));
    }
    else if (iter->first == "contrast")
    {
      set_contrast(from_string_to<long>()(iter->second));
    }
    else if (iter->first == "saturation")
    {
      set_saturation(from_string_to<long>()(iter->second));
    }
    else if (iter->first == "bitrate_control")
    {
      set_bitrate_control(from_string_to<ESF_BITRATECONTROL>()(iter->second));
    }
    else if (iter->first == "average_bitrate")
    {
      set_average_bitrate(from_string_to<long>()(iter->second));
    }
    else if (iter->first == "video_quality")
    {
      set_video_quality(from_string_to<long>()(iter->second));
    }
    else if (iter->first == "gop_structure")
    {
      set_gop_structure(from_string_to<ESF_GOPSTRUCTURE>()(iter->second));
    }
    else if (iter->first == "gop_size")
    {
      set_gop_size(from_string_to<long>()(iter->second));
    }
    else
    {
      vcl_cerr << "DSHOW: vidl2_dshow_istream_params_esf param not valid: "
               << iter->first << vcl_endl;
    }
  }

  return *this;
}

//-------------------------------------------------------------------------
// Read accessor functions.
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
// Write accessor functions.
//-------------------------------------------------------------------------
/* inline */ vidl2_dshow_istream_params_esf&
vidl2_dshow_istream_params_esf::set_device_name(const vcl_string& name)
{
  device_name_ = name;
  return *this;
}

/* inline */ vidl2_dshow_istream_params_esf&
vidl2_dshow_istream_params_esf::set_register_in_rot(bool val)
{
  register_in_rot_ = val;
  return *this;
}

/* inline */ vidl2_dshow_istream_params_esf&
vidl2_dshow_istream_params_esf::set_video_standard(
  ESF_VIDEO_STANDARD val, bool mono)
{
  video_standard_ = val;
  is_monochrome_ = mono;
  is_property_changed_.set(esf_property_video_standard);
  return *this;
}

/* inline */ vidl2_dshow_istream_params_esf&
vidl2_dshow_istream_params_esf::set_resolution(
  ESF_RESOLUTION val, long width = 0, long height = 0)
{
  resolution_ = val;
  width_  = width;
  height_ = height;
  is_property_changed_.set(esf_property_resolution);
  return *this;
}

/* inline */ vidl2_dshow_istream_params_esf&
vidl2_dshow_istream_params_esf::set_capture_region_size(long val_x, long val_y)
{
  capture_region_size_x_ = val_x;
  capture_region_size_y_ = val_y;
  is_property_changed_.set(esf_property_capture_region_size);
  return *this;
}

/* inline */ vidl2_dshow_istream_params_esf&
vidl2_dshow_istream_params_esf::set_capture_region_pos(long val_x, long val_y)
{
  capture_region_pos_x_ = val_x;
  capture_region_pos_y_ = val_y;
  is_property_changed_.set(esf_property_capture_region_pos);
  return *this;
}

/* inline */ vidl2_dshow_istream_params_esf&
vidl2_dshow_istream_params_esf::set_output_format(GUID val)
{
  output_format_ = val;
  is_property_changed_.set(esf_property_output_format);
  return *this;
}

/* inline */ vidl2_dshow_istream_params_esf&
vidl2_dshow_istream_params_esf::set_capture_rate(double val)
{
  capture_rate_ = val;
  is_property_changed_.set(esf_property_capture_rate);
  return *this;
}

/* inline */ vidl2_dshow_istream_params_esf&
vidl2_dshow_istream_params_esf::set_brightness(long val)
{
  brightness_ = val;
  is_property_changed_.set(esf_property_brightness);
  return *this;
}
/* inline */ vidl2_dshow_istream_params_esf&
vidl2_dshow_istream_params_esf::set_contrast(long val)
{
  contrast_ = val;
  is_property_changed_.set(esf_property_contrast);
  return *this;
}

/* inline */ vidl2_dshow_istream_params_esf&
vidl2_dshow_istream_params_esf::set_saturation(long val)
{
  saturation_ = val;
  is_property_changed_.set(esf_property_saturation);
  return *this;
}

/* inline */ vidl2_dshow_istream_params_esf&
vidl2_dshow_istream_params_esf::set_bitrate_control(ESF_BITRATECONTROL val)
{
  bitrate_control_ = val;
  is_property_changed_.set(esf_property_bitrate_control);
  return *this;
}

/* inline */ vidl2_dshow_istream_params_esf&
vidl2_dshow_istream_params_esf::set_average_bitrate(long val)
{
  average_bitrate_ = val;
  is_property_changed_.set(esf_property_average_bitrate);
  return *this;
}

/* inline */ vidl2_dshow_istream_params_esf&
vidl2_dshow_istream_params_esf::set_video_quality(long val)
{
  video_quality_ = val;
  is_property_changed_.set(esf_property_video_quality);
  return *this;
}

/* inline */ vidl2_dshow_istream_params_esf&
vidl2_dshow_istream_params_esf::set_gop_structure(ESF_GOPSTRUCTURE val)
{
  gop_structure_ = val;
  is_property_changed_.set(esf_property_gop_structure);
  return *this;
}

/* inline */ vidl2_dshow_istream_params_esf&
vidl2_dshow_istream_params_esf::set_gop_size(long val)
{
  gop_size_ = val;
  is_property_changed_.set(esf_property_gop_size);
  return *this;
}
