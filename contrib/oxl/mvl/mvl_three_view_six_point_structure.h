// This is oxl/mvl/mvl_three_view_six_point_structure.h
#ifndef mvl_three_view_six_point_structure_h_
#define mvl_three_view_six_point_structure_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author fsm@robots.ox.ac.uk
// Code for computing projective structure from 3 views of 6 points.
//
// The six points are assumed to be:
//
//  [ 1 ] [ 0 ] [ 0 ] [ 0 ] [ 1 ]     [ X ]
//  [ 0 ] [ 1 ] [ 0 ] [ 0 ] [ 1 ] and [ Y ] = Q
//  [ 0 ] [ 0 ] [ 1 ] [ 0 ] [ 1 ]     [ Z ]
//  [ 0 ] [ 0 ] [ 0 ] [ 1 ] [ 1 ]     [ T ]
//
// There are up to three solutions.

#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>

struct mvl_three_view_six_point_structure
{
  mvl_three_view_six_point_structure();

  // this flag is currently ignored.
  bool verbose;

  // These matrices have one row per view and 6 columns.
  // The ith view of the jth point is (u(i, j), v(i, j)).
  vnl_matrix<double> u;
  vnl_matrix<double> v;

  // Call this once you have filled in 'u' and 'v'.
  bool compute();

  // output data.
  struct solution_t
  {
    bool valid;
    vnl_matrix<double> P[3]; // camera matrices.
    vnl_vector<double> Q;    // last world point.
  } solution[3];
};

#endif // mvl_three_view_six_point_structure_h_
