#ifdef __GNUC__
#pragma implementation
#endif

#include <vcl_iostream.h>
#include "LineSeg.h"

// -- Constructor
LineSeg::LineSeg(float x0, float y0, float x1, float y1, float theta, float grad_mean)
{
  _x0 = x0;
  _y0 = y0;
  _x1 = x1;
  _y1 = y1;
  _theta = theta;
  _grad_mean = grad_mean;  
}

// -- Save to ostream
ostream& operator<<(ostream& s, const LineSeg& l)
{
  return s << l._x0 << " "
	   << l._y0 << " "
	   << l._x1 << " "
	   << l._y1 << " "
	   << l._theta << " "
	   << l._grad_mean << endl;
}

// -- Read from istream
istream& operator>>(istream& s, LineSeg& l)
{
  return s >> l._x0 >> l._y0 >> l._x1 >> l._y1 >> l._theta >> l._grad_mean;
}
