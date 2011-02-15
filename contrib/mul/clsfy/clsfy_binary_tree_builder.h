// This is mul/clsfy/clsfy_binary_tree_builder.h
#ifndef clsfy_binary_tree_builder_h_
#define clsfy_binary_tree_builder_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Build a binary tree classifier
// \author Martin Roberts

#include <clsfy/clsfy_builder_base.h>
#include <clsfy/clsfy_binary_tree.h>
#include <vcl_vector.h>
#include <vcl_set.h>
#include <vcl_string.h>
#include <vcl_iosfwd.h>
#include <mbl/mbl_data_wrapper.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_random.h>


class clsfy_binary_tree_bnode :public  clsfy_binary_tree_node
{
    //Similar to classifiers tree node but the builder also needs
    //to keep track of relevant data subsets at each node
    vcl_set<unsigned> subIndicesL;
    vcl_set<unsigned> subIndicesR;


  clsfy_binary_tree_bnode(clsfy_binary_tree_node* parent,
                          const clsfy_binary_tree_op& op):
    clsfy_binary_tree_node(parent,op) {}

    virtual clsfy_binary_tree_node* create_child(const clsfy_binary_tree_op& op);

    //Note the owning classifier removes the tree - beware as once deleted its children
    //may be inaccessible for deletion
    virtual ~clsfy_binary_tree_bnode();

    friend class clsfy_binary_tree_builder;
};


//: Builds clsfy_binary_tree classifiers
// Keep finding the variable split that gives the least min_error for
// a binary threshold. Divide up the dataset by that and keep recursively
// building binary threshold classifiers in a tree structure till either
// Max depth level reached, or a node is pure, or node's data <min_nide_size

class clsfy_binary_tree_builder : public clsfy_builder_base
{
    //: The max depth of any leaf node in the tree
    //If negative no max is applied, and all final leaf nodes are pure
    //(i.e. single class)
    int max_depth_;

    //: Minimum number of points associated with any node
    // If negative this is ignored, otherwise if a split would produce a child
    // node less than this, then the split does not occur and the branch is
    // terminated
    int min_node_size_;

    //: Set this for random forest behaviour
    //At each split the selection is only from a random subset of this size
    //If negative (default) it is ignored and all are used
    int nbranch_params_;

    //: Work space for randomising params (NB not thread safe)
    mutable vcl_vector<unsigned > base_indices_;

  public:
    // Dflt ctor
    clsfy_binary_tree_builder();

    //: Create empty model
    // Caller is responsible for deletion
    virtual clsfy_classifier_base* new_classifier() const;

    //: Build classifier from data
    // return the mean error over the training set.
    virtual double build(clsfy_classifier_base& classifier,
                         mbl_data_wrapper<vnl_vector<double> >& inputs,
                         unsigned nClasses,
                         const vcl_vector<unsigned> &outputs) const;

    //: Name of the class
    virtual vcl_string is_a() const;

    //: Name of the class
    virtual bool is_class(vcl_string const& s) const;

    //: IO Version number
    short version_no() const;

    //: Create a copy on the heap and return base class pointer
    virtual clsfy_builder_base* clone() const;

    //: Print class to os
    virtual void print_summary(vcl_ostream& os) const;

    //: Save class to binary file stream
    virtual void b_write(vsl_b_ostream& bfs) const;

    //: Load class from binary file stream
    virtual void b_read(vsl_b_istream& bfs);

    //: The max tree depth (default -1 means no max set )
    int max_depth() const {return max_depth_;}

    //: Set the number of nearest neighbours to look for.
    // If not see default is high value to force continuation till
    // all final leaf nodes are pure (i.e. single class)
    // If set negative the value is ignored
    void set_max_depth(int max_depth) {max_depth_=max_depth;}

    int min_node_size() const {return min_node_size_;}

    //: Set minimum number of points associated with any node
    // If negative this is ignored, otherwise if a split would produce a child
    // node less than this, then the split does not occur and the branch is
    // terminated
    void set_min_node_size(int min_node_size) {min_node_size_=min_node_size;}

    //: Set this for random forest behaviour
    // At each split the selection is only from a random subset of this size
    // If negative then it is ignored
    void set_nbranch_params(int nbranch_params) {nbranch_params_ = nbranch_params;}

    //: set whether the build calculates a test error over the input training set
    // Default is on, but this can be turned off e.g. for a random forest of
    // many child trees
    void set_calc_test_error(bool on) {calc_test_error_=on;}

    //: Seed the sample used to select branching parameter subsets
    void seed_sampler(unsigned long seed);
  protected:
    //: Randomly select  the ndimsUsed dimensions for current branch
    // Return indices of selected parameters
    // Best of these is then chosen as the branch
    virtual void randomise_parameters(unsigned ndimsUsed,
                                      vcl_vector<unsigned  >& param_indices) const;

    mutable  vnl_random random_sampler_;


  private:
    void build_children(
        const vcl_vector<vnl_vector<double> >& vin,
        const vcl_vector<unsigned>& outputs,
        clsfy_binary_tree_bnode* parent, bool to_left) const;

    void copy_children(clsfy_binary_tree_bnode* pBuilderNode,clsfy_binary_tree_node* pNode) const;

    void set_node_prob(clsfy_binary_tree_node* pNode,
                       clsfy_binary_tree_bnode* pBuilderNode) const ;

    void build_a_node(
        const vcl_vector<vnl_vector<double> >& vin,
        const vcl_vector<unsigned>& outputs,
        const vcl_set<unsigned >& subIndices,
        clsfy_binary_tree_bnode* pNode) const;

    bool isNodePure(const vcl_set<unsigned >& subIndices,
                    const vcl_vector<unsigned>& outputs) const;

    void add_terminator(
        const vcl_vector<vnl_vector<double> >& vin,
        const vcl_vector<unsigned>& outputs,
        clsfy_binary_tree_bnode* parent,
        bool to_left, bool pure) const;

    bool calc_test_error_;
};


#endif // clsfy_binary_tree_builder_h_
