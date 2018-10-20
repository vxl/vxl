#ifndef msm_make_grid_h_
#define msm_make_grid_h_
//:
// \file
// \brief Generate points and curves to display a grid
// \author Tim Cootes

#include <msm/msm_points.h>
#include <msm/msm_curve.h>

//: Generate points and curves to display a grid
//  Grid contains nx x ny squares, each of size dx x dy.
//  Creates (nx+1)*(ny+1) points and 2+nx+ny curves (describing the horizontal
//  and vertical lines)
void msm_make_grid(msm_points& points, msm_curves& curves,
                    unsigned nx, double dx,
                    unsigned ny, double dy);

//: Generate points and curves to display a grid
//  Grid contains nx x ny squares, each of size dx x dy.
//  Draw in n_per_seg points along each line
//  Creates (nx+1)*(ny+1)*n_per_seg^2 points and 2+nx+ny curves
//  (describing the horizontal and vertical lines)
void msm_make_grid(msm_points& points, msm_curves& curves,
                   unsigned nx, double dx,
                   unsigned ny, double dy,
                   unsigned n_per_seg);

#endif // msm_make_grid_h_
