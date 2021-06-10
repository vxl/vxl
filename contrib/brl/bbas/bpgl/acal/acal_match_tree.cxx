#include <sstream>
#include <algorithm>

#include "acal_match_tree.h"


// --------------------
// acal_match_node
// --------------------

// parent node id
size_t
acal_match_node::parent_id() const
{
  if (this->has_parent_) {
    return this->parent()->cam_id_;
  } else {
    return 0;
  }
}


// children node ids
std::vector<size_t>
acal_match_node::children_ids() const
{
  std::vector<size_t> ids;
  for (auto child : this->children_) {
    ids.push_back(child->cam_id_);
  }
  return ids;
}


//: node equality operator
bool
acal_match_node::operator==(acal_match_node const& other) const
{
  return this->cam_id_ == other.cam_id_ &&
         this->is_root() == other.is_root() &&
         (this->is_root() || (this->parent_id() == other.parent_id())) &&
         this->children_ids() == other.children_ids() &&
         this->self_to_child_matches_ == other.self_to_child_matches_;
}


//: node output stream
std::ostream&
operator<<(std::ostream& os, acal_match_node const& node)
{
  // id
  os << "node " << node.cam_id_ << ": ";

  // parent info
  if (node.is_root()) {
    os << "root, ";
  } else {
    os << "parent " << node.parent_id() << ", ";
  }

  // children info
  os << "children: [";
  std::string separator;
  for (auto child : node.children_) {
    os << separator << child->cam_id_;
    separator = ",";
  }
  os << "]";

  // return stream
  return os;
}


// --------------------
// acal_match_tree
// --------------------

// make a deep copy of a subtree
void
acal_match_tree::copy_subtree(std::shared_ptr<acal_match_node> const& node_from,
                              std::shared_ptr<acal_match_node>& node_to)
{
  // don't copy the "from" parent pointer since copied subtree context is not specified
  // just set the camera id
  node_to->cam_id_ = node_from->cam_id_;

  size_t nc = node_from->children_.size();
  if(nc == 0) return; // nothing more to do node is a leaf

  // copy the data for the children
  for (size_t c = 0; c<nc ; ++c)
    node_to->add_child(node_from->children_[c]->cam_id_, node_from->self_to_child_matches_[c]);

  //recursively copy the child subtrees
  for(size_t c = 0; c<nc; ++c)
    copy_subtree(node_from->children_[c], node_to->children_[c]);

}


// copy constructor
acal_match_tree::acal_match_tree(acal_match_tree const& mt){
  copy_subtree(mt.root_, this->root_);
  this->update_tree_size();
}


// delete a leaf node of the tree
bool
acal_match_tree::delete_leaf(std::shared_ptr<acal_match_node>& leaf_node)
{
  if(leaf_node->children_.size() != 0)//not a leaf node
    return false;

  // isolated root
  if (!leaf_node->has_parent()){
    leaf_node.reset();//delete the isolated root
    return true;
  }
  //prepare to do surgery on the parent to remove self
  // parent's children
  std::vector<std::shared_ptr<acal_match_node> >& parent_children = leaf_node->parent()->children_;
  // parent's matches
  std::vector<std::vector<acal_match_pair> >& parent_matches = leaf_node->parent()->self_to_child_matches_;

  size_t nc = parent_children.size();
  if(nc == 0){// can't happen so just do nothing and return false;
    std::cout << "Parent of node " << leaf_node->cam_id_ << " has no children - shouldn't happen" << std::endl;
    return false;
  }
  // find self in parent's children
  for(size_t c = 0; c<nc; ++c)
    if(parent_children[c]->cam_id_ == leaf_node->cam_id_){
      //remove self from parent's children
      parent_children.erase(parent_children.begin() + c);
      parent_matches.erase(parent_matches.begin() + c);
      leaf_node.reset();//delete self
      return true;
    }
  //couldn't find leaf in parent children
  std::cout << "Couldn't find leaf node" << leaf_node->cam_id_ << " in its parent's  children - shouldn't happen" << std::endl;
  return false;
}


std::shared_ptr<acal_match_node>
acal_match_tree::find_leaf_node(std::shared_ptr<acal_match_node>& node)
{
  size_t nc = node->children_.size();
  if(nc == 0) // node is a leaf
    return node;

  for(size_t c = 0; c<nc; ++c)
    return find_leaf_node(node->children_[c]);

  std::cout << "Can't find a leaf node including and below node " << node->cam_id_ << std::endl;
  return std::shared_ptr<acal_match_node>();
}


// delete a subtree with node as root including self
bool
acal_match_tree::delete_subtree(std::shared_ptr<acal_match_node>& subtree_root)
{
  // ::find_leaf_node operates depth first so the entire subtree will
  // eventually be deleted as leaf nodes are removed
  while(subtree_root->children_.size() > 0){
    std::shared_ptr<acal_match_node> leaf = find_leaf_node(subtree_root);
    if(!leaf){//shouldn't happen
      std::cout << "Can't find leaf node " << leaf->cam_id_ << " in subtree with root " << subtree_root->cam_id_ << " - fatal" << std::endl;
      return false;
    }
    if(!delete_leaf(leaf)){//shouldn't happen
      std::cout << "Can't delete leaf node " << leaf->cam_id_ << " in subtree with root " << subtree_root->cam_id_ << " - fatal" << std::endl;
      return false;
    }
  }
  // every node below the subtree root is now deleted
  // delete the root and reset pointer
  delete_leaf(subtree_root);
  subtree_root.reset();

  return true;
}


// Each node to remove only appears once (at most) in a match tree
// If the tree root is the node to be removed the cam_id becomes invalid, i.e.,  size_t(-1)
acal_match_tree::acal_match_tree(acal_match_tree const& tree,
                                 std::vector<size_t> nodes_to_remove)
{
  //make a deep copy of the input tree
  *this = tree;
  size_t n = nodes_to_remove.size();
  if(n == 0) // nothing to remove
    return; // result is the same as a copy constructor

  for(size_t i = 0; i<n; ++i){
    size_t cam_id = nodes_to_remove[i];
    std::shared_ptr<acal_match_node> found = this->find(this->root_, cam_id);
    if(!found)// node not in tree - or already removed so ok
      continue;
    size_t found_id = found->cam_id_;
    if(!delete_subtree(found)){
      std::cout << "Failed to remove subtree with root id " << cam_id << std::endl;
      continue;
    }
    if (found_id == this->root_->cam_id_) {
      root_->cam_id_ = -1;
    }
  }
  this->update_tree_size();
  //should now have a valid tree with the nodes to remove gone
}


std::shared_ptr<acal_match_node>
acal_match_tree::find(std::shared_ptr<acal_match_node> const& node, size_t cam_id)
{
  if(node->cam_id_ == cam_id)
    return node;
  size_t nc = node->size();
  for(size_t c = 0; c<nc; ++c){
    std::shared_ptr<acal_match_node> found_node = find(node->children_[c], cam_id);
    if(found_node)
      return found_node;
  }
  return std::shared_ptr<acal_match_node>();
}


bool
acal_match_tree::add_child_node(
    size_t parent_id, size_t child_id,
    std::vector<acal_match_pair> const& parent_to_child_matches)
{

  size_t min_n = 0;
  if(min_n_tracks_>=1)
    min_n = min_n_tracks_ -1;

  // find parent node_id
  std::shared_ptr<acal_match_node> parent_node = this->find(root_, parent_id);
  if(!parent_node) {
    std::cout << "parent node " << parent_id << " doesn't exist - shouldn't happen" << std::endl;
    return false;
  }

  // avoid loops in the tree
  std::shared_ptr<acal_match_node> child = this->find(root_, child_id);
  if(child) {
    //    std::cout << "child node " << child_id << " already exists - shouldn't happen" << std::endl;
    return false;
  }
  std::vector<acal_match_pair> intersected_node_to_child, intersected_sibling,min_intersected_node_to_child;
  size_t nc = parent_node->size();

  // in case the new node is being added as a child to a leaf node
  if (nc == 0) {
    // get parent of the leaf node being exended (i.e. the grand parent of the new node)
    std::shared_ptr<acal_match_node> grand_parent = parent_node->parent();

    // in case the node to which the child is being added is the root, and the root has no children
    // thus the matches are not filtered
    if (!grand_parent) {
      min_intersected_node_to_child = parent_to_child_matches;
    }
    else {// intersect the grandparent match pairs with the new tree edge match pairs
      size_t chidx = -1;
      if (!grand_parent->child_index(parent_node, chidx))
        return false;
      std::vector<acal_match_pair> grand_mpairs = grand_parent->self_to_child_matches_[chidx];
      // have to reverse the grandparent matchpair order before intersection
      // since the all tree edges have the match order parent(corr1) -> child(corr2)
      acal_match_utils::reverse(grand_mpairs);
      acal_match_utils::intersect_match_pairs(parent_to_child_matches, grand_mpairs, min_intersected_node_to_child, intersected_sibling);
      // in case the intersection is null the new node can't be added
      if (min_intersected_node_to_child.size() <= min_n)
        return false;
    }
  }

  // there are children present for the node being extended so for each child
  // intersect the sibling match pairs with the input parent_to_child_matches
  size_t min_size = -1;
  for(size_t c = 0; c<nc; ++c){
    std::vector<acal_match_pair>& sibling_matches = parent_node->self_to_child_matches_[c];
    acal_match_utils::intersect_match_pairs(parent_to_child_matches, sibling_matches, intersected_node_to_child, intersected_sibling);
    size_t isize = intersected_node_to_child.size();
    // in case the intersection is null, the new node can't be added to the tree
    if( isize <= min_n)
      return false;
    if(isize < min_size){
      min_size = isize;
      min_intersected_node_to_child = intersected_node_to_child;
    }
  }

  // the min_intersected_node_to_child match pair set has
  // been reconciled by all the sibling children of the parent node
  // so it is safe to add the new node as a child
  parent_node->add_child(child_id, min_intersected_node_to_child);
  size_t child_index = nc;

  // propagate the result across the entire tree
  // the propagation will not result in null match pairs
  // since the min_intersected_node_to_child set includes
  // pairs previously consistent with the entire tree
  prune_tree(parent_node, child_index);
  return true;
}


// a recursive function to support pruning the entire tree
// the pairwise matches of "node" must have their corr1_s present in reduced_node_corrs
void
acal_match_tree::prune_node(std::shared_ptr<acal_match_node> const& node,
                            std::vector<acal_corr> const& reduced_node_corrs)
{
  if(node->is_leaf())
    return;
  size_t nc = node->size();
  for(size_t c = 0; c<nc; ++c){
    std::vector<acal_match_pair>& node_mpairs = node->self_to_child_matches_[c];
    std::vector<acal_match_pair> reduced_node_mpairs, reduced_child_mpairs;
    acal_match_utils::reduce_node_mpairs(node_mpairs, reduced_node_corrs, reduced_node_mpairs);
    node->self_to_child_matches_[c] = reduced_node_mpairs;
    size_t nm = reduced_node_mpairs.size();
    std::vector<acal_corr> child_corrs;
    for(size_t i = 0; i<nm; ++i)
      child_corrs.push_back(reduced_node_mpairs[i].corr2_);
    // recursively apply pruning to each child node
    prune_node(node->children_[c], child_corrs);
  }
}


bool
acal_match_tree::prune_tree(std::shared_ptr<acal_match_node> const& mutated_parent_node,
                            size_t child_index)
{
  // move up the tree to the root to establish a consistent set of pairwise matches between the root node and its
  // child ancestor of "mutated_parent_node"
  std::vector<acal_match_pair> node_child_matches = mutated_parent_node->self_to_child_matches_[child_index];
  std::shared_ptr<acal_match_node> node = mutated_parent_node;
  std::vector<acal_match_pair> reduced_parent_mpairs = node_child_matches, reduced_child_mpairs ;
  while(!node->is_root())
  {
    reduced_parent_mpairs.clear();
    reduced_child_mpairs.clear();
    std::shared_ptr<acal_match_node> parent = node->parent();
    size_t cidx = -1;
    if(!parent->child_index( node, cidx)){
      std::cout << "can't find child index for parent " << parent->cam_id_ << std::endl;
      return false;
    }
    std::vector<acal_match_pair>& parent_mpairs = parent->self_to_child_matches_[cidx];
    acal_match_utils::reduce_match_pairs_a_by_b(parent_mpairs, node_child_matches, reduced_parent_mpairs, reduced_child_mpairs);
    if(reduced_parent_mpairs.size() == 0){
      std::cout << "pruned parent matches " << parent->cam_id_ << " to zero" << std::endl;
      return false;
    }
    parent->self_to_child_matches_[cidx] = reduced_parent_mpairs;
	  node->self_to_child_matches_[child_index] = reduced_child_mpairs;
    node_child_matches = reduced_parent_mpairs;
    node = find(parent, parent->cam_id_);
    child_index = cidx;
  }
  std::vector<acal_corr> node_corrs;
  size_t nr = reduced_parent_mpairs.size();
  for(size_t i = 0; i<nr; ++i)
    node_corrs.push_back(reduced_parent_mpairs[i].corr1_);

  // prune the entire tree from the root using the consistent pairwise matches just propagated to the root
  prune_node(node, node_corrs);
  return true;
}


bool
acal_match_tree::write_dot(std::ostream& ostr, std::shared_ptr<acal_match_node> const& node, size_t root_id) const
{
  if(!ostr){
    std::cout << "invalid ostream - can't write dot file" << std::endl;
    return false;
  }
  if(node->is_leaf())
    return true;
  size_t parent_id = node->cam_id_;
  size_t nc = node->size();
  for (size_t c = 0; c<nc; ++c)
  {
    size_t n_matches = node->self_to_child_matches_[c].size();
    size_t child_id = node->children_[c]->cam_id_;
    // the use of "." "root_id" keeps all the node symbols unique betwee subgraphs
    // otherwise the graph plots are messy and incorrect
    std::stringstream ss;
    if(parent_id == root_id)
      ss << parent_id << "--"<< child_id <<"." << root_id << " [ label = \" " << n_matches << "\" ];";
    else
      ss << parent_id << "." << root_id << "--" << child_id << "." << root_id << " [ label = \" " << n_matches << "\" ];";
    ostr << ss.str() << std::endl;
    write_dot(ostr, node->children_[c], root_id);
  }
  return true;
}


bool
acal_match_tree::save_tree_dot_format(std::string const& path) const
{
  std::ofstream ostr(path.c_str());
  if(!ostr){
    std::cout << "Can't open " << path << " to write dot file" << std::endl;
    return false;
  }
  ostr << "graph" << std::endl;
  ostr << "graphN {" << std::endl;
  this->write_dot(ostr, root_, root_->cam_id_);
  ostr << "}" << std::endl;
  ostr.close();
  return true;
}


void
acal_match_tree::n_nodes(std::shared_ptr<acal_match_node> const& node, size_t& n)
{
  size_t nc = node->size();
  if (nc == 0) {
    n++;
    return;
  }
  for (size_t c = 0; c<nc; ++c)
    n_nodes(node->children_[c], n);
  n++;
  return;
}


void
acal_match_tree::collect_correspondences(
    std::shared_ptr<acal_match_node>& node,
    std::map<size_t, std::vector<vgl_point_2d<double> > >& node_corrs)
{
  std::vector<vgl_point_2d<double> > temp;
  size_t nc = node->size(); // number of children

  if (nc == 0) {

    // node has no children or parent
    if (!node->has_parent()) {
      std::cout << "singleton node " << node->cam_id_
                << " (no children or parent)" << std::endl;
      return;
    }

    // node is leaf, so use second half of match_pair (corr2_) with respect to parent
    std::shared_ptr<acal_match_node> parent = node->parent();

    // get parent's child index for node
    size_t cindx;
    if (parent->child_index(node,  cindx))
    {
      // retrieve the match pairs
      std::vector<acal_match_pair> & mpairs =  parent->self_to_child_matches_[cindx];
      size_t n = mpairs.size();
      if (n == 0) {
        std::cout << "node " << node->cam_id_
                  << " has no correspondences - shouldn't happen" << std::endl;
        return;
      }

      // extract the corr2_.pt_ half of the match pair and assign to leaf
      for(size_t i = 0; i<n; ++i)
        temp.push_back(mpairs[i].corr2_.pt_);
      node_corrs[node->cam_id_] = temp;
      return;

    } else {
      std::cout << "can't find " << node->cam_id_ << "child in parent "
                << parent->cam_id_ << std::endl;
      return;
    }
  }

  // not a leaf, so just pick any child to retrive match pairs to extract corr1_ to assign to node
  std::vector<acal_match_pair> & mpairs =  node->self_to_child_matches_[0];
  size_t n = mpairs.size();
  if (n == 0) {
    std::cout << "node " << node->cam_id_ << " has no correspondences - shouldn't happen" << std::endl;
    return;
  }

  // extract corr1_ points
  for (size_t i = 0; i<n; ++i) {
    temp.push_back(mpairs[i].corr1_.pt_);
  }

  node_corrs[node->cam_id_] = temp;
  for (size_t c = 0; c<nc; ++c) {
    collect_correspondences(node->children_[c], node_corrs);
  }

  return;
}


std::vector< std::map<size_t, vgl_point_2d<double> > >
acal_match_tree::tracks()
{
  std::vector< std::map<size_t, vgl_point_2d<double> > > ret;

  std::map<size_t, std::vector<vgl_point_2d<double> > > tree_corrs;
  this->collect_correspondences(root_, tree_corrs);
  if (tree_corrs.empty()) {
    return ret;
  }

  size_t nt = tree_corrs.begin()->second.size();
  for(size_t t = 0; t<nt; ++t){
    std::map<size_t, vgl_point_2d<double> > temp;
    for(std::map<size_t, std::vector<vgl_point_2d<double> > >::iterator trit = tree_corrs.begin();
        trit != tree_corrs.end(); ++trit){
      size_t cam_id = trit->first;
      temp[cam_id] = trit->second[t];
    }
    ret.push_back(temp);
  }
  return ret;
}


// return vector of nodes sorted by cam_id
std::vector<std::shared_ptr<acal_match_node> >
acal_match_tree::nodes() const
{
  std::vector<std::shared_ptr<acal_match_node> > nodes;
  nodes_recursive(nodes, root_);
  std::sort(nodes.begin(), nodes.end(),
            [](std::shared_ptr<acal_match_node> a, std::shared_ptr<acal_match_node> b) {
                return a->cam_id_ < b->cam_id_;
            });
  return nodes;
}


// private recursive function to return children nodes from starting node
void
acal_match_tree::nodes_recursive(std::vector<std::shared_ptr<acal_match_node> >& nodes,
                                 std::shared_ptr<acal_match_node> node) const
{
  nodes.push_back(node);
  for (auto child : node->children_) {
      nodes_recursive(nodes, child);
  }
}


// return sorted vector of node cam_ids
std::vector<size_t>
acal_match_tree::cam_ids() const
{
  std::vector<size_t> ids;
  cam_ids_recursive(ids, root_);
  std::sort(ids.begin(), ids.end());
  return ids;
}


// private recursive function to return children cam_ids from starting node
void
acal_match_tree::cam_ids_recursive(std::vector<size_t>& ids,
                                   std::shared_ptr<acal_match_node> node) const
{
  ids.push_back(node->cam_id_);
  for (auto child : node->children_) {
      cam_ids_recursive(ids, child);
  }
}


// tree equality operator
bool
acal_match_tree::operator==(acal_match_tree const& other) const
{
  auto this_nodes = this->nodes();
  auto other_nodes = other.nodes();

  // check for same # of nodes
  size_t N = this_nodes.size();
  if (other_nodes.size() != N) {
    return false;
  }

  // compare node contents
  for (size_t i = 0; i < N; i++) {
    if (*this_nodes[i] != *other_nodes[i]) {
      return false;
    }
  }

  // all passed
  return true;
}


// tree output stream
void
acal_match_tree::print(std::ostream& os) const
{
  os << "acal_match_tree:\n";
  print_recursive(os, this->root_);
}


// private recursive function to output from starting node
void
acal_match_tree::print_recursive(std::ostream& os,
                                 std::shared_ptr<acal_match_node> node,
                                 std::string indent) const
{
  os << indent << *node << "\n";
  for (auto child : node->children_) {
    print_recursive(os, child, indent + "  ");
  }
}


// tree output stream
std::ostream&
operator<<(std::ostream& os, acal_match_tree const& tree)
{
  tree.print(os);
  return os;
}

