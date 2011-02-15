// This is mul/clsfy/clsfy_random_forest_builder.h
#ifndef clsfy_random_forest_builder_h_
#define clsfy_random_forest_builder_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Build a random forest classifier
// \author Martin Roberts

#include <clsfy/clsfy_builder_base.h>
#include <clsfy/clsfy_random_forest.h>
#include <vcl_vector.h>
#include <vcl_set.h>
#include <vcl_string.h>
#include <vcl_iosfwd.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_random.h>

#include <mbl/mbl_data_wrapper.h>


//: Builds clsfy_random_forest classifiers
class clsfy_random_forest_builder : public clsfy_builder_base
{
 public:
  // Dflt ctor
  clsfy_random_forest_builder();

  clsfy_random_forest_builder(unsigned ntrees,
                              int max_depth=-1,int min_node_size=-1);
  virtual ~clsfy_random_forest_builder();

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


  //: set number of trees in forest
  // Note this must be set before calling build
  // Default is 100
  void set_ntrees(unsigned ntrees) {ntrees_=ntrees;}

  unsigned ntrees() const {return ntrees_;}

  virtual void seed_sampler(unsigned long seed);

  //: set whether the build calculates a test error over the input training set
  // Default is on, but this can be turned off
  // e.g. for a parallel build of many partial random forests of
  // which can be later merged
  void set_calc_test_error(bool on) {calc_test_error_=on;}

  //: Save a pointer to storage for out of bag indices
  void set_oob_indices( vcl_vector<vcl_vector<unsigned > >* poobIndices)
  {poob_indices_=poobIndices;}

 protected:
  //: Pick the number of parameters that the tree builder branches on
  // Default uses sqrt of ndims
  virtual unsigned select_nbranch_params(unsigned ndims) const;

  //: Pick a random data subset (with replacement)
  virtual void select_data(vcl_vector<vnl_vector<double> >& inputs,
                           const vcl_vector<unsigned> &outputs,
                           vcl_vector<vnl_vector<double> >& bootstrapped_inputs,
                           vcl_vector<unsigned> & bootstrapped_outputs) const;

  virtual unsigned long get_tree_builder_seed() const;

  //: Number of trees
  unsigned ntrees_;
  //: The max depth of any child tree
  //If negative no max is applied, and all final leaf nodes are pure
  //(i.e. single class)
  int max_depth_;


  //: Minimum number of points associated with any node
  // If negative this is ignored, otherwise if a split would produce a child
  // node less than this, then the split does not occur and the branch is
  // terminated
  int min_node_size_;

  //: Uniform sampler on 0,1 (for bootstrapping)
  mutable  vnl_random random_sampler_;

  //: Pointer to storage of point indices for each bootstrapped tree
  // Can be used for out of bag estimates
  // Saves for tree i the indices of all points used in its training
  // Note the storage is supplied from outside this class, as this is a kind of bolt-on
  vcl_vector<vcl_vector<unsigned > >* poob_indices_;
 private:
  //: Does the builder calculate the error on the training set?
  bool calc_test_error_;
};


#endif // clsfy_random_forest_builder_h_
