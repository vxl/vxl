// Example using vgui_poly_tableau.
//
// Give two image filenames on the command line.


#include <vgui/vgui.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_poly_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>

int main(int argc,char **argv) 
{
  vgui::init(argc,argv);

  if (argc < 2) return 1;

  vgui_image_tableau_new image1(argv[1]);

  vgui_image_tableau_new image2(argv[2]);

  vgui_poly_tableau_new poly;
  //               x    y    w    h
  poly->add(image1, 0.0, 0.0, 0.7, 0.7);
  poly->add(image2, 0.5, 0.5, 0.5, 0.5);

  vgui_viewer2D_tableau_new view(poly);
  vgui_shell_tableau_new shell(view);
  return vgui::run(shell, 512, 512,__FILE__);
}
