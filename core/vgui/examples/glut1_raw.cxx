//:
// \file
// GLUT example
// \brief Pure glut + vgui for image drawing in a movie player.
// This example shows how all the tableau stuff for drawing
// (but not mouse or keyboard) can be used with a pure and
// simple GLUT program.  This means that the really really hairy
// image drawing stuff can be used in any GL program, whether or
// not it buys into the whole VGUI framework.  The other main
// point is to show that vgui just makes OpenGL commands -- no
// magic happens when you send a vgui_DRAW event.

#include <ctime>
#include <cstdlib>
#include <vgui/vgui_gl.h>
#include <vgui/vgui_glut.h>

#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_loader_tableau.h>
#include <vgui/vgui_deck_tableau.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

vgui_deck_tableau_new   deck;
vgui_loader_tableau_new load(deck);
vgui_shell_tableau_new  shell(load);

// GLUT keyboard event callback
void keyboard(unsigned char k, int, int)
{
  if (k=='q' || k=='Q' || k==27)
    std::exit(0);
}

// GLUT idle callback
void idle()
{
  static std::time_t last = 0;
  std::time_t now = time(nullptr);
  if (now > last) {
    // Advance the deck
    deck->next();

    // Tell glut to redraw, that will call display() [below]
    glutPostRedisplay();
    last = now;
  }
}

// GLUT display callback
void display()
{
  // This is the point where the vgui tableaux get to do their stuff.
  shell->handle(vgui_DRAW);

  glutSwapBuffers();
}

// usage: give a number of image filenames on command line.
int main(int argc, char **argv)
{
  // Initialize the tableau, and add any images on the commandline
  // to the deck.  When run, the images will cycle.
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
