#include "acal_io_match_tree.h"

#include <iostream>
#include <map>
#include <memory>

#include <vsl/vsl_pair_io.h>
#include <vsl/vsl_map_io.h>
#include <vsl/vsl_vector_io.h>


// -----acal_match_tree-----

//: Binary save object to stream
void
vsl_b_write(vsl_b_ostream & os, const acal_match_tree& tree)
{
  // Collect all the acal_match_node objects inside the tree
  std::vector<std::shared_ptr<acal_match_node> > nodes = tree.nodes();

  // Create an "inverse" mapping, from node objects in memory, to new node IDs
  std::map<acal_match_node*, int> node_id_map;
  for (int node_id=0; node_id < nodes.size(); node_id++) {
    node_id_map[nodes[node_id].get()] = node_id;
  }

  // Use these node IDs to create serializable representations of each node
  std::map<int, node_representation_type> serializable_nodes;
  for (auto const& item : node_id_map) {
    acal_match_node* node = item.first;
    int node_id = item.second;

    int parent_id;
    if (node->has_parent()) {
      parent_id = node_id_map[node->parent().get()];
    } else {
      // root has a null parent, so give it a special ID
      parent_id = -1;
    }

    std::vector<int> children_ids;
    for (auto const& child_ptr : node->children_) {
      int child_id = node_id_map[child_ptr.get()];
      children_ids.push_back(child_id);
    }
    serializable_nodes[node_id] = std::make_tuple(node->cam_id_,
                                                  parent_id, children_ids,
                                                  node->self_to_child_matches_);
  }

  int root_id = node_id_map[tree.root_.get()];

  constexpr short io_version_no = 1;
  vsl_b_write(os, io_version_no);
  vsl_b_write(os, serializable_nodes);
  vsl_b_write(os, tree.n_);
  vsl_b_write(os, tree.min_n_tracks_);
  vsl_b_write(os, root_id);
}


//: Binary load object from stream
void
vsl_b_read(vsl_b_istream & is, acal_match_tree& tree)
{
  if (!is) return;

  short io_version_no;
  vsl_b_read(is, io_version_no);
  switch (io_version_no)
  {
    case 1:
    {
      std::map<int, node_representation_type> serialized_nodes;
      vsl_b_read(is, serialized_nodes);

      // Construct nodes
      std::map<int, std::shared_ptr<acal_match_node> > nodes;
      for (auto const& item : serialized_nodes) {
        int node_id = item.first;
        auto representation = item.second;

        auto n = std::make_shared<acal_match_node>(std::get<0>(representation));
        n->self_to_child_matches_ = std::get<3>(representation);
        nodes[node_id] = n;
      }

      // Put node pointers into nodes
      for (auto& item : nodes) {
        int node_id = item.first;
        std::shared_ptr<acal_match_node>& node = item.second;

        int parent_id = std::get<1>(serialized_nodes[node_id]);
        std::vector<int> children_ids = std::get<2>(serialized_nodes[node_id]);

        // If this node isn't the root, fill in parent
        if (parent_id != -1) {
          const std::shared_ptr<acal_match_node>& parent = nodes[parent_id];
          node->parent(parent);
        }

        // Fill in children
        std::vector<std::shared_ptr<acal_match_node> > children;
        for (int child_id : children_ids) {
          std::shared_ptr<acal_match_node>& child = nodes[child_id];
          children.push_back(child);
        }
        node->children_ = children;
      }

      // Create tree
      vsl_b_read(is, tree.n_);
      vsl_b_read(is, tree.min_n_tracks_);
      int root_id;
      vsl_b_read(is, root_id);
      tree.root_ = nodes[root_id];

      break;
    }
    default:
    {
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, acal_match_tree&), "
                << "Unknown version number "<< io_version_no << std::endl;
      is.is().clear(std::ios::badbit);  // Set an unrecoverable IO error on stream
      return;
    }
  }
}

//: Output a human readable summary to the stream
void vsl_print_summary(std::ostream& os, const acal_match_tree& tree)
{
  os << "acal_match_tree:" << std::endl;
  os << "  Number of nodes: " << tree.n_ << std::endl;
  os << "  Min num of tracks: " << tree.min_n_tracks_ << std::endl;
  // TODO: nodes
  os << std::endl;
}
