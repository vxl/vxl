//:
// \file
// \author Kongbin Kang

#include "brct_windows_frame.h"
#include <vcl_cstdlib.h> // for vcl_exit()
#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_easy3D_tableau.h>
#include <vgui/vgui_viewer3D_tableau.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include "brct_menus.h"
//static live_video_manager instance
brct_windows_frame *brct_windows_frame::instance_ = 0;

//===============================================================
//: The singleton pattern - only one instance of the manager can occur
//==============================================================
brct_windows_frame *brct_windows_frame::instance()
{
  if (!instance_)
  {
    instance_ = new brct_windows_frame();
    instance_->init();
  }
  return brct_windows_frame::instance_;
}

//==================================================================
//: constructors/destructor
//==================================================================
brct_windows_frame::
brct_windows_frame() : vgui_wrapper_tableau()
{
}

brct_windows_frame::~brct_windows_frame()
{
}

//======================================================================
//: set up the tableaux at each grid cell
//  the vtol2D_tableaux have been initialized in the constructor
//======================================================================
void brct_windows_frame::init()
{
  grid_ = vgui_grid_tableau_new(2,1);
  grid_->set_grid_size_changeable(true);
  unsigned int col=0, row = 0;
  // add 3D tableau

  vgui_easy3D_tableau_new tab3d;
  tab_3d_ = tab3d;
  tab_3d_->set_point_radius(2);
  // Add a point at the origin
  tab_3d_->set_foreground(1,1,1);
  tab_3d_->add_point(0,0,0);

    // Add a line in the xaxis:
  tab_3d_->set_foreground(1,1,0);
  tab_3d_->add_line(1,0,0, 4,0,0);

  // Add a line in the yaxis:
  tab_3d_->set_foreground(0,1,0);
  tab_3d_->add_line(0,1,0, 0,4,0);

  // Add a line in the zaxis:
  tab_3d_->set_foreground(0,0,1);
  tab_3d_->add_line(0,0,1, 0,0,4);

  vgui_viewer3D_tableau_sptr v3d = vgui_viewer3D_tableau_new(tab_3d_);
  grid_->add_at(v3d, col+1, row);
  vgui_shell_tableau_sptr shell = vgui_shell_tableau_new(grid_);
   this->add_child(shell);
}

//=========================================================================
//: make an event handler
// note that we have to get an adaptor and set the tableau to receive events
// this handler does nothing but is a place holder for future event processing
// For now, just pass the events down to the child tableaux
//=========================================================================
bool brct_windows_frame::handle(const vgui_event &e)
{
  return this->child.handle(e);
}


void brct_windows_frame::quit()
{
  vcl_exit(1);
}

void brct_windows_frame::add_curve3d(vcl_vector<vgl_point_3d<double> >& pts)
{
  int size = pts.size();
  points_3d_.resize(size);
  instance_->tab_3d_->set_foreground(1, 1, 1);
  for (int i=0; i<size-1; i++) {
    vgl_point_3d<double>& s = pts[i];
    vgl_point_3d<double>& e = pts[i+1];
    instance_->tab_3d_->add_line(s.x(), s.y(), s.z(), e.x(), e.y(), e.z());
  }

  instance_->post_redraw();
}

void brct_windows_frame::remove_curve3d()
{
  points_3d_.clear();
  this->post_redraw();
}
