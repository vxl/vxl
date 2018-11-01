// This is core/vgui/internals/vgui_rasterpos.cxx
//:
// \file
// \author fsm

#include "vgui_rasterpos.h"
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vgui/vgui_matrix_state.h>

// Purpose : a RasterPos() routine which sets the raster position,
// even if GL thinks it is invalid.
void vgui_rasterpos4dv(double const X[4])
{
  // This class will restore the matrix state on exit of this
  // function.
  vgui_matrix_state matrix_state;

  GLint vp[4]; // x,y, w,h
  glGetIntegerv(GL_VIEWPORT, vp);

  vnl_matrix_fixed<double,4,4> T = vgui_matrix_state::total_transformation();
  vnl_vector_fixed<double,4> tmp = T * vnl_vector_fixed<double,4>(X);
  double rx = tmp[0]/tmp[3];
  double ry = tmp[1]/tmp[3];

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  //           To set a valid raster position outside the viewport, first
  //          set a valid raster position inside the viewport, then call
  //          glBitmap with NULL as the bitmap parameter and with xmove
  //          and ymove set to the offsets of the new raster position.
  //          This technique is useful when panning an image around the
  //          viewport.
  // (from http://www.opengl.org/developers/faqs/technical/clipping.htm#0070)
  glRasterPos2f(0,0);
  glBitmap(0,0,
           0,0,
           float(rx*vp[2]/2), float(ry*vp[3]/2),
           nullptr);
}


void vgui_rasterpos2f(float x, float y)
{
  double X[4]={x,y,0,1};
  vgui_rasterpos4dv(X);
}


void vgui_rasterpos2i(int x, int y)
{
  double X[4]={double(x),double(y),0.0,1.0};
  vgui_rasterpos4dv(X);
}


bool vgui_rasterpos_valid()
{
  GLboolean params;
  glGetBooleanv(GL_CURRENT_RASTER_POSITION_VALID, &params);
  return params != GL_FALSE;
}
