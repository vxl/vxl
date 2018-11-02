// This is mul/clsfy/clsfy_binary_tree.h
#ifndef clsfy_binary_tree_h_
#define clsfy_binary_tree_h_
//:
// \file
// \brief Binary tree classifier
// \author Martin Roberts
#include <iosfwd>
#include <iostream>
#include <utility>
#include <clsfy/clsfy_binary_threshold_1d.h>
#include <clsfy/clsfy_classifier_base.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


//: One node of a binary tree classifier - wrapper round clsfy_binary_threshold_1d
//  Needs also to store the data feature index associated with the node
//  Then it calls its binary classifier for that node
//  Returns class zero if s_*x[i]<threshold_

class clsfy_binary_tree_op
{
 protected:
    //Index within data of variable used at this node (set to -1 if none assigned)
    int data_index_;
    const vnl_vector<double>* data_ptr_;
    clsfy_binary_threshold_1d classifier_;

 public:

  clsfy_binary_tree_op() : data_index_(-1), data_ptr_(nullptr) {}
  clsfy_binary_tree_op(const vnl_vector<double>* data_ptr,
                       int data_index=-1)
    : data_index_(data_index), data_ptr_(data_ptr) {}

  clsfy_binary_threshold_1d& classifier() {return classifier_;}
  unsigned data_index() const {return data_index_;}
  void set_data_index(unsigned index) {data_index_=index;}
  void set_data_ptr(const vnl_vector<double>* data_ptr) {data_ptr_= data_ptr;}

  //: Return reference to data - NB throws std::bad_cast if null
  const vnl_vector<double >& data() const {return *data_ptr_;}

  void set_data(const vnl_vector<double >& inputs) {data_ptr_=&inputs;}
  //: Return value
  double val() const {return (*data_ptr_)[data_index_];}

  //: Classify
  unsigned classify() {return classifier_.classify(val());}

  unsigned ndims() {return data_ptr_ ? data_ptr_->size() : 0;}

  //: Save class to a binary File Stream
  void b_write(vsl_b_ostream& bfs) const;

  //: Load the class from a Binary File Stream
  void b_read(vsl_b_istream& bfs);

  short version_no() const {return 1;}
};


class clsfy_binary_tree_node
{
  int nodeId_;
  clsfy_binary_tree_node* parent_;
  clsfy_binary_tree_node* left_child_;
  clsfy_binary_tree_node* right_child_;
  clsfy_binary_tree_op op_;
  double prob_; //Only used on terminal nodes
 public:

  clsfy_binary_tree_node(clsfy_binary_tree_node* parent,
                         clsfy_binary_tree_op  op)
  : nodeId_(-1),parent_(parent),left_child_(nullptr),right_child_(nullptr),op_(std::move(op)),prob_(0.5) {}

  virtual clsfy_binary_tree_node* create_child(const clsfy_binary_tree_op& op);
  void add_child(const clsfy_binary_tree_op& op,bool bLeft)
  {
    clsfy_binary_tree_node* child=create_child(op);
    if (bLeft)
      left_child_=child;
    else
      right_child_=child;
  }

  //Note the owning classifier removes the tree - beware as once deleted its children
  //may be inaccessible for deletion
  virtual ~clsfy_binary_tree_node() = default;

  friend class clsfy_binary_tree;
  friend class clsfy_binary_tree_builder;
};


//: A binary tree classifier
// Drop down the tree using a binary threshold on a specific variable from the set at each node.
// Branch left for one classification, right for the other
// Eventually a node is reached with no children and that node's
// binary threshold classification is returned

class clsfy_binary_tree : public clsfy_classifier_base
{
 public:

  struct graph_rep
  {
      int me;
      int left_child;
      int right_child;
  };

  //: Constructor
  clsfy_binary_tree(): root_(nullptr),cache_node_(nullptr) {}

  ~clsfy_binary_tree() override;

  clsfy_binary_tree(const clsfy_binary_tree& srcTree);

  clsfy_binary_tree& operator=(const clsfy_binary_tree& srcTree);

  static void remove_tree(clsfy_binary_tree_node* root);
  //: Return the classification of the given probe vector.
  unsigned classify(const vnl_vector<double> &input) const override;

  //: Provides a probability-like value that the input being in each class.
  // output(i) i<nClasses, contains the probability that the input is in class i
  void class_probabilities(std::vector<double> &outputs, const vnl_vector<double> &input) const override;

  //: This value has properties of a Log likelihood of being in class (binary classifiers only)
  // class probability = exp(logL) / (1+exp(logL))
  double log_l(const vnl_vector<double> &input) const override;

  //: The number of possible output classes.
  unsigned n_classes() const override {return 1;}

  //: The dimensionality of input vectors.
  unsigned n_dims() const override;

  //: Storage version number
  virtual short version_no() const;

  //: Name of the class
  std::string is_a() const override;

  //: Name of the class
  bool is_class(std::string const& s) const override;

  //: Create a copy on the heap and return base class pointer
  clsfy_classifier_base* clone() const override;

  //: Print class to os
  void print_summary(std::ostream& os) const override;

  //: Save class to binary file stream
  void b_write(vsl_b_ostream& bfs) const override;

  //: Load class from binary file stream
  void b_read(vsl_b_istream& bfs) override;

  //: Normally only the builder uses this
  void set_root(  clsfy_binary_tree_node* root);
 private:
  clsfy_binary_tree_node* root_;
  mutable clsfy_binary_tree_node* cache_node_;
 private:
  void copy(const clsfy_binary_tree& srcTree);
  void copy_children(clsfy_binary_tree_node* pSrcNode,clsfy_binary_tree_node* pNode);
};

#endif // clsfy_binary_tree_h_
