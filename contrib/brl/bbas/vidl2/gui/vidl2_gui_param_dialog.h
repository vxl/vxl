// This is brl/bbas/vidl2/gui/vidl2_gui_param_dialog.h
#ifndef vidl2_gui_param_dialog_h_
#define vidl2_gui_param_dialog_h_
//-----------------------------------------------------------------------------
//:
// \file
// \author Matt Leotta
// \date   14 June 2006
// \brief functions to open streams with user input via vgui_dialogs
//
// The functions in this file act as interactive factories.  They prompt
// the user with dialog boxes to obtain the required parameters to construct
// a vidl2_istream or vidl2_ostream.
//
//----------------------------------------------------------------------------


#include <vidl2/vidl2_istream.h>
#include <vidl2/vidl2_ostream.h>

// forward declarations
class vidl2_image_list_istream;
class vidl2_image_list_ostream;
class vidl2_ffmpeg_istream;
class vidl2_ffmpeg_ostream;
class vidl2_dc1394_istream;


//: Use vgui dialogs to prompt the user for parameters and open an istream
// Allows the user to select any of the supported istreams
vidl2_istream* vidl2_gui_open_istream_dialog();


//: Use vgui dialogs to prompt the user for parameters and open an ostream
// Allows the user to select any of the supported ostreams
vidl2_ostream* vidl2_gui_open_ostream_dialog();


//: struct with static members used as a namespace replacement
// Change to a namespace when they are allowed in VXL
struct vidl2_gui_param_dialog
{
  //: Use vgui dialogs to open an image list istream
  static vidl2_image_list_istream* image_list_istream();

  //: Use vgui dialogs to open an image list ostream
  static vidl2_image_list_ostream* image_list_ostream();

  //: Use vgui dialogs to open a FFMPEG istream
  static vidl2_ffmpeg_istream* ffmpeg_istream();

  //: Use vgui dialogs to open a FFMPEG ostream
  static vidl2_ffmpeg_ostream* ffmpeg_ostream();

  //: Use vgui dialogs to open a dc1394 istream
  static vidl2_dc1394_istream* dc1394_istream();
};


#endif // vidl2_gui_param_dialog_h_
