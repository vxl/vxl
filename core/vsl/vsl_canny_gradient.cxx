/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation
#endif
#include "vsl_canny_gradient.h"
#include <vcl/vcl_cmath.h>

// _smooth  is the (smoothed, presumably) input image.
// _dx, _dy are the output x, y gradient images.
// _grad    is the output 

// FIXME: why does it do dx[x][y] = smooth[x+1][y] - smooth[x-1][y]
//                   but dy[x][y] = smooth[x][y-1] - smooth[x][y+1] ?

void vsl_canny_gradient(int _xsize, int _ysize, 
			float const * const * _smooth,
			float * const * _dx, 
			float * const * _dy, 
			float * const * _grad)
{ 

  //  Topline
  _dy[0][0]=(_smooth[0][0]-_smooth[0][1])*2;   // left edge
  _dx[0][0]=(_smooth[1][0]-_smooth[0][0])*2;
  for (int x=1; x<_xsize-1; ++x) {
    _dy[x][0]=(_smooth[x  ][0]-_smooth[x  ][1])*2;
    _dx[x][0]= _smooth[x+1][0]-_smooth[x-1][0];
  }   // end for x
  _dy[_xsize-1][0]=(_smooth[_xsize-1][0]-_smooth[_xsize-1][1])*2;   // right edge
  _dx[_xsize-1][0]=(_smooth[_xsize-1][0]-_smooth[_xsize-2][0])*2;


  // Middle lines
  for (int y=1; y<_ysize-1; ++y) {
    _dy[0][y]= _smooth[0][y-1]-_smooth[0][y+1];   // left edge
    _dx[0][y]=(_smooth[1][y  ]-_smooth[0][y  ])*2;
    for (int x=1; x<_xsize-1; ++x) {
      _dy[x][y]=_smooth[x  ][y-1]-_smooth[x  ][y+1];
      _dx[x][y]=_smooth[x+1][y  ]-_smooth[x-1][y  ];
    }   // end for x
    _dy[_xsize-1][y]= _smooth[_xsize-1][y-1]-_smooth[_xsize-1][y+1];   // right edge
    _dx[_xsize-1][y]=(_smooth[_xsize-1][y  ]-_smooth[_xsize-2][y  ])*2;
  }   // end for y


  // Bottom line
  _dy[0][_ysize-1]=(_smooth[0][_ysize-2]-_smooth[0][_ysize-1])*2;   // left edge
  _dx[0][_ysize-1]=(_smooth[1][_ysize-1]-_smooth[0][_ysize-1])*2;
  for (int x=1; x<_xsize-1; ++x) {
    _dy[x][_ysize-1]=(_smooth[x  ][_ysize-2]-_smooth[x  ][_ysize-1])*2;
    _dx[x][_ysize-1]= _smooth[x+1][_ysize-1]-_smooth[x-1][_ysize-1];
  }   // end for x
  _dy[_xsize-1][_ysize-1]=(_smooth[_xsize-1][_ysize-2]-_smooth[_xsize-1][_ysize-1])*2;   // right edge
  _dx[_xsize-1][_ysize-1]=(_smooth[_xsize-1][_ysize-1]-_smooth[_xsize-2][_ysize-1])*2;


  //  Magnitude for entire image
  for (int y=0; y<_ysize; ++y) 
    for (int x=0; x<_xsize; ++x) 
      _grad[x][y] = (float)sqrt(_dx[x][y]*_dx[x][y] + _dy[x][y]*_dy[x][y]);
}

// taken from vsl_canny_rothwell.cxx
void vsl_canny_gradient_central(int _xsize, int _ysize, 
				float const * const * _smooth,
				float * const * _dx, 
				float * const * _dy, 
				float * const * _grad)
{
  for (int x=1; x<_xsize-1; ++x) {
    for (int y=1; y<_ysize-1; ++y) {
      _dx[x][y] = _smooth[x+1][y] - _smooth[x-1][y];
      _dy[x][y] = _smooth[x][y+1] - _smooth[x][y-1];
      _grad[x][y] = (float)sqrt(_dx[x][y]*_dx[x][y] + _dy[x][y]*_dy[x][y]);
    }
  }
}
