#ifndef bdgl_peano_curve_h_
#define bdgl_peano_curve_h_
//-----------------------------------------------------------------------------
//:
// \file
// \author Vishal Jain
// \brief construction of Peano curve on a square grid.
//
// \verbatim
//  Modifications
//   Initial version July 6th, 2009
// \endverbatim
//
//-----------------------------------------------------------------------------
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vcl_algorithm.h>

vcl_vector<vgl_point_2d<double> > rotate_2d(vcl_vector<vgl_point_2d<double> > in, double angle);
vcl_vector<vgl_point_2d<double> > trans_2d(vcl_vector<vgl_point_2d<double> > in, double tx, double ty);
vcl_vector<vgl_point_2d<double> > scale_2d(vcl_vector<vgl_point_2d<double> > in, double s);

//: Peano curve inscribed in a square of length 4 centered at the origin.
vcl_vector<vgl_point_2d<double> >  recurse_peano_curve(unsigned level);

//: peano_curve on cube
vcl_vector<vgl_point_3d<double> >  peano_curve_on_cube(unsigned level);

#endif
