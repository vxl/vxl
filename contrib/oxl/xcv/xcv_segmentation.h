#ifndef xcv_segmentation_h_
#define xcv_segmentation_h_

//--------------------------------------------------------------------------------
// .NAME    xcv_segmentation - xcv menu and interface for segmentation
// .INCLUDE xcv/xcv_segmentation.h
// .FILE    xcv_segmentation.cxx
// .SECTION Description
//    Creates a menu for, and provides an interface to segmentation operations
//    contained in vxl.
// .SECTION Author
//   K.Y.McGaul
// .SECTION Modifications
//   K.Y.McGaul     27-JAN-2000    Initial version.
//--------------------------------------------------------------------------------

#include <vcl_list.h>
#include <vcl_vector.h>

class vgui_menu;
class vsl_harris_params;
class vsl_canny_ox_params;
class vgui_easy2D;
class vsl_edge;

class xcv_segmentation
{
public:
  static bool get_harris_params(vsl_harris_params*);
  static void perform_harris(vsl_harris_params&, unsigned col, unsigned row);
  static void harris();

  static void draw_edges(vcl_list<vsl_edge*> detected_edges,
    unsigned col, unsigned row);
  static void draw_straight_lines(vcl_vector<float>, vcl_vector<float>,
    vcl_vector<float>, vcl_vector<float>, unsigned, unsigned);
  static bool get_canny_ox_params(vsl_canny_ox_params*);
  static void canny_ox();
  static bool get_break_lines_ox_params(double* bk_thresh);
  static void get_broken_edges(double bk_thresh, vcl_list<vsl_edge*>*);
  static void break_lines_ox();
  static bool get_detect_lines_ox_params(float* high, int* edge_min,
    int* min_lenth, int* min_fit_length);
  static void detect_lines_ox();

  static vgui_menu create_segmentation_menu();

private:
  static vcl_list<vsl_edge*> detected_edges;
};

#endif // xcv_segmentation_
