#ifndef xcv_display_h_
#define xcv_display_h_

//--------------------------------------------------------------------------------
// .NAME    xcv_display
// .INCLUDE xcv/xcv_display.h
// .FILE    xcv_display.cxx
// .SECTION Description:
//   Creates a menu for, and provides an interface to operations that change
//   the display.
// .SECTION Author
//   K.Y.McGaul
// .SECTION Modifications:
//   K.Y.McGaul     27-JAN-2000    Initial version.
//   Marko Bacic    19-JUL-2000    Added support for region of interest
//--------------------------------------------------------------------------------

class vgui_menu;
class vgui_roi_tableau_make_roi;

class xcv_display
{
public:
  static void centre_image();
  static void toggle_magnify();
  static void toggle_enhance();
  static void make_roi();
  static void remove_roi();
  static vgui_menu create_display_menu();

private:
  static vgui_roi_tableau_make_roi *roi_tableau_client_;
};

#endif // xcv_display_
