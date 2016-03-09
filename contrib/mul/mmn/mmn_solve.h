#ifndef mmn_solve_h_
#define mmn_solve_h_
//:
// \file
// \brief Find choice of nodes which minimises Markov problem
// \author Tim Cootes

#include <vnl/vnl_vector.h>
#include <mmn/mmn_arc.h>
#include <mmn/mmn_dependancy.h>
#include <vcl_compiler.h>
#include <iostream>
#include <vector>

double mmn_solve(const std::vector<vnl_vector<double> >& node_cost,
                 const std::vector<vnl_matrix<double> >& pair_cost,
                 const std::vector<mmn_dependancy>& deps,
                 unsigned max_n_arcs,
                 std::vector<unsigned>& x);

#endif // mmn_solve_h_
