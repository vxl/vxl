#include <sstream>
#include <algorithm>

#include "acal_match_tree.h"


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
    std::shared_ptr<acal_match_node> grand_parent = parent_node->parent(root_);

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
    std::shared_ptr<acal_match_node> parent = node->parent(root_);
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
acal_match_tree::write_dot(std::ostream& ostr, std::shared_ptr<acal_match_node> const& node, size_t root_id)
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
acal_match_tree::save_tree_dot_format(std::string const& path)
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
acal_match_tree::assign_depth(std::shared_ptr<acal_match_node> const& node, size_t& depth)
{
  node->node_depth_ = depth;
  size_t nc = node->size();
  if(nc == 0)
    return;
  depth++;
  for(size_t c = 0; c<nc; ++c)
    assign_depth(node->children_[c],depth);
  return;
}


void
acal_match_tree::collect_nodes(std::shared_ptr<acal_match_node>& node,
                               std::vector<acal_match_node*>& nodes)
{
  nodes.push_back(node.get());
  size_t nc = node->size();
  if(nc == 0)
    return;
  for(size_t c = 0; c<nc; ++c)
    collect_nodes(node->children_[c], nodes);
  return;
}


static bool depth_greater(acal_match_node* a, acal_match_node* b){
  return (a->node_depth_ > b->node_depth_);
}


std::vector<acal_match_node*>
acal_match_tree::depth_sorted_nodes()
{
  std::vector<acal_match_node*> ret;
  size_t zero = 0;
  assign_depth(root_, zero);
  collect_nodes(root_, ret);
  std::sort(ret.begin(), ret.end(), depth_greater);
  return ret;
}


acal_match_tree::~acal_match_tree()
{
  if (root_ == nullptr) {
    return;
  }

  std::vector<acal_match_node*> dsort = this->depth_sorted_nodes();
  for (std::vector<acal_match_node*>::iterator nit = dsort.begin();
       nit != dsort.end(); ++nit) {
    (*nit)->parent_ = 0;
    (*nit)->children_.clear();
  }
}


void
acal_match_tree::n_nodes(std::shared_ptr<acal_match_node> const& node, size_t& n)
{
  size_t nc = node->size();
  if(nc == 0){
    n++;
    return;
  }
  for(size_t c = 0; c<nc; ++c)
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
  size_t nc = node->size();
  if(nc == 0) {
    // node is leaf, so use second half of match_pair (corr2_) with respect to parent
    std::shared_ptr<acal_match_node> parent = node->parent(root_);

    // get parent's child index for node
    size_t cindx;
    if(parent->child_index(node,  cindx))
    {
      // retrieve the match pairs
      std::vector<acal_match_pair> & mpairs =  parent->self_to_child_matches_[cindx];
      size_t n = mpairs.size();
      if(n == 0){
        std::cout << "node " << node->cam_id_ << " has no correspondences - shouldn't happen" << std::endl;
        return;
      }
      // extract the corr2_.pt_ half of the match pair and assign to leaf
      for(size_t i = 0; i<n; ++i)
        temp.push_back(mpairs[i].corr2_.pt_);
      node_corrs[node->cam_id_] = temp;
      return;
    } else {
      std::cout << "can't find " << node->cam_id_ << "child in parent " << parent->cam_id_ << std::endl;
      return;
    }
  }

  // not a leaf, so just pick any child to retrive match pairs to extract corr1_ to assign to node
  std::vector<acal_match_pair> & mpairs =  node->self_to_child_matches_[0];
  size_t n = mpairs.size();
  if(n == 0){
    std::cout << "node " << node->cam_id_ << " has no correspondences - shouldn't happen" << std::endl;
    return;
  }

  // extract corr1_ points
  for(size_t i = 0; i<n; ++i)
    temp.push_back(mpairs[i].corr1_.pt_);

  node_corrs[node->cam_id_] = temp;
  for(size_t c = 0; c<nc; ++c)
    collect_correspondences(node->children_[c], node_corrs);
  return;
}


std::vector< std::map<size_t, vgl_point_2d<double> > >
acal_match_tree::tracks()
{
  std::vector< std::map<size_t, vgl_point_2d<double> > > ret;

  std::map<size_t, std::vector<vgl_point_2d<double> > > tree_corrs;
  this->collect_correspondences(root_, tree_corrs);

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


std::vector<size_t>
acal_match_tree::cam_ids()
{
  std::vector<size_t> ret;
  std::vector<acal_match_node*> nodes;
  this->collect_nodes(root_, nodes);
  size_t n = nodes.size();
  for(size_t i = 0; i<n; ++i)
    ret.push_back(nodes[i]->cam_id_);
  return ret;
}
