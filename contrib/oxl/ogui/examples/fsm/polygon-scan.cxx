//:
// \file
// \author fsm

#include <vcl_cassert.h>
#include <vcl_cmath.h>
#include <vcl_vector.h>

#include <vul/vul_arg.h>

#include <vgl/vgl_polygon.h>
#include <vgl/vgl_polygon_scan_iterator.h>

#include <vnl/vnl_math.h>

#include <vgui/vgui.h>
#include <vgui/vgui_gl.h>
#include <vgui/vgui_viewer2D.h>
#include <vgui/vgui_projection_inspector.h>

struct example_ptab : public vgui_tableau
{
  bool boundary;
  int v;
  vcl_vector<float> x;
  vcl_vector<float> y;

  example_ptab(unsigned n, bool boundary_) : boundary(boundary_), v(-1) {
    for (unsigned i=0; i<n; ++i) {
      double t = 2*vnl_math::pi*i/n;
      x.push_back(vcl_cos(t));
      y.push_back(vcl_sin(t));
    }
  }

  bool handle(vgui_event const &e) {
    unsigned n = x.size(); assert(n == y.size());

    switch (e.type) {
    case vgui_DRAW: {
      glColor3f(1, 1, 1);

      // draw edges
      glLineWidth(1.0);
      glBegin(GL_LINE_LOOP);
      for (unsigned int i=0; i<n; ++i)
        glVertex2f(x[i], y[i]);
      glEnd();

      // draw vertices
      glPointSize(3);
      glBegin(GL_POINTS);
      for (unsigned int i=0; i<n; ++i)
        glVertex2f(x[i], y[i]);
      glEnd();

      // draw interior points
      vgl_polygon<float> p(&x[0], &y[0], n);
      vgl_polygon_scan_iterator<float> si(p, boundary);
      si.reset();
      glColor3f(0, 1, 0); // green
      glBegin(GL_POINTS);
      while (si.next()) {
        int y = si.scany();
        int x0 = si.startx();
        int x1 = si.endx();
        for (int x = x0; x<=x1; ++x)
          glVertex2f(x, y);
      }
      glEnd();

      return true;
    }

    case vgui_MOTION: {
      if (v == -1)
        return false;
      vgui_projection_inspector pi;
      float ix, iy;
      pi.window_to_image_coordinates(e.wx, e.wy, ix, iy);
      assert(0<=v && v<(int)n);
      x[v] = ix;
      y[v] = iy;
      post_redraw();
      return true;
    }

    case vgui_BUTTON_DOWN: {
      if (v >= 0)
        return false;
      vgui_projection_inspector pi;
      float ix, iy;
      pi.window_to_image_coordinates(e.wx, e.wy, ix, iy);
      double bd=0;
      int bi = -1;
      for (unsigned int i=0; i<n; ++i) {
        double d = hypot(ix - x[i], iy - y[i]);
        if (bi == -1 || d<bd) {
          bd = d;
          bi = i;
        }
      }
      v = bi;
      assert(0<=v && v<(int)n);
      x[v] = ix;
      y[v] = iy;
      post_redraw();
      return true;
    }

    case vgui_BUTTON_UP:
      v = -1;
      return true;

    default:
      return false;
    }
  }
};

int main(int argc, char **argv)
{
  vgui::init(argc, argv);
  vul_arg<unsigned> n("-n", "number of vertices", 5);
  vul_arg<bool>     b("-b", "include boundary vertices");
  vul_arg_parse(argc, argv);

  vgui_tableau_sptr tab = new example_ptab(n(), b());
  vgui_viewer2D_new zoom(tab);
  return vgui::run(zoom, 512, 512);
}
