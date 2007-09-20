#ifndef mmn_solve_h_
#define mmn_solve_h_

//:
// \file
// \brief Find choice of nodes which minimises Markov problem
// \author Tim Cootes

#include <vnl/vnl_vector.h>
#include <mmn/mmn_arc.h>
#include <mmn/mmn_dependancy.h>
#include <vcl_vector.h>
#include <vcl_utility.h>  // For vcl_pair

double mmn_solve(const vcl_vector<vnl_vector<double> >& node_cost,
                 const vcl_vector<vnl_matrix<double> >& pair_cost,
                 const vcl_vector<mmn_dependancy>& deps,
                 unsigned max_n_arcs,
                 vcl_vector<unsigned>& x);

#endif // mmn_solve_h_
