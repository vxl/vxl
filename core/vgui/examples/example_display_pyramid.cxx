// This example is given an image filename on the command line and displays
// an image pyramid for that image.  The user can move through the list
// of images by pressing PageUp and PageDown.

#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vil1/vil1_load.h>
#include <vil1/vil1_image.h>
#include <vil1/vil1_pyramid.h>

#include <vgui/vgui.h>
#include <vgui/vgui_gl.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_image_tableau.h>

struct example_pyramid_tableau : public vgui_tableau
{
  int level;
  vil1_pyramid pyr;
  vgui_image_tableau_new image_tab;
  vgui_parent_child_link pclink;

  example_pyramid_tableau(vil1_image const &image)
    : level(0), pyr(image), pclink(this, image_tab) {
    image_tab->set_image(image);
  }

  bool handle(vgui_event const &e) {
    // compensate for size change by scaling by 2^level :
    glMatrixMode(GL_MODELVIEW);
    for (int i=0; i<level; ++i)
      glScalef(2, 2, 2);

    // Look for PageUp and PageDown events.
    if (e.type == vgui_KEY_PRESS && e.key == vgui_PGUP)
    {
      ++level;
      if (level <= 5) {
        image_tab->set_image(pyr[level]);
        post_redraw();
        std::cerr << "level " << level << std::endl;
      }
      else
        level = 5;
      return true;
    }
    else if (e.type == vgui_KEY_PRESS && e.key == vgui_PGDN)
    {
      --level;
      if (level >= 0)
      {
        image_tab->set_image(pyr[level]);
        post_redraw();
        std::cerr << "level " << level << std::endl;
      }
      else
        level = 0;
      return true;
    }
    else
      return pclink.handle(e);
  }
};

int main(int argc, char **argv)
{
  vgui::init(argc, argv);

  if (argc != 2)
  {
    std::cerr << "need name of image" << std::endl;
    return 1;
  }

  vil1_image image = vil1_load(argv[1]);
  if (!image) {
    std::cerr << "load failed -- invalid image?" << std::endl;
    return 1;
  }
  std::cerr << image << std::endl;

  vgui_tableau_sptr tab(new example_pyramid_tableau(image));
  vgui_viewer2D_tableau_new zoom(tab);
  return vgui::run(zoom, image.width(), image.height());
}
