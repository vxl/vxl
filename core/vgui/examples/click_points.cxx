//:
// \file
// \brief This program prints out the mouse position when the left mouse button is pressed.
// It goes to the next image (or exits) whenever the mouse position is at the bottom of the screen (lower 10 pixels).
// \author Peter Vanroose
// \date   4 November 2008

#include <iostream>
#include <cstdlib>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgui/vgui.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>

//-----------------------------------------------------------------------------
// A tableau that displays the mouse position when left mouse button is pressed.
struct example_tableau : public vgui_image_tableau
{
  char ** filenames;
  int nr_files;
  int current_file;
  example_tableau(char ** f, int n = 1) : vgui_image_tableau(*f), filenames(f), nr_files(n), current_file(0) { }

  ~example_tableau() { }

  bool handle(const vgui_event &e)
  {
    if (e.type == vgui_BUTTON_DOWN && e.button == vgui_LEFT && e.modifier == 0)
    {
      if (e.wy < 10) {
        ++current_file;
        if (current_file >= nr_files) std::exit(0);
        set_image(filenames[current_file]);
        post_redraw();
      }
      else std::cout << filenames[current_file] << "\t" << e.wx << "\t" << e.wy << std::endl;
      return true; // event has been used
    }

    // We are not interested in other events, so pass event to base class:
    return vgui_image_tableau::handle(e);
  }
};

//-----------------------------------------------------------------------------
// Make a smart-pointer constructor for our tableau.
struct example_tableau_new : public vgui_image_tableau_sptr
{
  example_tableau_new(char **f, int n = 1) : vgui_image_tableau_sptr(
    new example_tableau(f, n)) { }
};

//-----------------------------------------------------------------------------
// The command line arguments are expected to be image filenames.
// They are opened one after the other, each time one clicks at the bottom of the screen.
int main(int argc,char **argv)
{
  vgui::init(argc, argv);
  if (argc <= 1)
  {
    std::cerr << "Please give an image filename on the command line\n";
    return 0;
  }

  // Pass the image filename list into my tableau (derived from vgui_image_tableau) and load the first one
  vgui_tableau_sptr my_tab = example_tableau_new(argv+1, argc-1);

  vgui_viewer2D_tableau_new viewer(my_tab);
  vgui_shell_tableau_new shell(viewer);

  // Start event loop, using easy method.
  return vgui::run(shell, 960, 540);
}
