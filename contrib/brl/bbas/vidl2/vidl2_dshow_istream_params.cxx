// This is brl/bbas/vidl2/vidl2_dshow_istream_params.cxx
//=========================================================================
//:
// \file
// \brief  DirectShow input stream parameter block.
//
// See vidl2_dshow_istream_params.h for details.
//
//=========================================================================

#include <vidl2/vidl2_dshow_istream_params.h>
#include <vidl2/vidl2_dshow.h>

#include <vcl_iostream.h>
#include <vcl_iomanip.h>
#include <vcl_sstream.h>
#include <vcl_cassert.h>
#include <vcl_utility.h>

//-------------------------------------------------------------------------
// Private helpers.
//-------------------------------------------------------------------------
namespace
{
  // widths for formatting help (iomanip setw)
  const int w1 = 25;
  const int w2 = 8;

  vcl_map<vcl_string,vpa_property_wrap> vpa_properties(void)
  {
    typedef vcl_pair<vcl_string,vpa_property_wrap> pair;

    vcl_map<vcl_string,vpa_property_wrap> properties;

    // IAMVideoProcAmp interface
    properties.insert(pair("brightness",             vpa_property_wrap(VideoProcAmp_Brightness)           ));
    properties.insert(pair("contrast",               vpa_property_wrap(VideoProcAmp_Contrast)             ));
    properties.insert(pair("hue",                    vpa_property_wrap(VideoProcAmp_Hue)                  ));
    properties.insert(pair("saturation",             vpa_property_wrap(VideoProcAmp_Saturation)           ));
    properties.insert(pair("sharpness",              vpa_property_wrap(VideoProcAmp_Sharpness)            ));
    properties.insert(pair("gamma",                  vpa_property_wrap(VideoProcAmp_Gamma)                ));
    properties.insert(pair("color_enable",           vpa_property_wrap(VideoProcAmp_ColorEnable)          ));
    properties.insert(pair("white_balance",          vpa_property_wrap(VideoProcAmp_WhiteBalance)         ));
    properties.insert(pair("backlight_compensation", vpa_property_wrap(VideoProcAmp_BacklightCompensation)));
    properties.insert(pair("gain",                   vpa_property_wrap(VideoProcAmp_Gain)                 ));

    return properties;
  }

  void print_help(const CComPtr<IAMVideoProcAmp>& am_video_proc_amp,
                  const vcl_string& prop_tag)
  {
    vcl_map<vcl_string,vpa_property_wrap> prop = vpa_properties();
    long val, min, max, step, def, flag;

    vcl_cout << vcl_setw(w1) << prop_tag;
    if (SUCCEEDED(am_video_proc_amp->GetRange(prop[prop_tag].key,
                                              &min,
                                              &max,
                                              &step,
                                              &def,
                                              &flag)))
    {
      am_video_proc_amp->Get(prop[prop_tag].key, &val, &flag);
      vcl_cout << vcl_setw(w2) << val
               << vcl_setw(w2) << min
               << vcl_setw(w2) << max
               << vcl_setw(w2) << step
               << vcl_setw(w2) << def
               << vcl_setw(w2)
               << (flag == 0x0001
                   ? "auto"
                   : flag == 0x0002
                     ? "manual"
                     : "error")
               << '\n';
    }
    else
    {
      vcl_cout << " ...Not Supported...\n";
    }
  }

  void print_output_format_help(const CComPtr<IAMStreamConfig>& asc)
  //void print_output_format_help(IAMStreamConfig* asc)
  {
    AM_MEDIA_TYPE* amt = 0;
    VIDEO_STREAM_CONFIG_CAPS* vscc = reinterpret_cast<VIDEO_STREAM_CONFIG_CAPS*>(new BYTE[128]);

    int count = 0, size = 0;
    DSHOW_ERROR_IF_FAILED(asc->GetNumberOfCapabilities(&count, &size));
    //assert(sizeof(VIDEO_STREAM_CONFIG_CAPS) == size);

    DSHOW_ERROR_IF_FAILED(asc->GetFormat(&amt));
    vcl_cout << vcl_setw(w1) << "output_format"
             << ' ' << vidl2_dshow::get_guid_name(amt->subtype) << '\n';
    vidl2_dshow::delete_media_type(*amt);
    amt = 0;

    vcl_cout << vcl_string(w1, ' ') << " Supported output formats\n"
             << vcl_string(w1, ' ') << " ------------------------\n";
    for (int i = 0; i < count; i++)
    {
      DSHOW_ERROR_IF_FAILED(
        asc->GetStreamCaps(i, &amt, reinterpret_cast<BYTE*>(vscc)));

      double max_frame_interval = static_cast<double>(vscc->MaxFrameInterval);
      double min_frame_interval = static_cast<double>(vscc->MinFrameInterval);
      vcl_cout << vcl_setw(w1) << i
               << vcl_setw(3*w2)
               << vidl2_dshow::get_guid_name(amt->subtype)
               << " ("
               << vcl_setw(4) << vscc->InputSize.cx << 'x'
               << vcl_setw(4) << vscc->InputSize.cy
               << ") "
               << vcl_setprecision(2) << 1.0 / (max_frame_interval * 100.0e-9) << '-'
               << vcl_setprecision(2) << 1.0 / (min_frame_interval * 100.0e-9) << " fps\n";

      vidl2_dshow::delete_media_type(*amt);
      amt = 0;
    }

    delete vscc;
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
} // unnamed namespace

//-------------------------------------------------------------------------
// Implementation.
//-------------------------------------------------------------------------
//: Constructor - default
vidl2_dshow_istream_params::vidl2_dshow_istream_params(void)
  : vpa_properties_(vpa_properties())
  , register_in_rot_(false)
  , run_when_ready_(true)
  , load_filter_defaults_(false)
{}

//: Configure the source filter based on the parameters.
void vidl2_dshow_istream_params::configure_filter(
  const CComPtr<IBaseFilter>& source)
{
  // IAMVideoProcAmp interface.
  CComPtr<IAMVideoProcAmp> am_video_proc_amp;
  DSHOW_ERROR_IF_FAILED(source->QueryInterface(
    IID_IAMVideoProcAmp, reinterpret_cast<void**>(&am_video_proc_amp)));

  vcl_map<vcl_string,vpa_property_wrap>::const_iterator iter;
  for (iter = vpa_properties_.begin(); iter != vpa_properties_.end(); iter++)
  {
    if (iter->second.is_changed)
    {
      DSHOW_ERROR_IF_FAILED(am_video_proc_amp->Set(
        iter->second.key, iter->second.value, VideoProcAmp_Flags_Manual));
    }
    else if (load_filter_defaults_)
    {
      long min, max, step, def, flag;
      if (SUCCEEDED(am_video_proc_amp->GetRange(iter->second.key,
                    &min, &max, &step, &def, &flag)))
      {
        DSHOW_ERROR_IF_FAILED(
          am_video_proc_amp->Set(iter->second.key, def, flag));
      }
    }
  }

  // IAMStreamConfig interface.
  if (output_format_.is_changed || load_filter_defaults_)
  {
    CComPtr<ICaptureGraphBuilder2> graph_builder;
    DSHOW_ERROR_IF_FAILED(
      graph_builder.CoCreateInstance(CLSID_CaptureGraphBuilder2));
    CComPtr<IAMStreamConfig> am_stream_config;

    DSHOW_ERROR_IF_FAILED(graph_builder->FindInterface(
      &PIN_CATEGORY_CAPTURE,
      &MEDIATYPE_Video,
      source,
      IID_IAMStreamConfig,
      reinterpret_cast<void**>(&am_stream_config)));

    AM_MEDIA_TYPE* amt = 0;
    VIDEO_STREAM_CONFIG_CAPS vscc;

    int count = 0, size = 0;
    DSHOW_ERROR_IF_FAILED(
      am_stream_config->GetNumberOfCapabilities(&count, &size));
    assert(sizeof(vscc) == size);

    DSHOW_ERROR_IF_FAILED(am_stream_config->GetStreamCaps(
      output_format_.is_changed ? output_format_.value : 0,
      &amt, reinterpret_cast<BYTE*>(&vscc)));

    DSHOW_ERROR_IF_FAILED(am_stream_config->SetFormat(amt));

    vidl2_dshow::delete_media_type(*amt);
  }
}

void vidl2_dshow_istream_params::print_parameter_help(const vcl_string& name)
{
  vidl2_dshow::initialize_com();

  CComPtr<IMoniker> moniker = vidl2_dshow::get_capture_device_moniker(name);

  CComPtr<IBaseFilter> filter;
  DSHOW_ERROR_IF_FAILED(moniker->BindToObject(
    0, 0, IID_IBaseFilter, reinterpret_cast<void**>(&filter)));

  print_parameter_help(filter);
}

void vidl2_dshow_istream_params::print_parameter_help(const CComPtr<IBaseFilter>& filter)
{
  vcl_cout << "\n\nDirectShow Parameters\n"
           << "---------------------\n"
           << "1. IAMVideoProcAmp interface:\n\n";

  // IAMVideoProcAmp interface.
  CComPtr<IAMVideoProcAmp> am_video_proc_amp;
  DSHOW_ERROR_IF_FAILED(filter->QueryInterface(
    IID_IAMVideoProcAmp, reinterpret_cast<void**>(&am_video_proc_amp)));

  vcl_cout << vcl_string(w1, ' ')
           << vcl_setw(w2) << "curr"
           << vcl_setw(w2) << "min"
           << vcl_setw(w2) << "max"
           << vcl_setw(w2) << "step"
           << vcl_setw(w2) << "default"
           << vcl_setw(w2) << "flags"
           << '\n'
           << vcl_string(w1, ' ')
           << vcl_string(6*w2, '-') << '\n';

  print_help(am_video_proc_amp, "brightness"            );
  print_help(am_video_proc_amp, "contrast"              );
  print_help(am_video_proc_amp, "hue"                   );
  print_help(am_video_proc_amp, "saturation"            );
  print_help(am_video_proc_amp, "sharpness"             );
  print_help(am_video_proc_amp, "gamma"                 );
  print_help(am_video_proc_amp, "color_enable"          );
  print_help(am_video_proc_amp, "white_balance"         );
  print_help(am_video_proc_amp, "backlight_compensation");
  print_help(am_video_proc_amp, "gain"                  );
  vcl_cout << '\n';

  // IAMStreamConfig interface.
  CComPtr<ICaptureGraphBuilder2> graph_builder;
  DSHOW_ERROR_IF_FAILED(
    graph_builder.CoCreateInstance(CLSID_CaptureGraphBuilder2));
  CComPtr<IAMStreamConfig> am_stream_config;

  vcl_cout << "\n2.1 IAMStreamConfig interface (Capture Pin):\n\n";

  if (SUCCEEDED(graph_builder->FindInterface(
    &PIN_CATEGORY_CAPTURE,
    &MEDIATYPE_Video,
    filter,
    IID_IAMStreamConfig,
    reinterpret_cast<void**>(&am_stream_config))))
  {
    print_output_format_help(am_stream_config);
  }
  else { vcl_cout << "...Not Supported...\n"; }

  vcl_cout << "\n2.2 IAMStreamConfig interface (Preview Pin):\n\n";

  am_stream_config.Release();
  if (SUCCEEDED(graph_builder->FindInterface(
    &PIN_CATEGORY_PREVIEW,
    &MEDIATYPE_Video,
    filter,
    IID_IAMStreamConfig,
    reinterpret_cast<void**>(&am_stream_config))))
  {
    print_output_format_help(am_stream_config);
  }
  else { vcl_cout << "...Not Supported...\n"; }

  vcl_cout << '\n' << vcl_endl;
}

//: Set properties from a map(string,value).
// \sa mul/mbl/mbl_read_props.h
vidl2_dshow_istream_params&
vidl2_dshow_istream_params
::set_properties(const vcl_map<vcl_string,vcl_string>& props)
{
  vcl_map<vcl_string,vcl_string>::const_iterator iter;
  for (iter = props.begin(); iter != props.end(); iter++)
  {
    vcl_map<vcl_string,vpa_property_wrap>::iterator property
      = vpa_properties_.find(iter->first);
    if (property != vpa_properties_.end())
    {
      property->second.value = from_string_to<long>()(iter->second);
      property->second.is_changed = true;
    }
    else
    {
      if (iter->first == "output_format")
      {
        set_output_format(from_string_to<int>()(iter->second));
      }
      else if (iter->first == "register_in_rot")
      {
        set_register_in_rot(from_string_to<bool>()(iter->second));
      }
      else if (iter->first == "run_when_ready")
      {
        set_run_when_ready(from_string_to<bool>()(iter->second));
      }
      else if (iter->first == "device_name")
      {
        set_device_name(iter->second);
      }
      else if (iter->first == "output_filename")
      {
        set_output_filename(iter->second);
      }
      else if (iter->first == "target_output_format")
      {
        vidl2_exception_error(
          vidl2_exception("Target output format not supported yet!"));
        //set_target_output_format(from_string_to<GUID>()(iter->second));
      }
      else if (iter->first == "load_filter_defaults")
      {
        set_load_filter_defaults(from_string_to<bool>()(iter->second));
      }
      else
      {
        vcl_cerr << "DSHOW: vidl2_dshow_istream_params param not valid: "
                 << iter->first << vcl_endl;
      }
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
/* inline */ vidl2_dshow_istream_params&
vidl2_dshow_istream_params::set_register_in_rot(bool val)
{
  register_in_rot_ = val;
  return *this;
}

/* inline */ vidl2_dshow_istream_params&
vidl2_dshow_istream_params::set_run_when_ready(bool val)
{
  run_when_ready_ = val;
  return *this;
}

/* inline */ vidl2_dshow_istream_params&
vidl2_dshow_istream_params::set_device_name(const vcl_string& name)
{
  device_name_ = name;
  return *this;
}

/* inline */ vidl2_dshow_istream_params&
vidl2_dshow_istream_params::set_output_filename(const vcl_string& name)
{
  output_filename_ = name;
  return *this;
}

/* inline */ vidl2_dshow_istream_params&
vidl2_dshow_istream_params::set_target_output_format(GUID val)
{
  target_output_format_ = val;
  return *this;
}

/* inline */ vidl2_dshow_istream_params&
vidl2_dshow_istream_params::set_load_filter_defaults(bool val)
{
  load_filter_defaults_ = val;
  return *this;
}

/* inline */ vidl2_dshow_istream_params&
vidl2_dshow_istream_params::set_output_format(int val)
{
  output_format_.value = val;
  output_format_.is_changed = true;
  return *this;
}
