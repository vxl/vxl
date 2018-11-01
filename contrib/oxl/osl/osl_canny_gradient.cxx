// This is oxl/osl/osl_canny_gradient.cxx
//:
// \file
// \author fsm

#include <iostream>
#include <cmath>
#include "osl_canny_gradient.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// smooth_  is the (smoothed, presumably) input image.
// dx_, dy_ are the output x, y gradient images.
// grad_    is the output

// FIXME: why does it do dx[x][y] = smooth[x+1][y] - smooth[x-1][y]
//                   but dy[x][y] = smooth[x][y-1] - smooth[x][y+1] ?

void osl_canny_gradient(int xsize_, int ysize_,
                        float const * const * smooth_,
                        float * const * dx_,
                        float * const * dy_,
                        float * const * grad_)
{
  //  Topline
  dy_[0][0]=(smooth_[0][0]-smooth_[0][1])*2;   // left edge
  dx_[0][0]=(smooth_[1][0]-smooth_[0][0])*2;
  for (int x=1; x<xsize_-1; ++x) {
    dy_[x][0]=(smooth_[x  ][0]-smooth_[x  ][1])*2;
    dx_[x][0]= smooth_[x+1][0]-smooth_[x-1][0];
  }   // end for x
  dy_[xsize_-1][0]=(smooth_[xsize_-1][0]-smooth_[xsize_-1][1])*2;   // right edge
  dx_[xsize_-1][0]=(smooth_[xsize_-1][0]-smooth_[xsize_-2][0])*2;


  // Middle lines
  for (int y=1; y<ysize_-1; ++y) {
    dy_[0][y]= smooth_[0][y-1]-smooth_[0][y+1];   // left edge
    dx_[0][y]=(smooth_[1][y  ]-smooth_[0][y  ])*2;
    for (int x=1; x<xsize_-1; ++x) {
      dy_[x][y]=smooth_[x  ][y-1]-smooth_[x  ][y+1];
      dx_[x][y]=smooth_[x+1][y  ]-smooth_[x-1][y  ];
    }   // end for x
    dy_[xsize_-1][y]= smooth_[xsize_-1][y-1]-smooth_[xsize_-1][y+1];   // right edge
    dx_[xsize_-1][y]=(smooth_[xsize_-1][y  ]-smooth_[xsize_-2][y  ])*2;
  }   // end for y


  // Bottom line
  dy_[0][ysize_-1]=(smooth_[0][ysize_-2]-smooth_[0][ysize_-1])*2;   // left edge
  dx_[0][ysize_-1]=(smooth_[1][ysize_-1]-smooth_[0][ysize_-1])*2;
  for (int x=1; x<xsize_-1; ++x) {
    dy_[x][ysize_-1]=(smooth_[x  ][ysize_-2]-smooth_[x  ][ysize_-1])*2;
    dx_[x][ysize_-1]= smooth_[x+1][ysize_-1]-smooth_[x-1][ysize_-1];
  }   // end for x
  dy_[xsize_-1][ysize_-1]=(smooth_[xsize_-1][ysize_-2]-smooth_[xsize_-1][ysize_-1])*2;   // right edge
  dx_[xsize_-1][ysize_-1]=(smooth_[xsize_-1][ysize_-1]-smooth_[xsize_-2][ysize_-1])*2;


  //  Magnitude for entire image
  for (int y=0; y<ysize_; ++y)
    for (int x=0; x<xsize_; ++x)
      grad_[x][y] = (float)std::sqrt(dx_[x][y]*dx_[x][y] + dy_[x][y]*dy_[x][y]);
}

// taken from osl_canny_rothwell.cxx
void osl_canny_gradient_central(int xsize_, int ysize_,
                                float const * const * smooth_,
                                float * const * dx_,
                                float * const * dy_,
                                float * const * grad_)
{
  for (int x=1; x<xsize_-1; ++x) {
    for (int y=1; y<ysize_-1; ++y) {
      dx_[x][y] = smooth_[x+1][y] - smooth_[x-1][y];
      dy_[x][y] = smooth_[x][y+1] - smooth_[x][y-1];
      grad_[x][y] = (float)std::sqrt(dx_[x][y]*dx_[x][y] + dy_[x][y]*dy_[x][y]);
    }
  }
}
