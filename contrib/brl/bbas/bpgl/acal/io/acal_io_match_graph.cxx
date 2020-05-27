#include "acal_io_match_graph.h"

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
void vsl_print_summary(std::ostream& os, match_params& obj)
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
vsl_b_write(vsl_b_ostream & os, acal_match_graph& graph)
{
  constexpr short io_version_no = 1;
  vsl_b_write(os, io_version_no);

  // Get graph attributes
  std::map<size_t, std::map<size_t, std::shared_ptr<acal_match_tree> > > match_trees = graph.get_match_trees();
  std::vector<size_t> match_tree_metrics = graph.get_match_tree_metrics();

  // Create serializable representations of each vertex
  std::map<size_t, std::shared_ptr<match_vertex> > vertices = graph.vertices();
  std::map<size_t, std::pair<bool, std::vector<size_t> > > serializable_vertices;
  for (auto const& item : vertices) {
    size_t vertex_id = item.first;
    std::shared_ptr<match_vertex> vertex = item.second;

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
  std::vector<std::shared_ptr<match_edge> > edges = graph.edges();
  std::map<size_t, std::tuple<size_t, size_t, std::vector<acal_match_pair> > > serializable_edges;
  for (auto const& e_ptr : edges) {
    size_t edge_id = e_ptr->id_;
    if (serializable_edges.count(edge_id) > 0) {
      throw std::runtime_error("Can't pickle acal_match_graph - Non-unique edge ID!");
    }
    serializable_edges[edge_id] = std::make_tuple(e_ptr->v0_->cam_id_,
                                                  e_ptr->v1_->cam_id_,
                                                  e_ptr->matches_);
  }

  // Replace the connected component vertices with IDs
  std::vector<std::vector<std::shared_ptr<match_vertex> > > connected_components = graph.get_connected_components();
  std::vector<std::vector<size_t> > connected_components_with_ids;
  for (auto const& cc : connected_components) {
    std::vector<size_t> verts_in_cc;
    for (auto const& v_ptr : cc) {
      verts_in_cc.push_back(v_ptr->cam_id_);
    }
    connected_components_with_ids.push_back(verts_in_cc);
  }

  // Serialize the state of the graph
  vsl_b_write(os, graph.get_params());
  vsl_b_write(os, graph.get_image_paths());
  vsl_b_write(os, graph.all_acams());
  vsl_b_write(os, serializable_vertices);
  vsl_b_write(os, serializable_edges);
  vsl_b_write(os, connected_components_with_ids);
  vsl_b_write(os, graph.get_focus_tracks());
  vsl_b_write(os, graph.get_focus_track_metrics());
  vsl_b_write(os, graph.get_match_trees());
  vsl_b_write(os, graph.get_match_tree_metrics());
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
      std::map<size_t, std::pair<bool, std::vector<size_t> > > serialized_vertices;
      vsl_b_read(is, serialized_vertices);
      std::map<size_t, std::tuple<size_t, size_t, std::vector<acal_match_pair> > > serialized_edges;
      vsl_b_read(is, serialized_edges);
      std::vector<std::vector<size_t> > connected_components_with_ids;
      vsl_b_read(is, connected_components_with_ids);
      std::map<size_t, std::map<size_t, std::vector< std::map<size_t, vgl_point_2d<double> > > > > focus_tracks;
      vsl_b_read(is, focus_tracks);
      std::vector<double> focus_track_metrics;
      vsl_b_read(is, focus_track_metrics);
      std::map<size_t, std::map<size_t, std::shared_ptr<acal_match_tree> > > match_trees;
      vsl_b_read(is, match_trees);
      std::vector<size_t> match_tree_metrics;
      vsl_b_read(is, match_tree_metrics);

      // Construct vertices
      std::map<size_t, std::shared_ptr<match_vertex> > vertices;
      for (auto const& item : serialized_vertices) {
        size_t vertex_id = item.first;
        auto representation = item.second;

        auto v = std::make_shared<match_vertex>();
        v->cam_id_ = vertex_id;
        v->mark_ = representation.first;
        vertices[vertex_id] = v;
      }

      // Construct edges
      std::map<size_t, std::shared_ptr<match_edge> > edges;
      for (auto const& item : serialized_edges) {
        size_t edge_id = item.first;
        auto representation = item.second;

        auto e = std::make_shared<match_edge>();
        e->id_ = edge_id;
        e->matches_ = std::get<2>(representation);
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

      // Put vertex pointers into edges
      for (auto const& item : edges) {

        size_t edge_id = item.first;
        const std::shared_ptr<match_edge>& edge = item.second;

        size_t v0_id = std::get<0>(serialized_edges[edge_id]);
        size_t v1_id = std::get<1>(serialized_edges[edge_id]);

        const std::shared_ptr<match_vertex>& v0 = vertices[v0_id];
        const std::shared_ptr<match_vertex>& v1 = vertices[v1_id];

        edge->v0_ = v0;
        edge->v1_ = v1;
      }

      // Put vertex pointers into connected components
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

      // Set graph state
      graph.set_params(params);
      graph.set_image_paths(image_paths);
      graph.set_all_acams(all_acams);
      graph.set_vertices(vertices);
      graph.set_edges(edges_vector);
      graph.set_connected_components(connected_components);
      graph.set_focus_tracks(focus_tracks);
      graph.set_focus_track_metrics(focus_track_metrics);
      graph.set_match_trees(match_trees);
      graph.set_match_tree_metrics(match_tree_metrics);

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
void vsl_print_summary(std::ostream& os, acal_match_graph& graph)
{
  os << "Number of connected components: " << graph.n_connected_comp() << std::endl;
  // TODO
  os << std::endl;
}
