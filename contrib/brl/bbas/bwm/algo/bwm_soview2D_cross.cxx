#include "bwm_soview2D_cross.h"

void bwm_soview2D_cross::draw() const
{
#ifdef DEBUG
  vcl_cerr << "bwm_soview2D_cross::draw() line id=" << id << '\n';
#endif

#if 0
  glBegin(GL_LINES);
  // horizantal lines of cross
  glVertex2f(x-r_,y);
  glVertex2f(x-0.5,y);
  glVertex2f(x+0.5,y);
  glVertex2f(x+r_,y);

  // vertical lines of cross
  glVertex2f(x,y-r_);
  glVertex2f(x,y-0.5);
  glVertex2f(x,y+0.5);
  glVertex2f(x,y+r_);
  glEnd();
#endif // 0
  GLint unpack;
  glGetIntegerv(GL_UNPACK_ALIGNMENT, &unpack);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  glRasterPos2d(x, y);
  glBitmap(16, 16, 8, 8, 0, 0, cross_bmp);

  glPixelStorei(GL_UNPACK_ALIGNMENT, unpack);
}
