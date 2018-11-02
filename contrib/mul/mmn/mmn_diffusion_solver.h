#ifndef mmn_diffusion_solver_h_
#define mmn_diffusion_solver_h_
//:
// \file
// \brief Run diffusion algorithm over the graph
// \author Martin Roberts

#include <vector>
#include <iostream>
#include <map>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <mmn/mmn_arc.h>
#include <mmn/mmn_graph_rep1.h>

//: Run diffusion algorithm to solve max sum problem
// See  T Werner. A Linear Programming Approach to Max-sum problem: A review;
// IEEE Trans on Pattern Recog & Machine Intell, July 2007
// Try and solve the max-sum problem by performing node-pencil averaging over the graph
// I.e. transform to equivalent problem by adding "potentials" to nodes and subtracting them
// from arcs. This is done to equalise node costs and the cost of the maximal connecting arcs
// If this converges the solution is to take the maximal nodes (which will then be arc-consistent).

class mmn_diffusion_solver
{
 private:
    //: in below the map is indexed by the neighbour's node id

    //: Inner vector indexed by source node state ID, map by neighbour node (t')
    typedef std::map<unsigned,vnl_vector<double > > potential_set_t;

    //: Matrix referenced by [source node state ID][target node state ID]
    // Map ID is target node ID
    typedef std::map<unsigned, vnl_matrix<double > > neigh_arc_cost_t;

    //:Store in graph form (so each node's neighbours are conveniently to hand)
    mmn_graph_rep1 graph_;

    //: The arcs from which graph was generated
    std::vector<mmn_arc> arcs_;

    //: Total number of nodes
    unsigned nnodes_;

    //: Workspace for costs of each arc
    std::vector<neigh_arc_cost_t > arc_costs_;

    //: Workspace for transformed costs of each arc
    std::vector<neigh_arc_cost_t > arc_costs_phi_;


    //: All the potentials at previous iteration (vector index is source node)
    std::vector<potential_set_t > phi_;

    //: Update potentials calculated during this iteration (vector index is source node)
    std::vector<potential_set_t > phi_upd_;

    //: Node costs (outer vector is node ID, inner vnl_vector is by state value)
    std::vector<vnl_vector<double> > node_costs_;

    //: Node costs after phi transform (outer vector is node ID, inner vnl_vector is by state value)
    std::vector<vnl_vector<double> > node_costs_phi_;

    //: Workspace for adjustment to potential
    std::vector<std::map<unsigned,vnl_vector<double > > > u_;

    //: Current iteration count
    unsigned count_;

    //: Max change in any potential value over this iteration
    double max_delta_;

    //: max number of iterations allowed
    unsigned max_iterations_;

    //: min iterations allowed before additional convergence checks
    unsigned min_iterations_;

    //: Convergence criterion on max_delta_
    double epsilon_;

    //:verbose debug output
    bool verbose_;

    //: Previous solution value at arc consistency check
    double soln_val_prev_;

    //: Count of number of times solution value was unchanged
    unsigned nConverging_;

    //: Max count of nConverging_
    static unsigned gNCONVERGED;

    //: Period at which arc consistency of solution is checked for
    static unsigned gACS_CHECK_PERIOD;

    //: Check if we carry on
    bool continue_diffusion();

    //: Update all messages from input node to its neighbours
    void update_potentials_to_neighbours(unsigned inode,
                                         const vnl_vector<double>& node_cost);

    //: Update all node and arc costs (equivalent transform) given phi (potentials)
    void transform_costs();

        //: Update node and arc costs (equivalent transform) given phi (potentials) for given node
    void transform_costs(unsigned inode);

    //: Find maximal nodes and arcs and check if arc consistent
    bool arc_consistent_solution(std::vector<unsigned >& x);

    //: Reset iteration counters
    void init();
    //: Calculate final sum of node and arc values
    double solution_cost(std::vector<unsigned>& x);

   public:
    //: Default constructor
    mmn_diffusion_solver();

    //: Construct with arcs
    mmn_diffusion_solver(unsigned num_nodes,const std::vector<mmn_arc>& arcs);

    //: Input the arcs that define the graph
    void set_arcs(unsigned num_nodes,const std::vector<mmn_arc>& arcs);

    //: Find values for each node with minimise the total cost
    //  \param node_cost: node_cost[i][j] is cost of selecting value j for node i
    //  \param pair_cost: pair_cost[a](i,j) is cost of selecting values (i,j) for nodes at end of arc a.
    //  \param x: On exit, x[i] gives choice for node i
    // NOTE: If arc a connects nodes v1,v2, the associated pair_cost is ordered
    // with the node with the lowest index being the first parameter.  Thus if
    // v1 has value i1, v2 has value i2, then the cost of this choice is
    // (v1<v2?pair_cost(i1,i2):pair_cost(i2,i1))
    //
    // Returns the minimum cost. Note that internally we deal with a maximisation
    // after negating the input costs, which are assumed to represent -log probabilities
    // In the return the boolean returns whether the algorithm was successful in converging
    // to an arc-consistent solution, and the double is the cost (negative minimum, i.e. -internal max)
    // Even if the solution is not arc-consistent a solution is still returned given by the local node
    // first maxima, but this may not then be optimal.
    std::pair<bool,double> operator()(const std::vector<vnl_vector<double> >& node_cost,
                                     const std::vector<vnl_matrix<double> >& arc_cost,
                                     std::vector<unsigned>& x);

    //: final iteration count
    unsigned count() const {return count_;}

    //: Produce shed loads of debug output
    void set_verbose(bool verbose) {verbose_=verbose;}
};

#endif // mmn_diffusion_solver_h_
