#ifndef xcv_multiview_h_
#define xcv_multiview_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// This is xcv/xcv_multiview.h
//
//: Creates a menu for, and provides an interface to multiview operations
//  contained in mvl.
//
// \file
// \author   K.Y.McGaul
// \verbatim
//  Modifications
//   K.Y.McGaul     27-JAN-2000    Initial version.
// \endverbatim
//

#include "xcv_twoview_manager.h"
#include "xcv_threeview_manager.h"
#include <vcl_vector.h>

class vgui_menu;
class vgui_rubberbander;
class vgui_easy2D;
class HomgPoint2D;

class xcv_multiview
{
public:
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

  //:Load a TriTensor into the threeview_manager from file.
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
  static xcv_twoview_manager* get_twoview_manager(vcl_vector<int>& cols, vcl_vector<int>& rows);
  static vcl_vector<vgui_rubberbander_sptr> twoview_rubber0, twoview_rubber1;
  static vcl_vector<xcv_twoview_manager*> twoview_mgrs;

  //: Gets a threeview_manager between the tableax at the given positions.
  static xcv_threeview_manager* get_threeview_manager(vcl_vector<int>& cols, vcl_vector<int>& rows);
  static vcl_vector<vgui_rubberbander_sptr> threeview_rubber0, threeview_rubber1,
    threeview_rubber2;
  static vcl_vector<xcv_threeview_manager*> threeview_mgrs;
};

#endif // xcv_multiview_h
