/*
  fsm
*/
#include <vgui/vgui.h>
#include <vgui/vgui_composite.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_spinner.h>
#include <vgui/vgui_load.h>

struct blat : public vgui_tableau {
  vcl_string type_name() const { return "blat"; }
  bool handle(vgui_event const &e) {
    if (e.type != vgui_DRAW)
      return false;
    glColor3f(1,1,1);
    glPointSize(4);
    glBegin(GL_POINTS);
    glVertex2f(0,0);
    glVertex2f(50,0);
    glVertex2f(0,50);
    glVertex2f(50,50);
    glEnd();
    glLineWidth(1);
    glBegin(GL_LINES);
    glVertex2f(-50,50);
    glVertex2f(50,-50);
    glEnd();
    return true;
  }
};

int main(int argc,char **argv) {
  vgui::init(argc,argv);

  const char *filename = argc>=2 ? argv[1] : (char*)0;
  vcl_cerr << "image file \'" << filename << "\'\n";

  // tableaux
  vgui_image_tableau image( filename );

  blat dlr; // draws some things

  vgui_composite com(&image, &dlr);

  vgui_spinner man(&com);

  vgui_load vs(&man);
  vs.set_ortho(0,256, 256,0);

#if 0
  vgui_shell_tableau shell(&vs);
  return vgui::run(&shell, 256, 256, "test_texture");
#endif // 0
  return vgui::run(&vs, 256, 256, "test_texture");
}
