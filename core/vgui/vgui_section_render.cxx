/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation
#endif
#include "vgui_section_render.h"

#include <vcl_cmath.h>
#include <vcl_cassert.h>

#include <vgui/internals/vgui_rasterpos.h>
#include <vgui/internals/vgui_accelerate.h>

static float fsm_max(float x, float y) { return x>y ? x : y; }
static float fsm_min(float x, float y) { return x<y ? x : y; }

// Set to 1 for verbose debugging.
#if 0
# define fsm_debug printf
#else
static inline void fsm_debug(char const *, ...) { }
#endif

//
bool vgui_section_render(void const *pixels,
                         unsigned w, unsigned h,
                         float x0, float y0,
                         float x1, float y1,
                         GLenum format,
                         GLenum type,
                         bool use_vgui_rasterpos)
{
  assert(pixels);
  assert(x0 <= x1);
  assert(y0 <= y1);

  // Get current matrix state, in fortran order.
  double Pt[4][4], Mt[4][4];
  glGetDoublev(GL_PROJECTION_MATRIX, &Pt[0][0]);
  glGetDoublev(GL_MODELVIEW_MATRIX,  &Mt[0][0]);

  // Get total world-to-device transformation. It should be of the form :
  // * 0 0 *
  // 0 * 0 *
  // 0 0 * *
  // 0 0 0 *
  // with the diagonal entries non-zero. If not of this form, return false.
  double T[4][4];
  for (unsigned i=0; i<4; ++i) {
    for (unsigned j=0; j<4; ++j) {
      T[i][j] = 0;
      for (unsigned k=0; k<4; ++k)
        T[i][j] += Pt[k][i] * Mt[j][k]; // Pt[k][i] = P[i][k] etc
    }
  }
  if (!T[0][0]|| T[0][1] || T[0][2] ||
      T[1][0] ||!T[1][1] || T[1][2] ||
      T[2][0] || T[2][1] ||!T[2][2] ||
      T[3][0] || T[3][1] || T[3][2] || !T[3][3])
    return false; // cannot do

  // From image to device coordinates, the projection is :
  // [ T00  0  T03 ]   [ a   u ]
  // [  0  T11 T13 ] ~ [   b v ]
  // [  0   0  T33 ]   [     1 ]
  float a = T[0][0]/T[3][3], b = T[1][1]/T[3][3];
  float u = T[0][3]/T[3][3], v = T[1][3]/T[3][3];

  // Get size of viewport. We need this to determine how much to scale pixels by.
  GLint vp[4]; // x,y, w,h
  glGetIntegerv(GL_VIEWPORT, vp);
  //int vp_x = vp[0];
  //int vp_y = vp[1];
  int vp_w = vp[2];
  int vp_h = vp[3];
  if (vp_w <= 0 || vp_h <= 0)
    return true;


  // From device to viewport coordinates, the transformation is :
  // [ vp_w   0  vp_x ] [ 1/2  0  1/2 ]
  // [   0  vp_h vp_y ] [  0  1/2 1/2 ]
  // [   0    0    1  ] [  0   0   1  ]
  // where vp_x, vp_y, vp_w, vp_h are the start, width and height of the viewport.

  // Compute pixel zoom, as passed to glPixelZoom().
  float zoomx = a*vp_w/2;
  float zoomy = b*vp_h/2;

  if (use_vgui_rasterpos) {
    // New way, as worked out by pcp. This is much simpler, but the sub-pixel rendering
    // seems to be broken for some GLs (Solaris OpenGL 1.1) : the image cannot be rendered
    // at non-integer pixel locations. The result is that the image doesn't pan smoothly.
    // fsm@robots
    vgui_rasterpos2f(0, 0); // set raster position
    assert(vgui_rasterpos_valid());
    glPixelZoom( zoomx, zoomy );
    vgui_accelerate::instance()->vgui_glDrawPixels(w, h, format, type, pixels );
    return true; // can almost do
  }

  // Clip the given region [x0, x1] x [y0, y1] to the viewport.
  // In device coordinates, the viewport is [-1, +1] x [-1, +1] so
  // it's easiest to start from that.
  if (a>0) {
    // [ (-1-u)/a, (+1-u)/a ]
    x0 = fsm_max(x0, (-1-u)/a);
    x1 = fsm_min(x1, (+1-u)/a);
  }
  else {
    // [ (+1-u)/a, (-1-u)/a ]
    x0 = fsm_max(x0, (+1-u)/a);
    x1 = fsm_min(x1, (-1-u)/a);
  }
  if (b>0) {
    // [ (-1-v)/b, (+1-v)/b ]
    y0 = fsm_max(y0, (-1-v)/b);
    y1 = fsm_min(y1, (+1-v)/b);
  }
  else {
    // [ (+1-v)/b, (-1-v)/b ]
    y0 = fsm_max(y0, (+1-v)/b);
    y1 = fsm_min(y1, (-1-v)/b);
  }
  if (x0 > x1 || y0 > y1) {
    fsm_debug("nothing to render\n");
    return true; // that's easy.
  }

  // Round up and down so we only render whole pixels (i.e. we render at
  // most the amount asked to render) and so that the raster position we
  // set is valid :
  int i_x0 = int(vcl_ceil (x0)), i_y0 = int(vcl_ceil (y0));
  int i_x1 = int(vcl_floor(x1)), i_y1 = int(vcl_floor(y1));

  // Set raster position
#if 1
  double eps=0.001; // to foil roundoff.
  glRasterPos2d(i_x0 + eps, i_y0 + eps);
#else
  glRasterPos2i(i_x0, i_y0);
#endif
#if defined(__sun__)
  // Solaris GL has odd behaviour.  Sometimes a reasonable raster position is
  // invalid, only when zoom is exactly 1, and the window is smaller than the
  // image...  Anyway, changing the position slightly seems to fix it...
  if (!vgui_rasterpos_valid()) {
    ++i_x0;
    ++i_y0;
    glRasterPos2i(i_x0, i_y0);
  }
#endif
  if (!vgui_rasterpos_valid()) {
    fsm_debug("invalid rasterpos\n");
    return false; //
  }

  // Set pixel transfer characteristics.
  glPixelStorei(GL_UNPACK_ALIGNMENT  , 1);         // use byte alignment for now.
  glPixelStorei(GL_UNPACK_ROW_LENGTH , w);         // size of image rows.
  glPixelStorei(GL_UNPACK_SKIP_PIXELS, i_x0);      // number of pixels to skip on the left.
  glPixelStorei(GL_UNPACK_SKIP_ROWS  , i_y0);      // number of pixels to skip at the bottom.

  glPixelZoom( zoomx, zoomy );
  vgui_accelerate::instance()->vgui_glDrawPixels(i_x1 - i_x0, // Size of pixel rectangle
                                                 i_y1 - i_y0, // to be written to frame buffer.
                                                 format,
                                                 type,
                                                 pixels);

  // Restore default values. FIXME : should restore *previous* values.
  glPixelStorei(GL_UNPACK_ALIGNMENT , 4); // can be 1,2,4 or 8
  glPixelStorei(GL_UNPACK_ROW_LENGTH ,0);
  glPixelStorei(GL_UNPACK_SKIP_ROWS  ,0);
  glPixelStorei(GL_UNPACK_SKIP_PIXELS,0);

  return true; // could do
}

//--------------------------------------------------------------------------------
