/*
  fsm
*/
#include <vgui/vgui.h>
#include <vgui/vgui_gl.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_matrix_state.h>
#include <vgui/vgui_composite.h>
#include <vgui/vgui_load.h>
#include <vgui/vgui_zoomer.h>
#include <vgui/vgui_image_tableau.h>

// tableau to draw a rubber line over the background
class rubber : public vgui_tableau {
public:
  rubber() : in_loop(false) { }
  ~rubber() { }
  vcl_string type_name() const { return "rubber"; }

  bool in_loop; // true when rubber banding.
  int x0,y0;    // position where mouse was pressed.
  int x1,y1;    // position it has moved to.
  bool handle(vgui_event const &e) {
    // start rubber banding
    if (e.type == vgui_BUTTON_DOWN && !in_loop) {
      x0 = x1 = e.wx;
      y0 = y1 = e.wy; // remember where it was
      in_loop = true;
      post_overlay_redraw();
      return true;
    }

    // motion
    else if (e.type == vgui_MOTION && in_loop) {
      x1 = e.wx;
      y1 = e.wy;
      post_overlay_redraw();
      return true;
    }

    // draw overlays
    else if (e.type == vgui_DRAW_OVERLAY && in_loop) {
      vgui_matrix_state M;

      // set up a direct correspondence between the mouse pointer
      // positions given (in viewport coordinates) and the vertex
      // coordinates.
      vgui_matrix_state::identity_gl_matrices();
      GLint vp[4];
      glGetIntegerv(GL_VIEWPORT, vp);
      glOrtho(0/*vp[0]*/, vp[0]+vp[2],  // left, right
              0/*vp[1]*/, vp[1]+vp[3],  // bottom, top
              1,-1);                    // near, far

      // draw
      glLineWidth(1);
      glColor3f(1,1,1);
      glBegin(GL_LINES);
      glVertex2i(x0, y0);
      glVertex2i(x1, y1);
      glEnd();

      return true;
    }

    // finish rubber banding
    else if (e.type == vgui_BUTTON_UP && in_loop) {
      in_loop = false;
      post_overlay_redraw(); // post once more, to clear the overlay plane.
      return true;
    }

    // ignore
    else
      return false;
  }
};

int main(int argc,char **argv) {
  vgui::init(argc,argv);

  vgui_image_tableau image(argv[1] ? argv[1] : "az32_10.tif");
  rubber rc;
  vgui_composite comp(&image, &rc);
  vgui_zoomer zoom(&comp);
  vgui_load load(&zoom); load.set_ortho(0, image.height(), image.width(), 0);

  return vgui::run(&load, image.width(), image.height(), "test_rubber");
}
