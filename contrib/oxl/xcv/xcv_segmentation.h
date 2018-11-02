// This is oxl/xcv/xcv_segmentation.h
#ifndef xcv_segmentation_h_
#define xcv_segmentation_h_
//:
// \file
// \author  K.Y.McGaul
// \brief   Interface for segmentation operations.
//
//  Creates a menu for, and provides an interface to segmentation operations
//  contained in vxl.
//
// \verbatim
//  Modifications
//   K.Y.McGaul     27-JAN-2000    Initial version.
// \endverbatim

#include <iostream>
#include <list>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class vgui_menu;
class osl_harris_params;
class osl_canny_ox_params;
class osl_edge;

class xcv_segmentation
{
 public:
  //: Make and display a dialog box to get Harris parameters.
  static bool get_harris_params(osl_harris_params*);
  //: Perform Harris corner detection.
  static void perform_harris(osl_harris_params&, unsigned col, unsigned row);
  //: Get the current row and column and params for Harris corner detection.
  static void harris();

  //: Draw the given edges onto the given location.
  static void draw_edges(std::list<osl_edge*> detected_edges,
                         unsigned col, unsigned row);
  //: Draw straight lines onto the given location.
  static void draw_straight_lines(std::vector<float>, std::vector<float>,
                                  std::vector<float>, std::vector<float>,
                                  unsigned, unsigned);
  //: Display a dialog box to get the parameters needed for Oxford Canny.
  static bool get_canny_ox_params(osl_canny_ox_params*);
  //: Perform Oxford canny edge detection.
  static void canny_ox();
  //: Get parameters from the user for break_lines_ox.
  static bool get_break_lines_ox_params(double* bk_thresh);
  //: Get a list of broken edges from the current image.
  static void get_broken_edges(double bk_thresh, std::list<osl_edge*>*);
  //: Break edges at points of extreme "bentness".
  static void break_lines_ox();
  //: Get parameters from user required for detect_lines_ox.
  static bool get_detect_lines_ox_params(float* high, int* edge_min,
                                         int* min_length, int* min_fit_length);
  //: Detect straight lines in the edges computed by Canny.
  static void detect_lines_ox();

  //: Creates a menu containing all the functions in this file.
  static vgui_menu create_segmentation_menu();

 private:
  //: List of edges detected by Oxford canny.
  static std::list<osl_edge*> detected_edges;
};

#endif // xcv_segmentation_h_
