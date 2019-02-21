// This is//external/acal/acal_match_tree.h
#ifndef acal_match_tree_h
#define acal_match_tree_h

//:
// \file
// \brief A tree of match tracks with nodes as cameras. Tree edges are correspondence pairs
// \author J.L. Mundy
// \date January 14, 2019
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <math.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>
#include "acal_match_utils.h"


class acal_match_node
{
 public:
  acal_match_node():cam_id_(-1), parent_(0), node_depth_(0){}
  acal_match_node(size_t node_id): cam_id_(node_id), parent_(0),node_depth_(0){}
  acal_match_node(size_t node_id, size_t child_id, std::vector<acal_match_pair> const& self_to_child_matches):
  cam_id_(node_id), node_depth_(0), parent_(0) {
    this->add_child(child_id, self_to_child_matches);
  }
  ~acal_match_node(){
    parent_ = 0;
  }
  size_t size(){return children_.size();}
  bool is_leaf(){return size() == 0;}
  bool is_root(){return parent_==0;}

  // this parent accessor is designed to avoid wraping the
  // bare parent pointer in a shared_ptr. Otherwise, the
  // parent will be deleted multiple times causing a seg fault
  // the argument "root" is needed for a protected shared pointer to the root node,
  // since the root has no parent, needed by the approach
  std::shared_ptr<acal_match_node> parent(std::shared_ptr<acal_match_node> const& root){
    if(parent_ == 0)
      return std::shared_ptr<acal_match_node>();
    acal_match_node* grand_parent = parent_->parent_;
    if(grand_parent == 0)
      return root;
    size_t nc = grand_parent->size();
    for(size_t i = 0; i<nc; ++i)
      if(parent_ == grand_parent->children_[i].get())
        return grand_parent->children_[i];
    return std::shared_ptr<acal_match_node>();
  }

  //: add a child node to "this" node
  void add_child(size_t child_id, std::vector<acal_match_pair> const& self_to_child_matches){
    std::shared_ptr<acal_match_node> child(new acal_match_node(child_id));
    child->parent_= this;
    children_.push_back(child);
    self_to_child_matches_.push_back(self_to_child_matches);
  }

  //: find the index (cindx) of a node in the set of parent's children
  bool child_index(std::shared_ptr<acal_match_node> const& node, size_t& cidx) {
    size_t nc = children_.size();
    bool found = false;
    for (size_t i = 0; i < nc && !found; ++i)
      if (children_[i]->cam_id_ == node->cam_id_) {
        cidx = i;
        found = true;
      }
    return found;
  }

  // members
  size_t cam_id_;
  size_t node_depth_;
  acal_match_node* parent_;
  std::vector<std::shared_ptr<acal_match_node> > children_;
  std::vector<std::vector<acal_match_pair> > self_to_child_matches_;
};


class acal_match_tree
{
 public:

  acal_match_tree():n_(0), min_n_tracks_(1){}
  ~acal_match_tree();
  size_t size(){return n_;}
  void update_tree_size(){
    n_ = 0;
    n_nodes(root_, n_);
  }

  //: initialize the tree
  void create_root(size_t node_id, size_t child_id, std::vector<acal_match_pair> const& node_to_child_matches){
    root_ = std::shared_ptr<acal_match_node>(new acal_match_node(node_id, child_id, node_to_child_matches));
  }

  //: add a child node and reconcile the correpondence pairs globally over the full tree
  bool add_child_node(size_t parent_id, size_t child_id, std::vector<acal_match_pair> const& parent_to_child_matches);

  //: prune correpondence pairs to insure consistent tracks over the full tree
  bool prune_tree(std::shared_ptr<acal_match_node> const& mutated_parent_node, size_t child_index);

  //: recursively remove correspondence pairs that are not present in reduced node corrs
  void prune_node(std::shared_ptr<acal_match_node> const& node, std::vector<acal_corr> const& reduced_node_corrs);

  //: recursively find a node camera id and return the node pointer
  static std::shared_ptr<acal_match_node> find(std::shared_ptr<acal_match_node> const& node, size_t cam_id);

  //: recursively find the number of nodes below a node in the tree
  void  n_nodes(std::shared_ptr<acal_match_node> const& node, size_t& n);

  // debug methods
  //: save the tree in dot format for display
  bool save_tree_dot_format(std::string const& path);

  //: recursively write the tree nodes and edges in dot format
  bool write_dot(std::ostream& ostr, std::shared_ptr<acal_match_node> const& node, size_t root_id);

  //:: assign depth recursively
  void assign_depth(std::shared_ptr<acal_match_node> const& node, size_t& depth);

  //: collect nodes recursively
  void collect_nodes(std::shared_ptr<acal_match_node>& node, std::vector<acal_match_node*>& nodes);

  //: collect correspondences recursively
  void collect_correspondences(std::shared_ptr<acal_match_node>& node, std::map<size_t, std::vector<vgl_point_2d<double> > >& corrs);

  //: reorganize correspondences in track format
  std::vector< std::map<size_t, vgl_point_2d<double> > > tracks();

  //: sort nodes by tree depth
  std::vector<acal_match_node* > depth_sorted_nodes();

  //: return all cam ids
  std::vector<size_t> cam_ids();

  // members
  size_t n_;
  size_t min_n_tracks_;
  std::shared_ptr<acal_match_node> root_;
};

#endif
