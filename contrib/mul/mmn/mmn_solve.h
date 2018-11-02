#ifndef mmn_solve_h_
#define mmn_solve_h_
//:
// \file
// \brief Find choice of nodes which minimises Markov problem
// \author Tim Cootes

#include <iostream>
#include <vector>
#include <vnl/vnl_vector.h>
#include <mmn/mmn_arc.h>
#include <mmn/mmn_dependancy.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

double mmn_solve(const std::vector<vnl_vector<double> >& node_cost,
                 const std::vector<vnl_matrix<double> >& pair_cost,
                 const std::vector<mmn_dependancy>& deps,
                 unsigned max_n_arcs,
                 std::vector<unsigned>& x);

#endif // mmn_solve_h_
