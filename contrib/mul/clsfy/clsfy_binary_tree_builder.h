// This is mul/clsfy/clsfy_binary_tree_builder.h
#ifndef clsfy_binary_tree_builder_h_
#define clsfy_binary_tree_builder_h_
//:
// \file
// \brief Build a binary tree classifier
// \author Martin Roberts

#include <vector>
#include <set>
#include <string>
#include <iostream>
#include <iosfwd>
#include <clsfy/clsfy_builder_base.h>
#include <clsfy/clsfy_binary_tree.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <mbl/mbl_data_wrapper.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_random.h>


class clsfy_binary_tree_bnode :public  clsfy_binary_tree_node
{
    //Similar to classifiers tree node but the builder also needs
    //to keep track of relevant data subsets at each node
    std::set<unsigned> subIndicesL;
    std::set<unsigned> subIndicesR;


  clsfy_binary_tree_bnode(clsfy_binary_tree_node* parent,
                          const clsfy_binary_tree_op& op):
    clsfy_binary_tree_node(parent,op) {}

    clsfy_binary_tree_node* create_child(const clsfy_binary_tree_op& op) override;

    //Note the owning classifier removes the tree - beware as once deleted its children
    //may be inaccessible for deletion
    ~clsfy_binary_tree_bnode() override;

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
    mutable std::vector<unsigned > base_indices_;

  public:
    // Dflt ctor
    clsfy_binary_tree_builder();

    //: Create empty model
    // Caller is responsible for deletion
    clsfy_classifier_base* new_classifier() const override;

    //: Build classifier from data
    // return the mean error over the training set.
    double build(clsfy_classifier_base& classifier,
                         mbl_data_wrapper<vnl_vector<double> >& inputs,
                         unsigned nClasses,
                         const std::vector<unsigned> &outputs) const override;

    //: Name of the class
    std::string is_a() const override;

    //: Name of the class
    bool is_class(std::string const& s) const override;

    //: IO Version number
    short version_no() const;

    //: Create a copy on the heap and return base class pointer
    clsfy_builder_base* clone() const override;

    //: Print class to os
    void print_summary(std::ostream& os) const override;

    //: Save class to binary file stream
    void b_write(vsl_b_ostream& bfs) const override;

    //: Load class from binary file stream
    void b_read(vsl_b_istream& bfs) override;

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
                                      std::vector<unsigned  >& param_indices) const;

    mutable  vnl_random random_sampler_;


  private:
    void build_children(
        const std::vector<vnl_vector<double> >& vin,
        const std::vector<unsigned>& outputs,
        clsfy_binary_tree_bnode* parent, bool to_left) const;

    void copy_children(clsfy_binary_tree_bnode* pBuilderNode,clsfy_binary_tree_node* pNode) const;

    void set_node_prob(clsfy_binary_tree_node* pNode,
                       clsfy_binary_tree_bnode* pBuilderNode) const ;

    void build_a_node(
        const std::vector<vnl_vector<double> >& vin,
        const std::vector<unsigned>& outputs,
        const std::set<unsigned >& subIndices,
        clsfy_binary_tree_bnode* pNode) const;

    bool isNodePure(const std::set<unsigned >& subIndices,
                    const std::vector<unsigned>& outputs) const;

    void add_terminator(
        const std::vector<vnl_vector<double> >& vin,
        const std::vector<unsigned>& outputs,
        clsfy_binary_tree_bnode* parent,
        bool to_left, bool pure) const;

    bool calc_test_error_;
};


#endif // clsfy_binary_tree_builder_h_
