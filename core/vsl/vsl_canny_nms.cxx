/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation
#endif
#include "vsl_canny_nms.h"
#include <vcl_cmath.h>

//: returns number of edgels found [?]
int vsl_canny_nms(int _xsize, int _ysize, 
		  float * const * _dx, float * const * _dy, float const * const * _grad,
		  float * const *_thick, float * const * _theta) 
{ 
  const float k = float(180.0/3.1415926);
  int n_edgels_NMS = 0; // return value for this function

  for (int y=_ysize-2; y>0; --y) {
    for (int x=_xsize-2; x>0; --x) {
      float del;
      if (vcl_fabs(_dx[x][y])>vcl_fabs(_dy[x][y])) {
	if    (_grad[x][y]<=_grad[x+1][y  ] || _grad[x][y]<_grad[x-1][y  ])
	  continue;
      }
      else if (_grad[x][y]<=_grad[x  ][y-1] || _grad[x][y]<_grad[x  ][y+1])
	continue;
      
      // we have an edge
      float thick = _grad[x][y];
      float theta = k*vcl_atan2(_dx[x][y],_dy[x][y]);
      // theta not to be used to define _theta[x][y]. Only to define orient.
      int orient = ( (int) (theta+202.5) ) / 45;

      float newx = 0.0, newy = 0.0; // Initialise

      // Identify quadrant:
      //                     3   2   1
      //
      //                     4   *   0(8)
      //
      //                     5   6   7
      switch(orient) {
      case 0:   // sort of horizontal
      case 4:
      case 8:
	newx=x+0.5;   // pixel centre
	del  =  _grad[x][y-1]-_grad[x][y+1];
	del /= (_grad[x][y+1]+_grad[x][y-1]-2*_grad[x][y])*2;
	if (del>0.5) continue;
	newy=y+del+0.5;
	break;
      case 2:   // sort of vertical
      case 6:
	newy=y+0.5;
	del  = _grad[x-1][y]-_grad[x+1][y];
	del /= (_grad[x-1][y]+_grad[x+1][y]-2*_grad[x][y])*2;
	if (del>0.5) continue;
	newx=x+del+0.5;
	break;
      case 1:   // sort of left diagonal
      case 5:
	if (_grad[x][y]<=_grad[x+1][y-1] || _grad[x][y]<_grad[x-1][y+1])
	  continue;
	del  = _grad[x-1][y+1]-_grad[x+1][y-1];
	del /= (_grad[x-1][y+1]+_grad[x+1][y-1]-2*_grad[x][y])*2;
	if (del>0.5) continue;
	newy=y-del+0.5;
	newx=x+del+0.5;
	break;
      case 3:   // sort of right diagonal
      case 7:
	if (_grad[x][y]<=_grad[x-1][y-1] || _grad[x][y]<_grad[x+1][y+1])
	  continue;
	del  = _grad[x+1][y+1]-_grad[x-1][y-1];
	del /= (_grad[x+1][y+1]+_grad[x-1][y-1]-2*_grad[x][y])*2;
	if (del>0.5) continue;
	newy=y-del+0.5;
	newx=x-del+0.5;
	break;
      }   // end switch

      // _theta[x][y] as defined in the next line is compatible with
      //  the convention in TargetJr.
      //  The minus sign in front of _dy[x][y] is to change the way
      //  _dy is defined in this vsl_canny_ox (i.e, we want it to be 
      //  [y(i+1) - y(i-1)] rather than [y(i-1) - y(i+1)]). 
      //   See ComputeGradient above. 
      //  _theta[x][y] now stores the normal to the edge tangent. 
      //  Before it stored the tangent to the edge.
      //  _theta[x][y] = theta;  // This how it was defined previously
      _theta[x][y] = k*vcl_atan2(-_dy[x][y],_dx[x][y]);

      _thick[x][y] = thick;
      _dx[x][y] = newx;
      _dy[x][y] = newy;

      n_edgels_NMS++;
    }   // end for x
  }   // end for y
  return n_edgels_NMS;
}
