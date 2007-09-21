#ifndef mmn_dp_solver_h_
#define mmn_dp_solver_h_

//:
// \file
// \brief Find choice of values at each node which minimises Markov problem
// \author Tim Cootes

#include <mmn/mmn_arc.h>
#include <mmn/mmn_dependancy.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vcl_vector.h>

//: Find choice of values at each node which minimises Markov problem.
//  Minimises F() = sum node_costs + sum pair_costs
//
//  Assumes graph defining relationships can be fully decomposed by
//  repeatedly removing any nodes with two or fewer neighbours.
//  Global optimum is found using a series of sequential exhaustive
//  optimisations.
class mmn_dp_solver
{
private:
  //: Workspace for incremental costs of each node
  vcl_vector<vnl_vector<double> > nc_;

  //: Workspace for incremental costs of each arc
  vcl_vector<vnl_matrix<double> > pc_;

  //: index1[i][j] = optimal choice for i if other node is j
  vcl_vector<vcl_vector<unsigned> > index1_;

  // index2[i](j,k) = optimal choice of i if two other nodes are (j,k)
  vcl_vector<vnl_matrix<int> > index2_;

  //: Dependancies
  vcl_vector<mmn_dependancy> deps_;

  void process_dep1(const mmn_dependancy& dep);
  void process_dep2(const mmn_dependancy& dep);
public:
  //: Default constructor
  mmn_dp_solver();

  //: Index of root node
  unsigned root() const;

  //: Define dependancies
  void set_dependancies(const vcl_vector<mmn_dependancy>& deps,
                        unsigned n_nodes, unsigned max_n_arcs);

  //: Find values for each node with minimise the total cost
  //  \param node_cost: node_cost[i][j] is cost of selecting value j for node i
  //  \param pair_cost: pair_cost[a](i,j) is cost of selecting values (i,j) for nodes at end of arc a.  
  //  \param x: On exit, x[i] gives choice for node i
  // NOTE: If arc a connects nodes v1,v2, the associated pair_cost is ordered
  // with the node with the lowest index being the first parameter.  Thus if
  // v1 has value i1, v2 has value i2, then the cost of this choice is
  // (v1<v2?pair_cost(i1,i2):pair_cost(i2,i1))
  // Returns the minimum cost
  double solve(const vcl_vector<vnl_vector<double> >& node_cost,
                 const vcl_vector<vnl_matrix<double> >& pair_cost,
                 vcl_vector<unsigned>& x);

  //: Compute optimal values for x[i] given that root node is root_value
  //  Assumes that solve() has been already called.
  void backtrace(unsigned root_value,vcl_vector<unsigned>& x);

  //: root_cost()[i] is cost of selecting value i for the root node
  const vnl_vector<double>& root_cost() const { return nc_[root()]; }

};

#endif // mmn_dp_solver_h_
