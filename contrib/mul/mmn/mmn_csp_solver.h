#ifndef mmn_csp_solver_h_
#define mmn_csp_solver_h_
//:
// \file
// \brief See if the Constraint Satisfaction Problem is satisfiable
// \author Martin Roberts

#include <vector>
#include <iostream>
#include <set>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <mmn/mmn_arc.h>
#include <mmn/mmn_dependancy.h>
#include <mmn/mmn_graph_rep1.h>
#include <mbl/mbl_stl_pred.h>

//: Given (sparse?) graph eliminate invalid arcs and nodes until a kernel of arc-consistent nodes and arcs remains
//  See Werner 2007 paper in IEEE Trans on Pattern Recognition and Machine Intelligence
//  Can be used to see if Max Sum problem has been reduced to "trivial" form
//  i.e. if the maximal nodes are joined up via maximal arcs
//  We successively delete
//  a) nodes not linked with some neighbour by any edge
//  b) edges lacking an end node

class mmn_csp_solver
{
 public:
    //: Subset of labels present for each node
    typedef std::set<unsigned>  label_subset_t;

    //: Define the subset of labels linked
    // For each original arc (outer vector), the inner set gives all the
    // corresponding node labels actually linked
    // Note the first in the pair corresponds always to the lower node ID in the arc (i.e. as for arc pair costs
    typedef std::set<std::pair<unsigned ,unsigned >, mbl_stl_pred_pair_order<unsigned ,unsigned > >  arc_labels_subset_t;
    //Similar but multiset with partial ordering by first label
    typedef std::multiset<std::pair<unsigned ,unsigned >,
        mbl_stl_pred_pair_key_order<std::pair<unsigned ,unsigned > > >  arc_labels_subset_t1;
    //Similar but multiset with partial ordering by second label
    typedef std::multiset<std::pair<unsigned ,unsigned >,
        mbl_stl_pred_pair_value_order<std::pair<unsigned ,unsigned > > >  arc_labels_subset_t2;

 private:
    unsigned nnodes_;

    bool verbose_;
    //:Vector of nodes, defining which labels are present for each node
    //Note some sets may become empty
    std::vector<label_subset_t > node_labels_present_;

    //: Define the subset of labels linked
    // For each original arc (outer vector), the inner set gives all the
    // corresponding node labels actually linked
    // Note the first in the pair corresponds always to the lower node ID in the arc (i.e. as for arc pair costs
    std::vector<arc_labels_subset_t1 > arc_labels_linked1_;
    std::vector<arc_labels_subset_t2 > arc_labels_linked2_;
    //:Store in graph form (so each node's neighbours are conveniently to hand)
    mmn_graph_rep1 graph_;

    //: The arcs from which graph was generated
    std::vector<mmn_arc> arcs_;

    //: delete any node labels not linked by any current arcs
    //Return true if any deletions occur
    bool check_for_node_deletions();

    //: delete any arcs with either target node label not present
    //Return true if any deletions occur
    bool check_for_arc_deletions();

    void initialise_arc_labels_linked(const std::vector<mmn_csp_solver:: arc_labels_subset_t >& links_subset);

    void init();
 public:
    //: Default constructor
    mmn_csp_solver();

    //: Construct with arcs
    mmn_csp_solver(unsigned num_nodes,const std::vector<mmn_arc>& arcs);

    //: Input the arcs that define the graph
    void set_arcs(unsigned num_nodes,const std::vector<mmn_arc>& arcs);

    bool operator()(const std::vector<mmn_csp_solver::label_subset_t >& node_labels_subset,
                    const std::vector<mmn_csp_solver::arc_labels_subset_t >& links_subset);

    void set_verbose(bool verbose) {verbose_=verbose;}
    const std::vector<label_subset_t >& kernel_node_labels() const {return node_labels_present_;}
};

#endif // mmn_csp_solver_h_
