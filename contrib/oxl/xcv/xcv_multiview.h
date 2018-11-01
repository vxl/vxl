// This is oxl/xcv/xcv_multiview.h
#ifndef xcv_multiview_h_
#define xcv_multiview_h_
//:
// \file
// \author   K.Y.McGaul
// \brief Interface for multiview functions.
//
//  Creates a menu for, and provides an interface to multiview operations
//  contained in mvl.
//
// \verbatim
//  Modifications
//   K.Y.McGaul     27-JAN-2000    Initial version.
//   Peter Vanroose 18-JAN-2002    Added compute methods for F-matrix & H-matrix
// \endverbatim

#include <iostream>
#include <vector>
#include "xcv_twoview_manager.h"
#include "xcv_threeview_manager.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class vgui_menu;

class xcv_multiview
{
 public:
  //: Compute an FMatrix for the two selected views.
  static void compute_f_matrix();
  //: Compute an HMatrix2D for the two selected views.
  static void compute_h_matrix2d();
  //: Compute corner matches for the two selected views.
  static void compute_corner_matches();

  //: Load an FMatrix for the two selected views from a file.
  static void load_f_matrix();
  //: Load an HMatrix2D for the two selected views from a file.
  static void load_h_matrix2d();
  //: Load corner matches for the two selected views from a file.
  static void load_corner_matches();

  //: Save the FMatrix of the two selected views to file.
  static void save_f_matrix();
  //: Save the HMatrix2D of the two selected views to file.
  static void save_h_matrix2d();
  //: Save the corner matches of the two selected views to file.
  static void save_corner_matches();

  //: Toggle display of the FMatrix.
  static void toggle_f_matrix();
  //: Toggle display of the HMatrix2D.
  static void toggle_h_matrix();
  //: Display corner matches by highlighting corresponding points in two views.
  static void display_corner_matches();
  //: Display corner tracks.
  static void display_corner_tracks();

  //: Load a TriTensor into the threeview_manager from file.
  static void load_tri_tensor();
  //: Save a TriTensor to file.
  static void save_tri_tensor();
  //: Toggle display of the TriTensor for the selected views.
  static void toggle_tri_tensor();
  //: Transfers a point on two view to a point on the third view.
  static void transfer_point();
  //: Transfers a line on two views to a line on the third view.
  static void transfer_line();
  //: Creates a menu containing all the functions in xcv_multiview.
  //  Note that this is implemented in xcv_multiview_create_multiview_menu.cxx
  static vgui_menu create_multiview_menu();

 protected:
  //: Gets a twoview_manager between the tableaux at the given positions.
  static xcv_twoview_manager* get_twoview_manager(std::vector<int>& cols, std::vector<int>& rows);
  static std::vector<vgui_rubberband_tableau_sptr> twoview_rubber0, twoview_rubber1;
  static std::vector<xcv_twoview_manager*> twoview_mgrs;

  //: Gets a threeview_manager between the tableaux at the given positions.
  static xcv_threeview_manager* get_threeview_manager(std::vector<int>& cols, std::vector<int>& rows);
  static std::vector<vgui_rubberband_tableau_sptr> threeview_rubber0, threeview_rubber1,
    threeview_rubber2;
  static std::vector<xcv_threeview_manager*> threeview_mgrs;
};

#endif // xcv_multiview_h_
