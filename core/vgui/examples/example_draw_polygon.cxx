// This example draws a polygon on the display using OpenGl function calls
// to draw the polygon.  Note, you could achieve the same effect using
// a vgui_easy2D_tableau to draw the polygon and do all the calls to OpenGL
// functions for you.
//
// Example usage:  example_draw_polygon -n 7 -b

#include <cmath>
#include <vector>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vul/vul_arg.h>

#include <vgl/vgl_polygon.h>
#include <vgl/vgl_polygon_scan_iterator.h>

#include <vnl/vnl_math.h>

#include <vgui/vgui.h>
#include <vgui/vgui_gl.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_projection_inspector.h>

#define hypot(x,y) std::sqrt((x)*(x)+(y)*(y)) // hypot is not in C++98, and therefore not in vcl.

struct example_polygon_tableau : public vgui_tableau
{
  bool boundary;
  int v;
  std::vector<float> x;
  std::vector<float> y;

  example_polygon_tableau(unsigned n, bool boundary_)
   : boundary(boundary_), v(-1)
  {
    for (unsigned i=0; i<n; ++i)
    {
      double t = vnl_math::twopi*i/n;
      x.push_back(float(std::cos(t)));
      y.push_back(float(std::sin(t)));
    }
  }

  bool handle(vgui_event const &e)
  {
    unsigned n = x.size(); assert(n == y.size());

    switch (e.type)
    {
    case vgui_DRAW:
    {
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
        int yi = si.scany();
        int x0 = si.startx();
        int x1 = si.endx();
        for (int xi = x0; xi<=x1; ++xi)
          glVertex2f(xi, yi);
      }
      glEnd();

      return true;
    }

    case vgui_MOTION:
    {
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

    case vgui_BUTTON_DOWN:
    {
      if (v >= 0)
        return false;
      vgui_projection_inspector pi;
      float ix, iy;
      pi.window_to_image_coordinates(e.wx, e.wy, ix, iy);
      double bd=0;
      int bi = -1;
      for (unsigned int i=0; i<n; ++i)
      {
        double d = hypot(ix - x[i], iy - y[i]);
        if (bi == -1 || d<bd)
        {
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

  vgui_tableau_sptr tab = new example_polygon_tableau(n(), b());
  vgui_viewer2D_tableau_new zoom(tab);
  return vgui::run(zoom, 512, 512);
}
