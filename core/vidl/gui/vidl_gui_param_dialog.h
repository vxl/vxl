// This is core/vidl/gui/vidl_gui_param_dialog.h
#ifndef vidl_gui_param_dialog_h_
#define vidl_gui_param_dialog_h_
//-----------------------------------------------------------------------------
//:
// \file
// \author Matt Leotta
// \date   14 June 2006
// \brief functions to open streams with user input via vgui_dialogs
//
// The functions in this file act as interactive factories.  They prompt
// the user with dialog boxes to obtain the required parameters to construct
// a vidl_istream or vidl_ostream.
//
// \verbatim
//  Modifications
//   24-Oct-2012  Peter Vanroose - was a class with static methods, now a namespace
// \endverbatim
//----------------------------------------------------------------------------


#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vidl/vidl_iidc1394_params.h>
#include <vidl/vidl_istream.h>
#include <vidl/vidl_ostream.h>


// forward declarations
class vidl_image_list_istream;
class vidl_image_list_ostream;
class vidl_ffmpeg_istream;
class vidl_ffmpeg_ostream;
class vidl_dc1394_istream;
class vidl_v4l2_istream;


//: Use vgui dialogs to prompt the user for parameters and open an istream
// Allows the user to select any of the supported istreams
vidl_istream* vidl_gui_open_istream_dialog();


//: Use vgui dialogs to prompt the user for parameters and open an ostream
// Allows the user to select any of the supported ostreams
vidl_ostream* vidl_gui_open_ostream_dialog();


namespace vidl_gui_param_dialog
{
  //: Use vgui dialogs to open an image list istream
  vidl_image_list_istream* image_list_istream();

  //: Use vgui dialogs to open an image list ostream
  vidl_image_list_ostream* image_list_ostream();

  //: Use vgui dialogs to open a FFMPEG istream
  vidl_ffmpeg_istream* ffmpeg_istream();

  //: Use vgui dialogs to open a FFMPEG ostream
  vidl_ffmpeg_ostream* ffmpeg_ostream();

  //: Use vgui dialogs to open a dc1394 istream
  vidl_dc1394_istream* dc1394_istream();

  //: Use vgui dialogs to open a v4l2 istream
  vidl_v4l2_istream* v4l2_istream();

  bool update_iidc1394_params(std::vector<vidl_iidc1394_params::
                              feature_options>& features);
};


#endif // vidl_gui_param_dialog_h_
