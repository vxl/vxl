//:
// \file
// \author Kongbin Kang

#include "brct_windows_frame.h"
#include <vcl_cstdlib.h> // for vcl_exit()
#include <vcl_iostream.h>
#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_easy3D_tableau.h>
#include <vgui/vgui_viewer3D_tableau.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_shell_tableau.h>

//static live_video_manager instance
brct_windows_frame *brct_windows_frame::instance_ = 0;
vcl_string brct_windows_frame::win_title_ = "BRCT";

//===============================================================
//: The singleton pattern - only one instance of the manager can occur
//==============================================================
brct_windows_frame *brct_windows_frame::instance(vcl_string& s)
{
  if (!instance_)
    {
      instance_ = new brct_windows_frame();
	  brct_windows_frame::win_title_ = s;
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
	
	tab3d->set_point_radius(15);
  	// Add a point at the origin
  	tab3d->set_foreground(1,1,1);
  	tab3d->add_point(0,0,0);

  	// Add a line in the xaxis:
  	tab3d->set_foreground(1,1,0);
  	tab3d->add_line(1,0,0, 4,0,0);
  	tab3d->add_point(5,0,0);

  	// Add a line in the yaxis:
  	tab3d->set_foreground(0,1,0);
  	tab3d->add_line(0,1,0, 0,4,0);
  	tab3d->add_point(0,5,0);

  // Add a line in the zaxis:
  tab3d->set_foreground(0,0,1);
  tab3d->add_line(0,0,1, 0,0,4);
  tab3d->add_point(0,0,5);

  // Add a small triangle
  tab3d->set_foreground(1,0,0);
  tab3d->add_triangle( 2,0,0, 0,2,0, 0,0,2 );


	tab3d->set_point_radius(15);
	tab3d->set_foreground(1,1,1);
	tab3d->add_point(0,0,0);
    	
	vgui_viewer3D_tableau_sptr v3d = vgui_viewer3D_tableau_new(tab3d);
	grid_->add_at(v3d, col+1, row);
	vgui_shell_tableau_sptr shell = vgui_shell_tableau_new(grid_);
 	this->add_child(shell);

	// set up components of window: menu, scroll bar etc.
	win_ = vgui::produce_window(800, 600, produce_menu(), brct_windows_frame::win_title_);
  	win_->set_statusbar(true);
  	win_->enable_vscrollbar(true);
  	win_->enable_hscrollbar(true);
  	win_->show();
  	win_->get_adaptor()->set_tableau(instance_);
  	this->set_window(win_);
  	this->post_redraw();
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

vgui_menu brct_windows_frame::produce_menu()
{
	return vgui_menu();		
}
