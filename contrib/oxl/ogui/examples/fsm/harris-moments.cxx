/*
  fsm
*/
#include <vcl_cmath.h>
#include <vcl_iostream.h>

#include <vul/vul_arg.h>

#include <vnl/algo/vnl_svd.h>
#include <vnl/vnl_math.h>

#include <vil/vil_load.h>
#include <vil/vil_image.h>
#include <vil/vil_interpolate.h>
#include <vil/vil_memory_image_of.h>

#include <osl/osl_harris.h>

#include <vgui/vgui.h>
#include <vgui/vgui_gl.h>
#include <vgui/vgui_deck_tableau.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_viewer2D.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_projection_inspector.h>

struct example_mytab : public vgui_image_tableau
{
  typedef vgui_image_tableau base;

  float pos_x, pos_y;
  vil_memory_image_of<float> fxx;
  vil_memory_image_of<float> fxy;
  vil_memory_image_of<float> fyy;

  example_mytab(vil_image const &I, double sigma) : base(I) {
    osl_harris_params params;
    params.gauss_sigma = sigma;
    osl_harris harris(params);
    harris.compute(I);
    fxx = harris.image_fxx_buf;
    fxy = harris.image_fxy_buf;
    fyy = harris.image_fyy_buf;
  }

  void draw_ellipse() {
    GLboolean is_index_mode;
    glGetBooleanv(GL_INDEX_MODE, &is_index_mode);
    if (is_index_mode)
      glIndexi(4); // 100 in binary
    else
      glColor3f(1, 0, 0); // ref

    int x = (int) vcl_floor(pos_x);
    int y = (int) vcl_floor(pos_y);
    if (fxx.in_range(x, y) && fxx.in_range(x+1, y+1)) {
      vnl_matrix<double> S(2, 2);
      vil_interpolate_bilinear(fxx, pos_x, pos_y, &S[0][0]);
      vil_interpolate_bilinear(fxy, pos_x, pos_y, &S[0][1]);
      vil_interpolate_bilinear(fxy, pos_x, pos_y, &S[1][0]);
      vil_interpolate_bilinear(fyy, pos_x, pos_y, &S[1][1]);

      vnl_svd<double> svd(S);
      double l1 = vcl_sqrt(svd.W(0, 0))/64;
      double l2 = vcl_sqrt(svd.W(1, 1))/64;
      double ex = svd.U(0, 0);
      double ey = svd.U(1, 0);

      glLineWidth(2);
      glBegin(GL_LINES);
      // major axis
      glVertex2d(pos_x + l1*ex, pos_y + l1*ey);
      glVertex2d(pos_x - l1*ex, pos_y - l1*ey);
      // minor axis
      glVertex2d(pos_x - l2*ey, pos_y + l2*ex);
      glVertex2d(pos_x + l2*ey, pos_y - l2*ex);
      glEnd();

      glLineWidth(1);
      glBegin(GL_LINE_LOOP);
      unsigned N = 30; //unsigned( 10*(l1+l2)/2 );
      for (unsigned i=0; i<N; ++i) {
        double t = 2*vnl_math::pi*i/N;
        double c = vcl_cos(t);
        double s = vcl_sin(t);
        glVertex2d(pos_x + c*l1*ex + s*l2*ey, pos_y + c*l1*ey - s*l2*ex);
      }
      glEnd();
    }
    else {
#if 0
      glPointSize(5);
      glBegin(GL_POINTS);
      glVertex2f(pos_x, pos_y);
      glEnd();
#endif
    }
  }

  bool handle(vgui_event const &e) {
    switch (e.type) {

#if 1 // use overlays :
    case vgui_DRAW:
      base::handle(e);
      post_overlay_redraw();
      return true;

    case vgui_DRAW_OVERLAY:
      draw_ellipse();
      return true;

    case vgui_MOTION: {
      vgui_projection_inspector pi;
      float ix, iy;
      pi.window_to_image_coordinates(e.wx, e.wy, ix, iy);
      if (pos_x != ix || pos_y != iy) {
        post_overlay_redraw();
        pos_x = ix;
        pos_y = iy;
      }
    } return true;

#else // no overlays :
    case vgui_DRAW:
      base::handle(e);
      draw_ellipse();
      return true;

    case vgui_MOTION: {
      vgui_projection_inspector pi;
      float ix, iy;
      pi.window_to_image_coordinates(e.wx, e.wy, ix, iy);
      if (pos_x != ix || pos_y != iy) {
        post_redraw();
        pos_x = ix;
        pos_y = iy;
      }
    } return true;
#endif

    default:
      return base::handle(e);
    }
  }
};

int main(int argc, char **argv)
{
  vgui::init(argc, argv);
  vul_arg<double> sigma("-sigma", "", 0.7);
  vul_arg_parse(argc, argv);

  int w=0, h=0;

  vgui_deck_tableau_new deck;
  for (int n=1; n<argc; ++n) {
    vil_image I = vil_load(argv[n]);
    vcl_cerr << I << vcl_endl;

    if (!I) {
      vcl_cerr << "failed to load image" << vcl_endl;
      continue;
    }
    deck->add(new example_mytab(I, sigma()));
    if (w < I.width()) w = I.width();
    if (h < I.height()) h = I.height();
  }

  vgui_viewer2D_new zoom(deck);
  return vgui::run(zoom, w, h);
}
