//this-sets-emacs-to-*-c++-*-mode
#ifndef bmvv_tomography_manager_h_
#define bmvv_tomography_manager_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief Manager for tomography gui applications
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy March 05, 2002    Initial version.
// \endverbatim
//--------------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vil/vil_image.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <vdgl/vdgl_intensity_face_sptr.h>
#include <vgui/vgui_wrapper_tableau.h>
#include <vgui/vgui_easy2D_tableau_sptr.h>
#include <vgui/vgui_grid_tableau_sptr.h>
#include <bgui/bgui_vtol2D_tableau_sptr.h>
#include <bgui/bgui_picker_tableau_sptr.h>

class vgui_window;

//: A manager for displaying segmentation results.
class bmvv_tomography_manager : public vgui_wrapper_tableau
{
 public:
  bmvv_tomography_manager();
  ~bmvv_tomography_manager();
  static bmvv_tomography_manager *instance();

  //:methods for menu callbacks
  void quit();
  void load_image();
  void save_sinogram();
  void clear_display();
  void clear_selected();
  void sinogram();
  //: access to the window
  vgui_window* get_window(){return win_;}
  void set_window(vgui_window* win){win_=win;}
  void init();
  //: the virtual handle function
  virtual bool handle(const vgui_event&);
 protected:
  //:internal utility methods

  bgui_vtol2D_tableau_sptr get_vtol2D_tableau_at(unsigned col, unsigned row);
  bgui_vtol2D_tableau_sptr get_selected_vtol2D_tableau();
  bgui_picker_tableau_sptr get_picker_tableau_at(unsigned col, unsigned row);
  bgui_picker_tableau_sptr get_selected_picker_tableau();
  //
 private:
  vil_image img_;
  vil_image sino_;
  vgui_window* win_;
  vcl_vector<bgui_vtol2D_tableau_sptr> vtol_tabs_;
  vcl_vector<vtol_edge_2d_sptr> selected_curves_;
  vgui_grid_tableau_sptr grid_;
  
  static bmvv_tomography_manager *instance_;
};

#endif // bmvv_tomography_manager_h_
