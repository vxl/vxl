#ifndef xcv_multiview_h_
#define xcv_multiview_h_
#ifdef __GNUC__
#pragma interface
#endif
 
//--------------------------------------------------------------------------------
// .NAME	xcv_multiview - xcv menu and interface for multiview
// .INCLUDE	xcv/xcv_multiview.h
// .FILE	xcv_multiview.cxx
// .SECTION Description
//   Creates a menu for, and provides an interface to multiview operations
//   contained in mvl.
// .SECTION Author
//   K.Y.McGaul
// .SECTION Modifications
//   K.Y.McGaul     27-JAN-2000    Initial version.
//--------------------------------------------------------------------------------

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
  static void load_f_matrix();
  static void load_h_matrix2d();
  static void load_corner_matches();
  
  static void save_f_matrix();
  static void save_h_matrix2d();
  static void save_corner_matches();
 
  static void toggle_f_matrix();
  static void toggle_h_matrix();
  static void display_corner_matches();
  static void display_corner_tracks();

  static void load_tri_tensor();
  static void save_tri_tensor();
  static void toggle_tri_tensor();
  static void transfer_point();
  static void transfer_line();
  static vgui_menu create_multiview_menu();

protected:
  static xcv_twoview_manager* get_twoview_manager(vcl_vector<int>& cols, vcl_vector<int>& rows);
  static vcl_vector<vgui_rubberbander_ref> twoview_rubber0, twoview_rubber1;
  static vcl_vector<xcv_twoview_manager*> twoview_mgrs;

  static xcv_threeview_manager* get_threeview_manager(vcl_vector<int>& cols, vcl_vector<int>& rows);
  static vcl_vector<vgui_rubberbander_ref> threeview_rubber0, threeview_rubber1, 
    threeview_rubber2;
  static vcl_vector<xcv_threeview_manager*> threeview_mgrs;

};
#endif // xcv_multiview_h
