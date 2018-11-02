#include <cmath>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vgui/vgui_gl.h>
#include <vgui/vgui_glut.h>

#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_loader_tableau.h>

vgui_image_tableau_new   image;
vgui_loader_tableau_new  load(image);
vgui_shell_tableau_new   shell(load);

// GLUT display callback. we never return from it.
void display()
{
  int counter = 0;
  while (true) {
    load->set_image(int(512*(1+0.5*std::cos(counter/10.0))),
                    int(512*(1+0.5*std::sin(counter/ 7.0))));
    ++counter;

    shell->handle(vgui_DRAW);

    glutSwapBuffers();
  }
}

// usage: give an image filenames on command line.
int main(int argc, char **argv)
{
  image->set_image(argv[1]);

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
  glutInitWindowSize(512, 512);
  glutCreateWindow(__FILE__);
  glutDisplayFunc(display);
  glutMainLoop();

  return 0;
}
