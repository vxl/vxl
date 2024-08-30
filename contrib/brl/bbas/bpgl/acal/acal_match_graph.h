// This is//external/acal/acal_match_graph.h
#ifndef acal_match_graph_h
#define acal_match_graph_h

//:
// \file
// \brief A class to work with geo correction match_graph
// \author J.L. Mundy
// \date Nov 24, 2018
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include "acal_f_utils.h"
#include "acal_match_tree.h"

#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>


// forward declarations
struct match_params;
class match_vertex;
class match_edge;

struct match_params
{
  match_params() : min_n_tracks_(3), min_n_cams_(3), max_proj_error_(1.0), max_uncal_proj_error_(20.0) {}

  match_params(size_t min_n_tracks, size_t min_n_cams,
               double max_proj_error, double max_uncal_proj_error)
    : min_n_tracks_(min_n_tracks), min_n_cams_(min_n_cams),
      max_proj_error_(max_proj_error), max_uncal_proj_error_(max_uncal_proj_error)
  {}

  size_t min_n_tracks_;
  size_t min_n_cams_;
  double max_proj_error_;
  double max_uncal_proj_error_;

  bool operator==(match_params const& other) const {
    return this->min_n_tracks_ == other.min_n_tracks_ &&
           this->min_n_cams_ == other.min_n_cams_ &&
           this->max_proj_error_ == other.max_proj_error_ &&
           this->max_uncal_proj_error_ == other.max_uncal_proj_error_;
  }
  bool operator!=(match_params const& other) const {
    return !(*this == other);
  }

};


class match_vertex
{
 public:
  match_vertex(size_t cam_id = 0): cam_id_(cam_id) {}

  void add_edge(match_edge* edge) {
    std::vector<match_edge* >::iterator eit;
    eit = std::find(edges_.begin(), edges_.end(), edge);
    if (eit == edges_.end())
      edges_.push_back(edge);
  }

  void clear_edges() {
    size_t n = edges_.size();
    for (size_t i = 0; i < n; ++i)
      edges_[i] = 0;
    edges_.clear();
  }

  std::vector<size_t> edge_ids() const;

  bool operator==(match_vertex const& other) const;
  bool operator!=(match_vertex const& other) const { return !(*this == other); }

  size_t cam_id_;
  bool mark_ = false;
  std::vector<match_edge*> edges_;
};

// streaming operator
std::ostream& operator<<(std::ostream& os, match_vertex const& vertex);


class match_edge
{
 public:
  match_edge(std::shared_ptr<match_vertex> v0,
             std::shared_ptr<match_vertex> v1,
             std::vector<acal_match_pair> const& matches,
             size_t id = 0)
    : v0_(v0), v1_(v1), matches_(matches), id_(id)
  {
    v0_->add_edge(this);
    v1_->add_edge(this);
  }

  std::vector<size_t> vertex_ids() const;

  bool operator==(match_edge const& other) const;
  bool operator!=(match_edge const& other) const { return !(*this == other); }

  size_t id_;
  std::vector<acal_match_pair>  matches_;
  std::shared_ptr<match_vertex> v0_;
  std::shared_ptr<match_vertex> v1_;
};

// streaming operator
std::ostream& operator<<(std::ostream& os, match_edge const& edge);


class acal_match_graph
{
 public:
  acal_match_graph() {}
  //                         cam id i         cam id j            matches i -> j
  acal_match_graph(std::map<size_t, std::map<size_t, std::vector<acal_match_pair> > > const& incidence_matrix);

  bool load_incidence_matrix(std::map<size_t, std::map<size_t, std::vector<acal_match_pair> > > const& incidence_matrix);
  bool load_from_fmatches(std::string const& fmatches_path);
  bool load_affine_cams(std::string const& affine_cam_path);
  void adjust_affine_cams(std::map<size_t, vgl_vector_2d<double> >& cam_translations);
  void clear_vertex_marks();
  std::vector<std::shared_ptr<match_vertex> > adjacent_verts(std::shared_ptr<match_vertex> const& v);
  void visit(std::shared_ptr<match_vertex>& v, std::vector<std::shared_ptr<match_vertex> >& comp);
  void find_connected_components();
  void set_intersect_match_edge(std::shared_ptr<match_vertex> const& focus_vert,
                                match_edge* e,
                                std::vector<acal_corr>& focus_corrs,
                                std::map<size_t, std::vector<acal_corr> >& other_corrs
                                );
  bool find_joint_tracks(std::shared_ptr<match_vertex> const& focus_vert,
                         std::vector< std::map<size_t, vgl_point_2d<double> > >& joint_tracks, size_t min_n_tracks);

  void compute_focus_tracks();
  void compute_match_trees();
  bool valid_tree(std::shared_ptr<acal_match_tree> const& mtree);
  void validate_match_trees_and_set_metric();
  std::shared_ptr<acal_match_tree> largest_tree(size_t conn_comp_index);
  std::vector<std::shared_ptr<acal_match_tree> > trees(size_t conn_comp_index);

  std::shared_ptr<match_vertex> vert(size_t index) {return match_vertices_[index];}

  std::map<size_t, std::vector< std::map<size_t, vgl_point_2d<double> > > > & focus_tracks(size_t connected_comp_idx){
    return focus_tracks_[connected_comp_idx];
  }
  std::vector<std::shared_ptr<match_vertex> > ccomp_verts(size_t ccomp_index){
    if(ccomp_index >= conn_comps_.size()) return std::vector<std::shared_ptr<match_vertex> >(0);
    return conn_comps_[ccomp_index];
  }
  size_t n_connected_comp() const {return conn_comps_.size();}
  double focus_tracks_metric(size_t  const& conn_component_index = 0) const{return focus_track_metric_[conn_component_index];}

  size_t match_tree_metric(size_t conn_comp_index) const {return match_tree_metric_[conn_comp_index];}

  std::map<size_t, std::string> image_names();

  std::vector<std::pair<vgl_point_2d<double>, vgl_point_2d<double> > > corrs(size_t cam_id_i, size_t cam_id_j);

  //: debug
  void print_connected_components();
  void print_n_tracks_for_conn_comp();
  void print_focus_tracks();
  bool save_graph_dot_format(std::string const& path);
  bool save_focus_graphs_dot_format(size_t ccomp_index, std::string const& path);
  bool save_match_trees_dot_format(size_t ccomp_index, std::string const& path, size_t num_trees = -1);
  void print_bad_camera_ids();

  bool operator==(acal_match_graph const& other) const;
  bool operator!=(acal_match_graph const& other) const { return !(*this == other); }


  // members
  match_params params_;
  std::map<size_t, std::string> image_paths_;
  std::map<size_t, vpgl_affine_camera<double> > all_acams_;
  //       cam_id
  std::map<size_t, std::shared_ptr<match_vertex> > match_vertices_;
  std::vector<std::shared_ptr<match_edge> > match_edges_;
  //   c_comp      verts
  std::vector<std::vector<std::shared_ptr<match_vertex> > > conn_comps_;
  //     c_comp id     focus cam id      track          cam_id   correspondence
  std::map<size_t, std::map<size_t, std::vector< std::map<size_t, vgl_point_2d<double> > > > > focus_tracks_;
  std::vector<double> focus_track_metric_;
  //     c_comp id     focus cam id      tree
  std::map<size_t, std::map<size_t, std::shared_ptr<acal_match_tree> > > match_trees_;
  //   c_comp
  std::vector<size_t> match_tree_metric_;
  //       cam_id   count
  std::map<size_t, size_t> bad_track_camera_ids_;
};

#endif
