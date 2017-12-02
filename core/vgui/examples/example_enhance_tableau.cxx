// Example of using vgui_enhance_tableau.
//
// If you run this executable with two image filenames as parameters on the
// command line, when you move the mouse over the image with the left mouse
// button held down you will see the second image in a region around the mouse
// pointer.
//
// If only one image tableau is given then that image will be magnified in a
// region around the mouse pointer.

#include <vgui/vgui.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_enhance_tableau.h>

int main(int argc, char ** argv)
{
  if (argc < 2) return 1;

  vgui::init(argc, argv);

  vgui_image_tableau_new image1(argv[1]);
  vgui_enhance_tableau_sptr enhance;

  if (argc == 2)
  {
    enhance = vgui_enhance_tableau_new(image1);
  }
  else
  {
    vgui_image_tableau_new image2(argv[2]);
    enhance = vgui_enhance_tableau_new(image1, image2);
  }

  vgui_viewer2D_tableau_new viewer(enhance);

  return vgui::run(viewer, image1->width(), image1->height() , "test_enhance");
}
