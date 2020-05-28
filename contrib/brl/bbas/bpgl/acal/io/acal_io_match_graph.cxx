#include "acal_io_match_graph.h"
#include "acal_io_match_tree.h"

#include <iostream>
#include <map>
#include <memory>

#include <vsl/vsl_pair_io.h>
#include <vsl/vsl_map_io.h>
#include <vsl/vsl_vector_io.h>


// -----match_params-----

//: Binary save object to stream
void
vsl_b_write(vsl_b_ostream & os, const match_params& obj)
{
  constexpr short io_version_no = 1;
  vsl_b_write(os, io_version_no);
  vsl_b_write(os, obj.min_n_tracks_);
  vsl_b_write(os, obj.min_n_cams_);
  vsl_b_write(os, obj.max_proj_error_);
  vsl_b_write(os, obj.max_uncal_proj_error_);
}

//: Binary load object from stream
void
vsl_b_read(vsl_b_istream & is, match_params& obj)
{
  if (!is) return;

  short io_version_no;
  vsl_b_read(is, io_version_no);
  switch (io_version_no)
  {
    case 1:
      vsl_b_read(is, obj.min_n_tracks_);
      vsl_b_read(is, obj.min_n_cams_);
      vsl_b_read(is, obj.max_proj_error_);
      vsl_b_read(is, obj.max_uncal_proj_error_);
      break;

    default:
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, match_params&), "
                << "Unknown version number "<< io_version_no << std::endl;
      is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}

//: Output a human readable summary to the stream
void vsl_print_summary(std::ostream& os, const match_params& obj)
{
  os << "Min n tracks: " << obj.min_n_tracks_ << std::endl;
  os << "Min n cams: " << obj.min_n_cams_ << std::endl;
  os << "Max proj error: " << obj.max_proj_error_ << std::endl;
  os << "Max uncal proj error: " << obj.max_uncal_proj_error_ << std::endl;
  os << std::endl;
}


// -----acal_match_graph-----

//: Binary save object to stream
void
vsl_b_write(vsl_b_ostream & os, const acal_match_graph& graph)
{
  constexpr short io_version_no = 1;
  vsl_b_write(os, io_version_no);

  // Create serializable representations of each vertex
  std::map<size_t, vertex_representation_type> serializable_vertices;
  for (auto const& item : graph.match_vertices_) {
    size_t vertex_id = item.first;
    const std::shared_ptr<match_vertex>& vertex = item.second;

    size_t cam_id = vertex->cam_id_;
    if (serializable_vertices.count(cam_id) > 0) {
      throw std::runtime_error("Can't serialize acal_match_graph - Non-unique vertex cam ID!");
    }

    // vector of edges to vector of edge IDs
    std::vector<size_t> edge_ids;
    for (match_edge* me_ptr : vertex->edges_) {
      edge_ids.push_back(me_ptr->id_);
    }

    serializable_vertices[cam_id] = std::make_pair(vertex->mark_, edge_ids);
  }

  // Create serializable representations of each edge
  std::map<size_t, edge_representation_type> serializable_edges;
  for (auto const& e_ptr : graph.match_edges_) {
    size_t edge_id = e_ptr->id_;
    if (serializable_edges.count(edge_id) > 0) {
      throw std::runtime_error("Can't pickle acal_match_graph - Non-unique edge ID!");
    }
    serializable_edges[edge_id] = std::make_tuple(e_ptr->v0_->cam_id_,
                                                  e_ptr->v1_->cam_id_,
                                                  e_ptr->matches_);
  }

  // Replace the connected component vertices with IDs
  std::vector<std::vector<size_t> > connected_components_with_ids;
  for (auto const& cc : graph.conn_comps_) {
    std::vector<size_t> verts_in_cc;
    for (auto const& v_ptr : cc) {
      verts_in_cc.push_back(v_ptr->cam_id_);
    }
    connected_components_with_ids.push_back(verts_in_cc);
  }

  // Create serializable representations of each tree
  std::map<size_t, std::map<size_t, acal_match_tree> > serializable_trees;
  for (auto const& item1 : graph.match_trees_) {
    for (auto const& item2 : item1.second) {
      // Extract the tree itself, since we can serialize that directly
      serializable_trees[item1.first][item2.first] = *(item2.second);
    }
  }

  // Serialize the state of the graph
  vsl_b_write(os, graph.params_);
  vsl_b_write(os, graph.image_paths_);
  vsl_b_write(os, graph.all_acams_);
  vsl_b_write(os, serializable_vertices);
  vsl_b_write(os, serializable_edges);
  vsl_b_write(os, connected_components_with_ids);
  vsl_b_write(os, graph.focus_tracks_);
  vsl_b_write(os, graph.focus_track_metric_);
  vsl_b_write(os, serializable_trees);
  vsl_b_write(os, graph.match_tree_metric_);
}

//: Binary load object from stream
void
vsl_b_read(vsl_b_istream & is, acal_match_graph& graph)
{
  if (!is) return;

  short io_version_no;
  vsl_b_read(is, io_version_no);
  switch (io_version_no)
  {
    case 1:
    {
      // Read in serialized state
      match_params params;
      vsl_b_read(is, params);
      std::map<size_t, std::string> image_paths;
      vsl_b_read(is, image_paths);
      std::map<size_t, vpgl_affine_camera<double> > all_acams;
      vsl_b_read(is, all_acams);
      std::map<size_t, vertex_representation_type> serialized_vertices;
      vsl_b_read(is, serialized_vertices);
      std::map<size_t, edge_representation_type> serialized_edges;
      vsl_b_read(is, serialized_edges);
      std::vector<std::vector<size_t> > connected_components_with_ids;
      vsl_b_read(is, connected_components_with_ids);
      std::map<size_t, std::map<size_t, std::vector< std::map<size_t, vgl_point_2d<double> > > > > focus_tracks;
      vsl_b_read(is, focus_tracks);
      std::vector<double> focus_track_metrics;
      vsl_b_read(is, focus_track_metrics);
      std::map<size_t, std::map<size_t, acal_match_tree> > serialized_trees;
      vsl_b_read(is, serialized_trees);
      std::vector<size_t> match_tree_metrics;
      vsl_b_read(is, match_tree_metrics);

      // Construct vertices
      std::map<size_t, std::shared_ptr<match_vertex> > vertices;
      for (auto const& item : serialized_vertices) {
        size_t vertex_id = item.first;
        auto representation = item.second;

        auto v = std::make_shared<match_vertex>(vertex_id);
        v->mark_ = representation.first;
        vertices[vertex_id] = v;
      }

      // Construct edges, use vertices just created
      std::map<size_t, std::shared_ptr<match_edge> > edges;
      for (auto const& item : serialized_edges) {
        size_t edge_id = item.first;
        auto representation = item.second;

        size_t v0_id = std::get<0>(representation);
        size_t v1_id = std::get<1>(representation);
        std::vector<acal_match_pair> matches = std::get<2>(representation);

        std::shared_ptr<match_vertex>& v0 = vertices[v0_id];
        std::shared_ptr<match_vertex>& v1 = vertices[v1_id];

        auto e = std::make_shared<match_edge>(v0, v1, matches, edge_id);
        edges[edge_id] = e;
      }

      // Put edge pointers into vertices
      for (auto const& item : vertices) {

        size_t vertex_id = item.first;
        const std::shared_ptr<match_vertex>& vertex = item.second;

        std::vector<size_t> edge_ids = serialized_vertices[vertex_id].second;
        for (auto const& edge_id : edge_ids) {
          std::shared_ptr<match_edge>& e = edges[edge_id];
          vertex->add_edge(e.get());
        }
      }

      // Construct connected components using vertices
      std::vector<std::vector<std::shared_ptr<match_vertex> > > connected_components;
      for (auto const& cc_with_ids : connected_components_with_ids) {
        std::vector<std::shared_ptr<match_vertex> > cc;
        for (size_t v_id : cc_with_ids) {
          const std::shared_ptr<match_vertex>& v = vertices[v_id];
          cc.push_back(v);
        }
        connected_components.push_back(cc);
      }

      // Convert the edges map into an edges vector, since that's what acal_match_graph
      // expects. The order of the vector doesn't seem to matter
      std::vector<std::shared_ptr<match_edge> > edges_vector;
      for (auto const& item : edges) {
        const std::shared_ptr<match_edge>& edge = item.second;
        edges_vector.push_back(edge);
      }

      // Put raw trees back in shared pointers
      std::map<size_t, std::map<size_t, std::shared_ptr<acal_match_tree> > > trees;
      for (auto const& item1 : serialized_trees) {
        for (auto const& item2 : item1.second) {
          // Copy tree from stack onto the heap, wrap in a shared pointer
          trees[item1.first][item2.first] = std::make_shared<acal_match_tree>(item2.second);
        }
      }

      // Set graph state
      graph.params_ = params;
      graph.image_paths_ = image_paths;
      graph.all_acams_ = all_acams;
      graph.match_vertices_ = vertices;
      graph.match_edges_ = edges_vector;
      graph.conn_comps_ = connected_components;
      graph.focus_tracks_ = focus_tracks;
      graph.focus_track_metric_ = focus_track_metrics;
      graph.match_trees_ = trees;
      graph.match_tree_metric_ = match_tree_metrics;

      break;
    }
    default:
    {
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, acal_match_graph&), "
                << "Unknown version number "<< io_version_no << std::endl;
      is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      return;
    }
  }
}

//: Output a human readable summary to the stream
void vsl_print_summary(std::ostream& os, const acal_match_graph& graph)
{
  os << "Number of connected components: " << graph.n_connected_comp() << std::endl;
  // TODO
  os << std::endl;
}
