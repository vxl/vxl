#ifndef xcv_file_h_
#define xcv_file_h_

//--------------------------------------------------------------------------------
// .NAME    xcv_file
// .INCLUDE xcv/xcv_file.h
// .FILE    xcv_file.cxx
// .SECTION Description:
//   Creates a menu for, and provides an interface to operations for loading
//   and saving files.
// .SECTION Author
//   K.Y.McGaul
// .SECTION Modifications:
//   K.Y.McGaul     27-JAN-2000    Initial version.
//   xx-AUG-2000 -- Marko Bacic, Oxford RRG added support for saving in different
//                  image formats
//--------------------------------------------------------------------------------

class vgui_menu;

class xcv_file
{
public:
  static void print_psfile();
  static void load_image();
  static void remove_roi();
  static void save_image();
  static void save_as_ps();
  static void remove_image();
  static void exit_xcv();
  static vgui_menu create_file_menu(void);
};

#endif // xcv_file_
