// This is oxl/xcv/xcv_display.h
#ifndef xcv_display_h_
#define xcv_display_h_
//:
// \file
// \author   K.Y.McGaul
// \brief Interface for display functions.
//
//  Creates a menu for, and provides an interface to operations that change
//  the display.
//
// \verbatim
//  Modifications:
//   K.Y.McGaul     27-JAN-2000    Initial version.
//   Marko Bacic    19-JUL-2000    Added support for region of interest
// \endverbatim

#include <vil1/vil1_image.h>
class vgui_menu;
class vgui_roi_tableau_make_roi;

class xcv_display
{
 public:
  //: Center the current image inside the frame.
  static void centre_image();
  //: Turn the magnify glass on/off.
  static void toggle_magnify();
  //: Turn the enhance lens on/off.
  static void toggle_enhance();
  //: Define a region of interest.
  static void make_roi();
  //: Remove the region of interest.
  static void remove_roi();
  //: Show a profile of the image intensity on a user-selected line.
  static void show_line_slice();
  //: Creates a menu containing all the functions in this file.
  static vgui_menu create_display_menu();

 private:
  //: Generates intensity values along a line.
  static void line_profile(const vil1_image&, float x0, float y0, float x1, float y1,
                           int num_points, float* xvals, float* yvals, float* ivals);
  static vgui_roi_tableau_make_roi *roi_tableau_client_;
};

#endif // xcv_display_h_
