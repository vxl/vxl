#ifndef DOXYGEN_SHOULD_SKIP_THIS

// \author  fsm@robots.ox.ac.uk
//
// GLUT example.

#include <vgui/vgui_gl.h>
#include <vgui/vgui_glut.h>

#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_loader_tableau.h>
#include <vgui/vgui_deck_tableau.h>

#include <vcl_ctime.h>
#include <vcl_cstdlib.h> // for vcl_exit()

vgui_deck_tableau_new   deck;
vgui_loader_tableau_new load(deck);
vgui_shell_tableau_new  shell(load);

// GLUT display callback
void display()
{
  shell->handle(vgui_DRAW);

  glutSwapBuffers();
}

// GLUT keyboard event callback
void keyboard(unsigned char k, int, int)
{
  if (k=='q' || k=='Q' || k==27)
    vcl_exit(0);
}

// GLUT idle callback
void idle()
{
  static vcl_time_t last = 0;
  vcl_time_t now = time(0);
  if (now > last) {
    deck->next();
    glutPostRedisplay();
    last = now;
  }
}

// usage: give a number of image filenames on command line.
int main(int argc, char **argv)
{
  load->set_image(512, 512);
  for (int i=1; i<argc; ++i)
    deck->add(vgui_image_tableau_new(argv[i]));

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
  glutInitWindowSize(512, 512);
  glutCreateWindow(__FILE__);
  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  glutIdleFunc(idle);
  glutMainLoop();

  return 0;
}

#endif
