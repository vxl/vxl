// This is oxl/mvl/mvl_six_point_design_matrix_row.h
#ifndef mvl_six_point_design_matrix_row_h_
#define mvl_six_point_design_matrix_row_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author fsm@robots.ox.ac.uk

#include <vnl/vnl_matrix.h>

void mvl_six_point_design_matrix_row(vnl_matrix<double> const &A,
                                     vnl_matrix<double> const &B,
                                     double u, double v,
                                     double out_row[5]);

#endif // mvl_six_point_design_matrix_row_h_
