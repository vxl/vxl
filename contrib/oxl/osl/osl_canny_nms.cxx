// This is oxl/osl/osl_canny_nms.cxx
//:
// \file
// \author fsm

#include <iostream>
#include <cmath>
#include "osl_canny_nms.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_math.h>

//: returns number of edgels found [?]
unsigned int osl_canny_nms(int xsize_, int ysize_,
                           float * const * dx_, float * const * dy_, float const * const * grad_,
                           float * const *thick_, float * const * theta_)
{
  const auto k = float(vnl_math::deg_per_rad);
  unsigned int n_edgels_NMS = 0; // return value for this function

  for (int y=ysize_-2; y>0; --y) {
    for (int x=xsize_-2; x>0; --x) {
      float del;
      if (std::fabs(dx_[x][y])>std::fabs(dy_[x][y])) {
        if    (grad_[x][y]<=grad_[x+1][y  ] || grad_[x][y]<grad_[x-1][y  ])
          continue;
      }
      else if (grad_[x][y]<=grad_[x  ][y-1] || grad_[x][y]<grad_[x  ][y+1])
        continue;

      // we have an edge
      float thick = grad_[x][y];
      float theta = k*(float)std::atan2(dx_[x][y],dy_[x][y]);
      // theta not to be used to define theta_[x][y]. Only to define orient.
      int orient = ( (int) (theta+202.5) ) / 45; orient %= 8;

      float newx = 0.0f, newy = 0.0f; // Initialise

      // Identify quadrant:
      //                     3   2   1
      //
      //                     4   *   0
      //
      //                     5   6   7
      switch (orient)
      {
       case 0:   // sort of horizontal
       case 4:
        newx=x+0.5f;   // pixel centre
        del  =  grad_[x][y-1]-grad_[x][y+1];
        del /= (grad_[x][y+1]+grad_[x][y-1]-2*grad_[x][y])*2;
        if (del>0.5f) continue;
        newy=y+del+0.5f;
        break;
       case 2:   // sort of vertical
       case 6:
        newy=y+0.5f;
        del  = grad_[x-1][y]-grad_[x+1][y];
        del /= (grad_[x-1][y]+grad_[x+1][y]-2*grad_[x][y])*2;
        if (del>0.5f) continue;
        newx=x+del+0.5f;
        break;
       case 1:   // sort of left diagonal
       case 5:
        if (grad_[x][y]<=grad_[x+1][y-1] || grad_[x][y]<grad_[x-1][y+1])
          continue;
        del  = grad_[x-1][y+1]-grad_[x+1][y-1];
        del /= (grad_[x-1][y+1]+grad_[x+1][y-1]-2*grad_[x][y])*2;
        if (del>0.5f) continue;
        newy=y-del+0.5f;
        newx=x+del+0.5f;
        break;
       case 3:   // sort of right diagonal
       case 7:
        if (grad_[x][y]<=grad_[x-1][y-1] || grad_[x][y]<grad_[x+1][y+1])
          continue;
        del  = grad_[x+1][y+1]-grad_[x-1][y-1];
        del /= (grad_[x+1][y+1]+grad_[x-1][y-1]-2*grad_[x][y])*2;
        if (del>0.5f) continue;
        newy=y-del+0.5f;
        newx=x-del+0.5f;
        break;
       default: // this cannot be reached
        break;
      }   // end switch

      // theta_[x][y] as defined in the next line is compatible with
      //  the convention in TargetJr.
      //  The minus sign in front of dy_[x][y] is to change the way
      //  dy_ is defined in this osl_canny_ox (i.e, we want it to be
      //  [y(i+1) - y(i-1)] rather than [y(i-1) - y(i+1)]).
      //   See ComputeGradient above.
      //  theta_[x][y] now stores the normal to the edge tangent.
      //  Before it stored the tangent to the edge.
      //  theta_[x][y] = theta;  // This how it was defined previously
      theta_[x][y] = k*(float)std::atan2(-dy_[x][y],dx_[x][y]);

      thick_[x][y] = thick;
      dx_[x][y] = newx;
      dy_[x][y] = newy;

      ++n_edgels_NMS;
    }   // end for x
  }   // end for y
  return n_edgels_NMS;
}
