// This is core/vgui/vgui_section_render.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm

#include <cmath>
#include "vgui_section_render.h"

#include <vcl_compiler.h>
#include <cassert>
#include <climits>// for UCHAR_MAX
// not used? #include <iostream>
#include <vgui/internals/vgui_rasterpos.h>
#include <vgui/internals/vgui_accelerate.h>

static inline float fsm_max(float x, float y) { return x>y ? x : y; }
static inline float fsm_min(float x, float y) { return x<y ? x : y; }

// Set to 1 for verbose debugging.
#if 0
#include <cstdio>
# define fsm_debug std::printf
#else
static inline void fsm_debug(char const *, ...) { }
#endif
static bool clamped_viewport(float x0, float y0, float x1, float y1,
                             unsigned& i0, unsigned& j0,
                             unsigned& ni, unsigned& nj,
                             float& zoomx, float& zoomy)
{
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
  //
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
  float a = float(T[0][0]/T[3][3]), b = float(T[1][1]/T[3][3]);
  float u = float(T[0][3]/T[3][3]), v = float(T[1][3]/T[3][3]);

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
 zoomx = a*vp_w/2;
 zoomy = b*vp_h/2;

  // Clip the given region [x0, x1] x [y0, y1] to the viewport.  In
  // device coordinates, the viewport is [-1, +1] x [-1, +1] so it's
  // easiest to start from that. This clipping is especially important
  // for non-local displays, where the display clipping happens on the
  // display server.
  //
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

  // Before dumping the image, we have to set a valid raster
  // position. However, to get a smooth panning effect, we want to
  // render the (potentially) partial pixels at the borders, which
  // means we must get the raster position to an "invalid" position
  // outside the viewport. vgui_rasterpos wraps the appropriate
  // trickery.

  int i_x0 = int(std::floor(x0)), i_y0 = int(std::floor(y0));
  int i_x1 = int(std::ceil (x1)), i_y1 = int(std::ceil (y1));
  //Set the raster position
  vgui_rasterpos2i( i_x0, i_y0 );
  //return the view parameters
  i0 = static_cast<unsigned>(i_x0);   ni = static_cast<unsigned>(i_x1-i_x0);
  j0 = static_cast<unsigned>(i_y0);   nj = static_cast<unsigned>(i_y1-i_y0);
  return true;
}

bool pixel_view(unsigned& i0, unsigned& ni, unsigned& j0, unsigned& nj,
                float& zoomx, float& zoomy)
{
  float x0 = i0, x1 = ni, y0 = j0, y1 = nj;
  return clamped_viewport(x0, y0, x1, y1, i0, j0, ni, nj, zoomx, zoomy);
}

static void GL_setup(GLenum format, GLenum type , bool hardware_map,
                     GLint& alignment, GLint& row_length, GLint& skip_pixels,
                     GLint& skip_rows, GLint& table_size, GLboolean& map_color,
                     vbl_array_1d<float>* fLmap,  vbl_array_1d<float>* fRmap,
                     vbl_array_1d<float>* fGmap,  vbl_array_1d<float>* fBmap,
                     vbl_array_1d<float>* fAmap)
{
  glGetIntegerv(GL_UNPACK_ALIGNMENT,   &alignment);
  glGetIntegerv(GL_UNPACK_ROW_LENGTH,  &row_length);
   glGetIntegerv(GL_UNPACK_SKIP_PIXELS, &skip_pixels);
  glGetIntegerv(GL_UNPACK_SKIP_ROWS,   &skip_rows);
  glGetIntegerv(GL_MAX_PIXEL_MAP_TABLE, &table_size);
  glGetBooleanv(GL_MAP_COLOR, &map_color);
  //If hardware mapping is requested, set up the maps
  //only support color mapping for byte component type
  if (hardware_map&&type == GL_UNSIGNED_BYTE&&table_size>=(UCHAR_MAX+1))
  {
    if (format == GL_LUMINANCE)
    {
      glPixelTransferi(GL_MAP_COLOR, GL_TRUE);
      float* tfLmap = fLmap->begin();
      glPixelMapfv(GL_PIXEL_MAP_R_TO_R, UCHAR_MAX, tfLmap);
      glPixelMapfv(GL_PIXEL_MAP_G_TO_G, UCHAR_MAX, tfLmap);
      glPixelMapfv(GL_PIXEL_MAP_B_TO_B, UCHAR_MAX, tfLmap);
    }
    else if (format == GL_RGB)
    {
      glPixelTransferi(GL_MAP_COLOR, GL_TRUE);
      float* tfRmap = fRmap->begin();
      float* tfGmap = fGmap->begin();
      float* tfBmap = fBmap->begin();
      glPixelMapfv(GL_PIXEL_MAP_R_TO_R, UCHAR_MAX, tfRmap);
      glPixelMapfv(GL_PIXEL_MAP_G_TO_G, UCHAR_MAX, tfGmap);
      glPixelMapfv(GL_PIXEL_MAP_B_TO_B, UCHAR_MAX, tfBmap);
    }
    else if (format == GL_RGBA)
    {
      glPixelTransferi(GL_MAP_COLOR, GL_TRUE);
      float* tfRmap = fRmap->begin();
      float* tfGmap = fGmap->begin();
      float* tfBmap = fBmap->begin();
      float* tfAmap = fAmap->begin();
      glPixelMapfv(GL_PIXEL_MAP_R_TO_R, UCHAR_MAX, tfRmap);
      glPixelMapfv(GL_PIXEL_MAP_G_TO_G, UCHAR_MAX, tfGmap);
      glPixelMapfv(GL_PIXEL_MAP_B_TO_B, UCHAR_MAX, tfBmap);
      glPixelMapfv(GL_PIXEL_MAP_A_TO_A, UCHAR_MAX, tfAmap);
    }
  }
  if (hardware_map&&format == GL_LUMINANCE&&type == GL_UNSIGNED_SHORT
      && table_size>=USHRT_MAX)
  {
    glPixelTransferi(GL_MAP_COLOR, GL_TRUE);
    float* tfLmap = fLmap->begin();
    glPixelMapfv(GL_PIXEL_MAP_R_TO_R, USHRT_MAX, tfLmap);
    glPixelMapfv(GL_PIXEL_MAP_G_TO_G, USHRT_MAX, tfLmap);
    glPixelMapfv(GL_PIXEL_MAP_B_TO_B, USHRT_MAX, tfLmap);
  }
}

static void GL_restore(GLboolean& map_color, GLint alignment, GLint row_length,
                       GLint skip_pixels, GLint skip_rows)
{
  // Restore previous values.
  glPixelTransferi(GL_MAP_COLOR, map_color);
  glPixelStorei(GL_UNPACK_ALIGNMENT,   alignment);
  glPixelStorei(GL_UNPACK_ROW_LENGTH,  row_length);
  glPixelStorei(GL_UNPACK_SKIP_ROWS  , skip_pixels);
  glPixelStorei(GL_UNPACK_SKIP_PIXELS, skip_rows);
}

bool vgui_section_render(void const *pixels,
                         unsigned w, unsigned h, // Size of image.
                         float x0, float y0,  // Region of image
                         float x1, float y1,  // to render.
                         GLenum format,
                         GLenum type ,
                         bool hardware_map,
                         vbl_array_1d<float>* fLmap,
                         vbl_array_1d<float>* fRmap,
                         vbl_array_1d<float>* fGmap,
                         vbl_array_1d<float>* fBmap,
                         vbl_array_1d<float>* fAmap)
{
  assert(h>0);//eliminates warning of unused h
  assert(pixels);
  assert(x0 <= x1);
  assert(y0 <= y1);

  assert(!hardware_map||format != GL_LUMINANCE||fLmap);
  assert(!hardware_map||format != GL_RGB||(fRmap&&fGmap&&fBmap));
  assert(!hardware_map||format != GL_RGBA||(fRmap&&fGmap&&fBmap&&fAmap));
  float zoomx=1.0f, zoomy=1.0f;
  unsigned i0=0, j0=0, ni=0, nj=0;
  if (!clamped_viewport(x0, y0, x1, y1, i0, j0, ni, nj, zoomx, zoomy))
    return false;
  int i_x0 = i0, i_y0 = j0, i_x1 = i0+ni, i_y1 = j0+nj;
  // Store old transfer characteristics for restoring it in a bit.
  GLint alignment, row_length, skip_pixels, skip_rows, table_size;
  GLboolean map_color;
  GL_setup(format, type, hardware_map, alignment, row_length,
           skip_pixels, skip_rows, table_size, map_color, fLmap, fRmap,
           fGmap, fBmap, fAmap);
  // Set pixel transfer characteristics.
  glPixelStorei(GL_UNPACK_ALIGNMENT,   1);         // use byte alignment for now.
  glPixelStorei(GL_UNPACK_ROW_LENGTH,  w);         // size of image rows.
  glPixelStorei(GL_UNPACK_SKIP_PIXELS, i_x0);      // number of pixels to skip on the left.
  glPixelStorei(GL_UNPACK_SKIP_ROWS,   i_y0);      // number of pixels to skip at the bottom.

  glPixelZoom( zoomx, zoomy );
  vgui_accelerate::instance()->vgui_glDrawPixels(i_x1 - i_x0, // Size of pixel rectangle
                                                 i_y1 - i_y0, // to be written to frame buffer.
                                                 format,
                                                 type,
                                                 pixels);
  GL_restore(map_color, alignment, row_length, skip_pixels, skip_rows);
  return true; // could do
}

bool vgui_view_render(void const *pixels,
                      unsigned w, unsigned h, // Size of view
                      float zoomx, float zoomy,
                      GLenum format,
                      GLenum type ,
                      bool hardware_map,
                      vbl_array_1d<float>* fLmap,
                      vbl_array_1d<float>* fRmap,
                      vbl_array_1d<float>* fGmap,
                      vbl_array_1d<float>* fBmap,
                      vbl_array_1d<float>* fAmap)
{
  assert(pixels);
  assert(!hardware_map||format != GL_LUMINANCE||fLmap);
  assert(!hardware_map||format != GL_RGB||(fRmap&&fGmap&&fBmap));
  assert(!hardware_map||format != GL_RGBA||(fRmap&&fGmap&&fBmap&&fAmap));

  // Store old transfer characteristics for restoring it in a bit.
  GLint alignment, row_length, table_size, skip_pixels, skip_rows;
  GLboolean map_color;
  GL_setup(format, type, hardware_map, alignment, row_length,
           skip_pixels, skip_rows, table_size, map_color, fLmap, fRmap,
           fGmap, fBmap, fAmap);
  // Set pixel transfer characteristics.
  glPixelStorei(GL_UNPACK_ALIGNMENT,   1);         // use byte alignment for now.
  glPixelStorei(GL_UNPACK_ROW_LENGTH,  w);         // size of image rows.
  glPixelZoom( zoomx+0.001f, zoomy+0.001f );       // something weird happens
                                                   // for identity zoom
  vgui_accelerate::instance()->vgui_glDrawPixels(w, // Size of pixel rectangle
                                                 h, // to be written to frame buffer.
                                                 format,
                                                 type,
                                                 pixels);

  GL_restore(map_color, alignment, row_length, skip_pixels, skip_rows);
  return true; // could do
}

//--------------------------------------------------------------------------------
