// This is vxl/vgl/vgl_clip.cxx

/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation
#endif
#include "vgl_clip.h"
#include <vcl_cmath.h>
#include <vcl_algorithm.h> // for swap

bool vgl_clip_lineseg_to_line(double &x1, double &y1,
            double &x2, double &y2,
            double a,double b,double c) 
{
  double f1 = a*x1+b*y1+c;
  double f2 = a*x2+b*y2+c;
  if (f1<0) {
    if (f2<0)
      return false; // both out
    // 1 out, 2 in
    x1 = (f2*x1 - f1*x2)/(f2-f1);
    y1 = (f2*y1 - f1*y2)/(f2-f1);
    return true;
  }
  else {
    if (f2>0)
      return true;  // both in
    // 1 in, 2 out
    x2 = (f2*x1 - f1*x2)/(f2-f1);
    y2 = (f2*y1 - f1*y2)/(f2-f1);
    return true;
  }
}

bool vgl_clip_line_to_box(double a, double b, double c, // coefficients.
        double x1,double y1,  // bounding
        double x2,double y2,  // box.
        double &bx, double &by,  // start and 
        double &ex, double &ey)  // end points.
{
  if (x1>x2)
    vcl_swap(x1,x2);
  if (y1>y2)
    vcl_swap(y1,y2);

  // I guess this is not really necessary.
  double r = vcl_sqrt(a*a + b*b);
  if (r == 0)
    return false;
  a /= r;
  b /= r;
  c /= r;

  if (vcl_abs(a) > vcl_abs(b)) {
    // more vertical than horizontal
    bx = -(b*y1+c)/a;
    by = y1;
    ex = -(b*y2+c)/a;
    ey = y2;

    if (!vgl_clip_lineseg_to_line(bx,by, ex,ey, double(+1),double(0),-x1))
      return false;
    if (!vgl_clip_lineseg_to_line(bx,by, ex,ey, double(-1),double(0), x2))
      return false;
  }
  else {
    // more horizontal than vertical
    bx = x1;
    by = -(a*x1+c)/b;
    ex = x2;
    ey = -(a*x2+c)/b;

    if (!vgl_clip_lineseg_to_line(bx,by, ex,ey, double(0),double(+1),-y1))
      return false;
    if (!vgl_clip_lineseg_to_line(bx,by, ex,ey, double(0),double(-1), y2))
      return false;
  }

  return true;
}
