// This is oxl/xcv/xcv_file.h
#ifndef xcv_file_h_
#define xcv_file_h_
//:
// \file
// Creates a menu for, and provides an interface to operations for loading
// and saving files.
//
// \author  K.Y.McGaul
// \brief Interface to file functions.
//
//  Creates a menu for, and provides an interface to operations for loading
//  and saving files.
//
// \verbatim
//  Modifications:
//   K.Y.McGaul     27-JAN-2000    Initial version.
//   xx-AUG-2000 -- Marko Bacic, Oxford RRG added support for saving in different
//                  image formats
// \endverbatim

class vgui_menu;

class xcv_file
{
 public:
  //: Save image and spatial objects to specified postscript file.
  static void print_psfile();
  //: Load the selected image file into the current grid position.
  static void load_image();
  //: Save image to specified file.
  static void save_image();
  //: Dump OpenGL buffer to specified file.
  static void dump_image();
  //: Save image and spatial objects to postscript file.
  static void save_as_ps();
  //: Remove currently selected image.
  static void remove_image();
  //: Exit xcv.
  static void exit_xcv();
  //: Creates a menu containing all the functions in this file.
  static vgui_menu create_file_menu(void);
};

#endif // xcv_file_h_
