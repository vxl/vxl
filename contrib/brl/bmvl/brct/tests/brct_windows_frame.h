#ifndef brct_windows_frame_h_
#define brct_windows_frame_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief Manager for multiview gui applications
// \author
//   Kongbin Kang
//
// \endverbatim
//--------------------------------------------------------------------------------
#include <vgui/vgui_wrapper_tableau.h>
#include <vgui/vgui_easy3D_tableau.h>
#include <vgui/vgui_easy3D_tableau_sptr.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_grid_tableau_sptr.h>

class vgui_window;

//: A manager for displaying segmentation results.
class brct_windows_frame : public vgui_wrapper_tableau
{
 public:
  brct_windows_frame();
  ~brct_windows_frame();
  static brct_windows_frame *instance();

  //:methods for menu callbacks
  void quit();
  
  //: access to the window
  vgui_window* get_window(){return win_;}
  void set_window(vgui_window* win){win_=win;}
  void init();
  //: the virtual handle function
  virtual bool handle(const vgui_event&);

 protected:
  //:internal utility methods


  //
 private:
  vgui_window* win_;
  vgui_easy3D_tableau_sptr easy3d_tabs_;
  vgui_grid_tableau_sptr grid_;
  static brct_windows_frame *instance_;
};

#endif // brct_windows_frame_h_
