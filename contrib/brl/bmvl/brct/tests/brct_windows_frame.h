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
#include <vgui/vgui_menu.h>
#include <vgl/vgl_point_3d.h>

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
  void add_curve3d(vcl_vector<vgl_point_3d<double> >& pts);
  void remove_curve3d();
  
  //: access to the window
  //vgui_window* get_window(){return win_;}
  void set_window(vgui_window* win){win_=win;}
  void init();
  
  //: the virtual handle function
  virtual bool handle(const vgui_event&);

 protected:
  //:internal utility methods
	 

 private:
   //: get track of all the 3d points added into 3d tableau
  vgui_window* win_;
  vcl_vector<vgui_point3D* > points_3d_;
  vgui_easy3D_tableau_sptr tab_3d_;
  vgui_grid_tableau_sptr grid_;
  static brct_windows_frame *instance_;
};

#endif // brct_windows_frame_h_
