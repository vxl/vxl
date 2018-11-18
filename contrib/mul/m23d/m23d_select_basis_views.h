//:
// \file
// \author Tim Cootes
// \brief Select a subset most suitable for use as a basis set

#ifndef m23d_select_basis_views_h_
#define m23d_select_basis_views_h_

#include <iostream>
#include <vector>
#include <vnl/vnl_matrix.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Select a subset most suitable for use as a basis set
//  Data matrix is 2ns x np (ns= number of samples, np = number of points)
//  Each two rows gives the points in a single view.
//  This returns a list of point indices for views which have most
//  independent points, and are thus suitable for defining the basis.
//  Randomly generates n_tries possible bases and selects the best.
//  Note: First view (1st two rows) always included, as this often defines
//  the default viewing angle.
std::vector<unsigned> m23d_select_basis_views(const vnl_matrix<double>& P2D,
                                             unsigned n_modes,
                                             unsigned n_tries=1000);

//: Evaluate quality of a basis set
//  Data matrix is 2ns x np (ns= number of samples, np = number of points)
//  Each two rows gives the points in a single view.
//  Form a basis from the pairs of rows defined by selected, and compute
//  a measure of how independent the rows are - the smallest SV/largest SV
double m23d_evaluate_basis(const vnl_matrix<double>& P2D,
                           const std::vector<unsigned>& selected);

#endif // m23d_select_basis_views_h_
