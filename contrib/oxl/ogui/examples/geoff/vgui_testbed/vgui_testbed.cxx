#include <vcl_compiler.h>
#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_viewer2D.h>
#include <vgui/vgui_easy2D.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_command.h>

#include "vgui_point_selector.h"


//-----------------------------------------------------------------------------
// Callbacks (called from menu items)
//-----------------------------------------------------------------------------

void callback1( const void *ptr)
{
  vcl_cerr << "callback1 called with ptr=" << ptr << vcl_endl;
}

void callback2( const void *ptr)
{
  vcl_cerr << "callback2 called with ptr=" << ptr << vcl_endl;
}

void callback3( const void *ptr)
{
  vcl_cerr << "callback3 called with ptr=" << ptr << vcl_endl;
}

void callback4( const void *ptr)
{
  vcl_cerr << "callback4 called with ptr=" << ptr << vcl_endl;
}

void callback5( const void *ptr)
{
  vcl_cerr << "callback5 called with ptr=" << ptr << vcl_endl;
}

//-----------------------------------------------------------------------------
// Main
//-----------------------------------------------------------------------------

int main(int argc, char** argv)
{
  // Initialize the toolkit.
  vgui::init(argc, argv);

  //////////////////////////////////////////////
  // Create some tableaux

  // load an image and display it
  vgui_image_tableau_sptr imagetab( new vgui_image_tableau( "bt.000.pgm"));
  // This only works when the program is run from the source directory!

  // dump some geometry onto the image
  vgui_easy2D_sptr        geometrytab( new vgui_easy2D( imagetab));
  geometrytab->set_line_width( 3);
  geometrytab->add_line( 10, 20, 100, 120);
  geometrytab->add_line( 100, 120, 231, 423);

  // this is a zoomer and panner
  vgui_viewer2D_sptr      viewertab( new vgui_viewer2D( geometrytab));

  // our local tableau which just displays any mouse-click events
  example_point_selector_sptr pointselectortab( new example_point_selector( viewertab));

  // this does a few minor things, and isn't really necessary, but it does
  //  do some useful things like clearing the background when you pan or zoom
  vgui_shell_tableau_sptr shelltab( new vgui_shell_tableau( pointselectortab));

  //////////////////////////////////////////////

  //////////////////////////////////////////////
  // Create some a menu or two
  vgui_menu menubar;
  vgui_menu menuoption1;
  vgui_menu menuoption2;
  vgui_menu submenu;

  menuoption1.add( "Item 1.1", new vgui_command_cfunc( &callback1, ( void *) NULL));
  menuoption1.add( "Item 1.2", new vgui_command_cfunc( &callback2, ( void *) NULL));
  menuoption1.add( "Item 1.3", new vgui_command_cfunc( &callback3, ( void *) NULL));
  menuoption2.add( "Item 2.1", new vgui_command_cfunc( &callback4, ( void *) NULL));
  menuoption2.add( "Item 2.2", new vgui_command_cfunc( &callback5, ( void *) NULL));

  submenu.add( "Item 2.3.1", new vgui_command_cfunc( &callback1, ( void *) NULL));
  submenu.add( "Item 2.3.2", new vgui_command_cfunc( &callback2, ( void *) NULL));
  submenu.add( "Item 2.3.3", new vgui_command_cfunc( &callback3, ( void *) NULL));
  menuoption2.add( "Item 2.3", submenu);

  menubar.add( "Option 1", menuoption1);
  menubar.add( "Option 2", menuoption2);
  //////////////////////////////////////////////


  // Initialize the window
  vcl_string title = "xcv";
  vgui_window *win = vgui::produce_window( 300, 300, menubar, title);

  // add tableaux hierarchy to window
  win->get_adaptor()->set_tableau( shelltab);

  win->set_statusbar(true);
  win->enable_vscrollbar(false);
  win->enable_hscrollbar(false);
  win->show();
  return vgui::run();
}
