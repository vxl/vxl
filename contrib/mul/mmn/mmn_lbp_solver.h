#ifndef mmn_lbp_solver_h_
#define mmn_lbp_solver_h_
//:
// \file
// \brief Run loopy belief propagation over the graph
// \author Martin Roberts

#include <vcl_vector.h>
#include <vcl_map.h>
#include <vcl_deque.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <mmn/mmn_arc.h>
#include <mmn/mmn_graph_rep1.h>
#include <mmn/mmn_solver.h>
#include <vcl_iosfwd.h>

//: Run loopy belief to estimate overall marginal probabilities of all node states
//  Then use converged LBP messages to also estimate overall most likely configuration
//
// Can use this for non-tree graphs, but convergence to optimum is not absolutely guaranteed
// Should converge if there is at most one loop in the graph
// The input graph is converted to form mmn_graph_rep1 from the input arcs

class mmn_lbp_solver: public mmn_solver
{
 public:
    //: Message update mode type (all in parallel, or randomised node order with immediate effect}
    enum msg_update_t {eALL_PARALLEL,eRANDOM_SERIAL};
 private:
    //: in below the map is indexed by the neighbour's node id

    //: Inner vector indexed by target node state ID
    typedef vcl_map<unsigned,vnl_vector<double > > message_set_t;

    //: Matrix referenced by [source node state ID][target node state ID]
    // Map ID is target node ID
    typedef vcl_map<unsigned, vnl_matrix<double > > neigh_arc_cost_t;

    //:Store in graph form (so each node's neighbours are conveniently to hand)
    mmn_graph_rep1 graph_;

    //: The arcs from which the graph was generated
    vcl_vector<mmn_arc> arcs_;

    //: Total number of nodes
    unsigned nnodes_;

    //: Workspace for costs of each arc
    vcl_vector<neigh_arc_cost_t > arc_costs_;

    //: All the messages at previous iteration (vector index is source node)
    vcl_vector<message_set_t > messages_;
    //: Update messages calculated during this iteration (vector index is source node)
    vcl_vector<message_set_t > messages_upd_;

    //: Node costs (outer vector is node ID, inner vnl_vector is by state value)
    vcl_vector<vnl_vector<double> > node_costs_;

    //: belief prob for each state of each node
    // Assumes input node costs are well-normalised for these to be proper probabilities
    vcl_vector<vnl_vector<double> > belief_;

    //: previous N solutions (used to trap cycling)
    vcl_deque<vcl_vector<unsigned  > > soln_history_;

    //: previous max_delta values(used to check still descending)
    vcl_deque<double  > max_delta_history_;

    //: Current iteration count
    unsigned count_;

    //: Max change in any message value over this iteration
    double max_delta_;

    //: max number of iterations allowed
    unsigned max_iterations_;

    //: min number of iterations before checking for solution looping (cycling)
    unsigned min_simple_iterations_;

    //: Convergence criterion on max_delta_
    double epsilon_;

    //: count of number of times a solution in history is revisited
    unsigned nrevisits_;

    //: cycle condition detected
    bool isCycling_;

    //: Number of times cycling has been detected
    unsigned cycle_detection_count_;

    //: message update smoothing constant (used if cycling detected)
    double alpha_;

    //: should message update be smoothed during cycling
    bool smooth_on_cycling_;

    //; Maximum number of allowed cycle detections
    //NOTE only used if smooth_on_cycling_ is true
    //Otherwise we give up after the first cycle is detected
    unsigned max_cycle_detection_count_;

    //: solution value when cycling first detected
    double zbest_on_cycle_detection_;

    //:verbose debug output
    bool verbose_;

    msg_update_t msg_upd_mode_;

    //: Magic numbers for cycle detection
    static const unsigned NHISTORY_;
    static const unsigned NCYCLE_DETECT_;

    //: Check if we carry on
    bool continue_propagation(vcl_vector<unsigned>& x);

    //: Update all messages from input node to its neighbours
    void update_messages_to_neighbours(unsigned inode,
                                       const vnl_vector<double>& node_cost);

    //: Renormalise messages (assume they represent log probabilities) so SUM(exp) over target states is 1
    void renormalise_log(vnl_vector<double >& logMessageVec);

    //: Reset iteration counters
    void init();
    //: Calculate final sum of node and arc values
    double solution_cost(vcl_vector<unsigned>& x);

    double best_solution_cost_in_history(vcl_vector<unsigned>& x);

    //: update beliefs and calculate changes therein
    void calculate_beliefs(vcl_vector<unsigned>& x);
 public:
    //: Default constructor
    mmn_lbp_solver();

    //: Construct with arcs
    mmn_lbp_solver(unsigned num_nodes,const vcl_vector<mmn_arc>& arcs);

    //: Input the arcs that define the graph
    virtual void set_arcs(unsigned num_nodes,const vcl_vector<mmn_arc>& arcs);

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
    // If the states of a node in node_cost are not well normalised as a probability
    // the algorithm should still work in some sense
    // but the meaning of the belief_ objects is not really then well-defined.
    // As it is marginal "belief" that is maximised, inputting non-normalised data may not give quite the
    // expected answer - there may be some biases, in effect implicit weightings to particular nodes
    double operator()(const vcl_vector<vnl_vector<double> >& node_cost,
                      const vcl_vector<vnl_matrix<double> >& arc_cost,
                      vcl_vector<unsigned>& x);

    //: return the beliefs, i.e. the marginal probabilities of each node's states
    //
    virtual double solve(
                 const vcl_vector<vnl_vector<double> >& node_cost,
                 const vcl_vector<vnl_matrix<double> >& pair_cost,
                 vcl_vector<unsigned>& x);

    const vcl_vector<vnl_vector<double>  >&  belief() const {return belief_;}

    //: final iteration count
    unsigned count() const {return count_;}

    //: Set true if want to alpha smooth message updates when cycling detected
    // This may break the cycling condition
    void set_smooth_on_cycling(bool bOn) {smooth_on_cycling_=bOn;}

    void set_max_cycle_detection_count_(unsigned max_cycle_detection_count) {max_cycle_detection_count_=max_cycle_detection_count;}

    void set_verbose(bool verbose) {verbose_=verbose;}

    //: Set message update mode (parallel or randomised serial}
    void set_msg_upd_mode(msg_update_t msg_upd_mode) {msg_upd_mode_ = msg_upd_mode;}

    //: Initialise from a text stream
    virtual bool set_from_stream(vcl_istream &is);

    //: Version number for I/O
    short version_no() const;

    //: Name of the class
    virtual vcl_string is_a() const;

    //: Create a copy on the heap and return base class pointer
    virtual mmn_solver* clone() const;

    //: Print class to os
    virtual void print_summary(vcl_ostream& os) const;

    //: Save class to binary file stream
    virtual void b_write(vsl_b_ostream& bfs) const;

    //: Load class from binary file stream
    virtual void b_read(vsl_b_istream& bfs);
};

#endif // mmn_lbp_solver_h_
