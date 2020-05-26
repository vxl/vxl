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


class acal_match_node : public std::enable_shared_from_this<acal_match_node>
{
 public:

  //: constructor
  acal_match_node(size_t node_id = 0) : cam_id_(node_id) {}

  //: property accessors
  size_t size() const { return children_.size(); }
  bool is_leaf() const { return children_.empty(); }
  bool is_root() const { return !has_parent_; }
  bool has_parent() const { return has_parent_; }

  //: parent accessors
  std::shared_ptr<acal_match_node> parent() const { return parent_.lock(); }
  void parent(std::shared_ptr<acal_match_node> node) {
    has_parent_ = true;
    parent_ = node;
  }

  //: create & add a child node to "this" node
  void add_child(size_t child_id, std::vector<acal_match_pair> const& self_to_child_matches) {
    auto child = std::make_shared<acal_match_node>(child_id);
    child->parent(shared_from_this());
    children_.push_back(child);
    self_to_child_matches_.push_back(self_to_child_matches);
  }

  //: parent & children node id
  size_t parent_id() const;
  std::vector<size_t> children_ids() const;

  //: equality operators
  bool operator==(acal_match_node const& other) const;
  bool operator!=(acal_match_node const& other) const { return !(*this == other); }

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

  // public members
  size_t cam_id_ = 0;
  std::vector<std::shared_ptr<acal_match_node> > children_;
  std::vector<std::vector<acal_match_pair> > self_to_child_matches_;

 private:

  // private members
  bool has_parent_ = false;
  std::weak_ptr<acal_match_node> parent_;

};

// streaming operator
std::ostream& operator<<(std::ostream& os, acal_match_node const& node);



class acal_match_tree
{
 public:

  //: constructor
  acal_match_tree(size_t root_id = 0) {
    root_->cam_id_ = root_id;
  }

  //: size accessor
  size_t size() {return n_;}

  void update_tree_size() {
    n_ = 0;
    n_nodes(root_, n_);
  }

  //: add a child node and reconcile the correspondence pairs globally over the full tree
  bool add_child_node(size_t parent_id, size_t child_id, std::vector<acal_match_pair> const& parent_to_child_matches);

  //: prune correspondence pairs to insure consistent tracks over the full tree
  bool prune_tree(std::shared_ptr<acal_match_node> const& mutated_parent_node, size_t child_index);

  //: recursively remove correspondence pairs that are not present in reduced node corrs
  void prune_node(std::shared_ptr<acal_match_node> const& node, std::vector<acal_corr> const& reduced_node_corrs);

  //: recursively find a node camera id and return the node pointer
  static std::shared_ptr<acal_match_node> find(std::shared_ptr<acal_match_node> const& node, size_t cam_id);

  //: recursively find the number of nodes below a node in the tree
  void n_nodes(std::shared_ptr<acal_match_node> const& node, size_t& n);

  // debug methods
  //: save the tree in dot format for display
  bool save_tree_dot_format(std::string const& path);

  //: recursively write the tree nodes and edges in dot format
  bool write_dot(std::ostream& ostr, std::shared_ptr<acal_match_node> const& node, size_t root_id);

  //: collect correspondences recursively
  void collect_correspondences(std::shared_ptr<acal_match_node>& node, std::map<size_t, std::vector<vgl_point_2d<double> > >& corrs);

  //: reorganize correspondences in track format
  std::vector< std::map<size_t, vgl_point_2d<double> > > tracks();

  //: return nodes
  std::vector<std::shared_ptr<acal_match_node> > nodes() const;

  //: return sorted cam ids
  std::vector<size_t> cam_ids() const;

  //: print tree
  void print(std::ostream& os) const;

  //: equality operators
  bool operator==(acal_match_tree const& other) const;
  bool operator!=(acal_match_tree const& other) const { return !(*this == other); }

  // members
  size_t n_ = 1;
  size_t min_n_tracks_ = 1;
  std::shared_ptr<acal_match_node> root_ = std::make_shared<acal_match_node>();

 private:

  // recursively process tree info
  void nodes_recursive(std::vector<std::shared_ptr<acal_match_node> >& nodes, std::shared_ptr<acal_match_node> node) const;
  void cam_ids_recursive(std::vector<size_t>& ids, std::shared_ptr<acal_match_node> node) const;
  void print_recursive(std::ostream& os, std::shared_ptr<acal_match_node> node, std::string indent = "  ") const;

};

// streaming operator
std::ostream& operator<<(std::ostream& os, acal_match_tree const& tree);


#endif
