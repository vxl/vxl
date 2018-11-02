// This is core/vidl/vidl_dshow.h
#ifndef vidl_dshow_h_
#define vidl_dshow_h_
//=========================================================================
//:
// \file
// \brief  DirectShow helper functions used in vidl.
// \author Miguel A. Figueroa-Villanueva (miguelf at ieee dot org)
//
// \verbatim
//  Modifications
//   02/03/2006 - File created. (miguelfv)
//   03/07/2006 - File imported to vxl repository. (miguelfv)
// \endverbatim
//
//=========================================================================

#include <string>
#include <vector>
#include <iostream>
#include <vidl/vidl_exception.h>
#include <vidl/vidl_pixel_format.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <atlbase.h>
#include <dshow.h>
#include <qedit.h>

struct vidl_dshow
{
  //: Initialize COM (must be called before using any of these functions).
  static void initialize_com();

  //: Get an error description for the given HRESULT.
  static std::string get_error_text(const char* file, int line, HRESULT hr);

  //: Register object in Running Object Table (ROT)
  static void register_in_rot(IUnknown* unknown, DWORD& reg);

  //: Remove object from Running Object Table (ROT)
  static void remove_from_rot(DWORD reg);

  //: Save filter graph to a *.grf file.
  static void save_graph_to_file(const CComPtr<IFilterGraph2>& filter_graph,
                                 const std::string& filename);

  //: Load filter graph from a *.grf file.
  static void load_graph_from_file(const CComPtr<IFilterGraph2>& filter_graph,
                                   const std::wstring& filename);

  //: Get GUID name or FOURCC.
  static std::string get_guid_name(const GUID& guid);

  //: Get multimedia subtype GUID from FOURCC.
  static GUID get_guid_from_fourcc(const std::string& fourcc);

  //: Extract information from AM_MEDIA_TYPE object.
  static void get_media_info(const AM_MEDIA_TYPE& amt,
                             unsigned int& width,
                             unsigned int& height,
                             vidl_pixel_format& pixel_format);

  //: Delete AM_MEDIA_TYPE memory.
  static void delete_media_type(AM_MEDIA_TYPE& amt);

  //: Print a list of capture device names (i.e., FriendlyName).
  static void print_capture_device_names();

  //: Get a list of capture device names (i.e., FriendlyName).
  static std::vector<std::string> get_capture_device_names();

  //: Get IMoniker associated with name.
  static CComPtr<IMoniker> get_capture_device_moniker(const std::string& name);

#if 0
  static void build_filter_graph(/*CComPtr<ICaptureGraphBuilder2>& graph_builder,*/
                                 CComPtr<IFilterGraph2>& filter_graph,
                                 CComPtr<IMoniker>& moniker,
                                 CComPtr<ISampleGrabber>& sample_grabber);
#endif

  static void connect_filters(CComPtr<IFilterGraph2>& filter_graph,
                              CComPtr<IBaseFilter>& source,
                              CComPtr<IBaseFilter>& target);
};

// Some macros to help with error reporting.
#define DSHOW_GET_ERROR_TEXT(hr) \
  vidl_dshow::get_error_text(__FILE__,__LINE__,(hr))

#define DSHOW_WARN_IF_FAILED(stm) {                                       \
  HRESULT hr = (stm);                                                     \
  if FAILED(hr) {                                                         \
    std::cerr << vidl_dshow_exception(                                     \
                  vidl_dshow::get_error_text(__FILE__,__LINE__,hr)).what()\
             << '\n';                                                     \
  }}

#define DSHOW_ERROR_IF_FAILED(stm) {                     \
  HRESULT hr = (stm);                                    \
  if FAILED(hr) {                                        \
    vidl_exception_error(vidl_dshow_exception(           \
      vidl_dshow::get_error_text(__FILE__,__LINE__,hr)));\
  }}

#endif // vidl_dshow_h_
