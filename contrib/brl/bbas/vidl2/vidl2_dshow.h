// This is brl/bbas/vidl2/vidl2_dshow.h
#ifndef vidl2_dshow_h_
#define vidl2_dshow_h_
//=========================================================================
//:
// \file
// \brief  DirectShow helper functions used in vidl2.
// \author Miguel A. Figueroa-Villanueva (miguelf at ieee dot org)
//
// \verbatim
//  Modifications
//   02/03/2006 - File created. (miguelfv)
//   03/07/2006 - File imported to vxl repository. (miguelfv)
// \endverbatim
//
//=========================================================================

#include <vidl2/vidl2_exception.h>

#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_iosfwd.h>

#include <atlbase.h>
#include <dshow.h>
#include <qedit.h>

enum vidl2_pixel_format;

struct vidl2_dshow
{
  //: Initialize COM (must be called before using any of these functions).
  static void initialize_com();

  //: Get an error description for the given HRESULT.
  static vcl_string get_error_text(const char* file, int line, HRESULT hr);

  //: Register object in Running Object Table (ROT)
  static void register_in_rot(IUnknown* unknown, DWORD& reg);

  //: Remove object from Running Object Table (ROT)
  static void remove_from_rot(DWORD reg);

  //: Save filter graph to a *.grf file.
  static void save_graph_to_file(const CComPtr<IFilterGraph2>& filter_graph,
                                 const vcl_string& filename);

  //: Load filter graph from a *.grf file.
  static void load_graph_from_file(const CComPtr<IFilterGraph2>& filter_graph,
                                   const vcl_wstring& filename);

  //: Get GUID name or FOURCC.
  static vcl_string get_guid_name(const GUID& guid);

  //: Get multimedia subtype GUID from FOURCC.
  static GUID get_guid_from_fourcc(const vcl_string& fourcc);

  //: Extract information from AM_MEDIA_TYPE object.
  static void get_media_info(const AM_MEDIA_TYPE& amt,
                             unsigned int& width,
                             unsigned int& height,
                             vidl2_pixel_format& pixel_format);

  //: Delete AM_MEDIA_TYPE memory.
  static void delete_media_type(AM_MEDIA_TYPE& amt);

  //: Print a list of capture device names (i.e., FriendlyName).
  static void print_capture_device_names();

  //: Get a list of capture device names (i.e., FriendlyName).
  static vcl_vector<vcl_string> get_capture_device_names();

  //: Get IMoniker associated with name.
  static CComPtr<IMoniker> get_capture_device_moniker(const vcl_string& name);

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
  vidl2_dshow::get_error_text(__FILE__,__LINE__,(hr))

#define DSHOW_WARN_IF_FAILED(stm) {                                       \
  HRESULT hr = (stm);                                                     \
  if FAILED(hr) {                                                         \
    vcl_cerr << vidl2_dshow_exception(                                    \
                vidl2_dshow::get_error_text(__FILE__,__LINE__,hr)).what() \
             << vcl_endl;                                                 \
  }}

#define DSHOW_ERROR_IF_FAILED(stm) {                         \
  HRESULT hr = (stm);                                        \
  if FAILED(hr) {                                            \
    vidl2_exception_error(vidl2_dshow_exception(             \
      vidl2_dshow::get_error_text(__FILE__,__LINE__,hr)));   \
  }}

#endif // vidl2_dshow_h_
