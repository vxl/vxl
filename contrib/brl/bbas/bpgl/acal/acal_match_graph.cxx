#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>

#include "acal_match_graph.h"
#include "vul/vul_file.h"


// --------------------
// match_vertex
// --------------------

// edge ids connected to vertex
std::vector<size_t>
match_vertex::edge_ids() const
{
  std::vector<size_t> ids;
  for (auto edge : edges_) {
    ids.push_back(edge->id_);
  }
  return ids;
}


// equality operator
bool
match_vertex::operator==(match_vertex const& other) const
{
  return this->cam_id_ == other.cam_id_ &&
         this->edge_ids() == other.edge_ids();
}


// streaming operator
std::ostream&
operator<<(std::ostream& os, match_vertex const& vertex)
{
  os << "vertex " << vertex.cam_id_ << " with edges [";

  std::string separator;
  for (auto edge_id : vertex.edge_ids()) {
    os << separator << edge_id;
    separator = ",";
  }
  os << "]";

  return os;
}


// --------------------
// match_edge
// --------------------

// vertex ids connected to edge
std::vector<size_t>
match_edge::vertex_ids() const
{
  std::vector<size_t> ids = {v0_->cam_id_, v1_->cam_id_};
  return ids;
}


// equality operator
bool
match_edge::operator==(match_edge const& other) const
{
  return this->id_ == other.id_ &&
         this->vertex_ids() == other.vertex_ids() &&
         this->matches_ == other.matches_;
}


// streaming operator
std::ostream&
operator<<(std::ostream& os, match_edge const& edge)
{
  os << "edge " << edge.id_ << " connecting vertices ["
     << edge.v0_->cam_id_ << "," << edge.v1_->cam_id_
     << "] with " << edge.matches_.size() << " matches";
  return os;
}


// --------------------
// acal_match_graph
// --------------------

acal_match_graph::acal_match_graph(
    std::map<size_t, std::map<size_t, std::vector<acal_match_pair> > > const& incidence_matrix)
{
  bool success = this->load_incidence_matrix(incidence_matrix);
}


bool
acal_match_graph::load_incidence_matrix(
    //       cam id i         cam id j            matches (i, j)
    std::map<size_t, std::map<size_t, std::vector<acal_match_pair> > > const& incidence_matrix)
{
  // construct vertices
  for (std::map<size_t, std::map<size_t, std::vector<acal_match_pair> > >::const_iterator iit = incidence_matrix.begin();
      iit != incidence_matrix.end(); ++iit) {
    size_t i = iit->first;
    if (!match_vertices_[i]) {
      match_vertices_[i] = std::make_shared<match_vertex>(i);
    }
    const std::map<size_t, std::vector<acal_match_pair> >& temp = iit->second;
    for (std::map<size_t, std::vector<acal_match_pair> >::const_iterator jit = temp.begin();
        jit != temp.end(); ++jit) {
      size_t j = jit->first;
      if (!match_vertices_[j]) {
        match_vertices_[j] = std::make_shared<match_vertex>(j);
      }
    }
  }

  // construct edges
  size_t edge_id = 0;
  for (std::map<size_t, std::map<size_t, std::vector<acal_match_pair> > >::const_iterator iit = incidence_matrix.begin();
      iit != incidence_matrix.end(); ++iit) {
    size_t i = iit->first;
    std::shared_ptr<match_vertex> v0 = match_vertices_[i];
    const std::map<size_t, std::vector<acal_match_pair> >& temp = iit->second;
    for (std::map<size_t, std::vector<acal_match_pair> >::const_iterator jit = temp.begin();
        jit != temp.end(); ++jit) {
      size_t j = jit->first;
      const std::vector<acal_match_pair>& matches = jit->second;
      if (matches.size() == 0)
        continue;
      std::shared_ptr<match_vertex> v1 = match_vertices_[j];
      match_edges_.push_back(std::make_shared<match_edge>(v0, v1, matches, edge_id));
      edge_id++;
    }
  }

  return true;
}


bool
acal_match_graph::load_from_fmatches(std::string const& fmatches_path)
{
  std::map<size_t, std::map<size_t, std::vector<acal_match_pair> > > fmatches;
  std::map<size_t, std::string> image_paths;
  bool good = acal_f_utils::read_f_matches(fmatches_path, fmatches, image_paths);
  if (!good)
    return false;

  if (!this->load_incidence_matrix(fmatches))
    return false;

  image_paths_ = image_paths;
  return true;
}


bool
acal_match_graph::load_affine_cams(std::string const& affine_cam_path)
{
  return acal_f_utils::read_affine_cameras(affine_cam_path, all_acams_);
}


void
acal_match_graph::clear_vertex_marks()
{
  for (std::map<size_t, std::shared_ptr<match_vertex> >::iterator vit = match_vertices_.begin();
      vit != match_vertices_.end(); ++vit)
    (*vit).second->mark_ = false;
}


std::vector<std::shared_ptr<match_vertex> >
acal_match_graph::adjacent_verts(std::shared_ptr<match_vertex> const& v)
{
  std::vector<std::shared_ptr<match_vertex> > ret;
  for (std::vector<match_edge*>::iterator eit = v->edges_.begin();
      eit != v->edges_.end(); ++eit) {

    if ((*eit)->v0_ != v)
      ret.push_back((*eit)->v0_);

    if ((*eit)->v1_ != v)
     ret.push_back((*eit)->v1_);
  }
  return ret;
}


void
acal_match_graph::visit(
    std::shared_ptr<match_vertex>& v,
    std::vector<std::shared_ptr<match_vertex> >& comp)
{
  v->mark_ = true;
  comp.push_back(v);
  std::vector<std::shared_ptr<match_vertex> > adj_verts = adjacent_verts(v);
  for (std::vector<std::shared_ptr<match_vertex> >::iterator vit = adj_verts.begin();
      vit != adj_verts.end(); ++vit) {
    if ((*vit)->mark_)
      continue;
    visit(*vit,comp);
  }
}


void
acal_match_graph::find_connected_components()
{
  this->clear_vertex_marks();
  for (std::map<size_t, std::shared_ptr<match_vertex> >::iterator vit = match_vertices_.begin();
      vit != match_vertices_.end(); ++vit) {
    if ((*vit).second->mark_)
      continue;
    std::vector<std::shared_ptr<match_vertex> > comp;
    visit((*vit).second, comp);
    if (comp.size())
      conn_comps_.push_back(comp);
  }
}


void
acal_match_graph::set_intersect_match_edge(
    std::shared_ptr<match_vertex> const& focus_vert,
    match_edge* e,
    std::vector<acal_corr>& focus_corrs,
    std::map<size_t, std::vector<acal_corr> >& other_corrs)
{
  size_t fid = focus_vert->cam_id_; // focus cam id
  size_t oid = -1;   //other cam id
  std::shared_ptr<match_vertex> v0 = e->v0_, v1 = e->v1_; //edge verts
  size_t nm = e->matches_.size();
  if (nm == 0)
    return; //e has no matches - shouldn't happen

  // initialization step for first call
  if (focus_corrs.size() == 0 && other_corrs.size() == 0) {
    focus_corrs.resize(nm);
    if (v0->cam_id_ == fid) {  // is v0 the focus vertex?
      oid = v1->cam_id_;
      other_corrs[oid].resize(nm);
      for (size_t m = 0; m<nm; ++m) { // idx1, uv1 are focus corrs
        const acal_match_pair& pr = e->matches_[m];
        focus_corrs[m] = pr.corr1_;
        other_corrs[oid][m] = pr.corr2_;
      }
    } else {  // v1 is the focus vertex
      oid = v0->cam_id_;
      other_corrs[oid].resize(nm);
      for (size_t m = 0; m<nm; ++m) {  // idx2, uv2 are focus corrs
        const acal_match_pair& pr = e->matches_[m];
        focus_corrs[m] = pr.corr2_;
        other_corrs[oid][m] = pr.corr1_;
      }
    }
    return;
  }

  // intersect correspondences of e with current correspondence sets
  std::vector<acal_corr> inter_focus_corrs; //focus corrs after intersection
  std::map<size_t, std::vector<acal_corr> > inter_other_corrs; //other camera corrs after intersection
  if (v0->cam_id_ == fid) { // idx1, uv1 are assigned to focus
    oid = v1->cam_id_;
    for (size_t m = 0; m<nm; ++m) {
        const acal_match_pair& pr = e->matches_[m];
        size_t idf = pr.corr1_.id_, ido = pr.corr2_.id_;

        bool found = false; // find idf in focus_corrs
        for (std::vector<acal_corr>::iterator fit = focus_corrs.begin();
            fit != focus_corrs.end()&&!found; ++fit)
          found = (*fit).id_ == idf;

        if (found) {
          inter_focus_corrs.push_back(pr.corr1_);
          inter_other_corrs[oid].push_back(pr.corr2_);
        }
    }
  } else {  // idx2, uv2 are assigned to focus
    oid = v0->cam_id_;
    for (size_t m = 0; m<nm; ++m) {
      const acal_match_pair& pr = e->matches_[m];
      size_t idf = pr.corr2_.id_, ido = pr.corr1_.id_;

        bool found = false; // find idf in focus_corrs
        for (std::vector<acal_corr>::iterator fit = focus_corrs.begin();
            fit != focus_corrs.end()&&!found; ++fit)
          found = (*fit).id_ == idf;

      if (found) {
        inter_focus_corrs.push_back(pr.corr2_);
        inter_other_corrs[oid].push_back(pr.corr1_);
      }
    }
  }

  // other cam, oid, is up to date but need to remove correspondences from other cams
  size_t nif = inter_focus_corrs.size();
  for (std::map<size_t, std::vector<acal_corr > >::iterator cit = other_corrs.begin();
      cit != other_corrs.end(); ++cit) {
    size_t ocam_id = cit->first;
    std::vector<acal_corr> pruned_other_corrs;
    const std::vector<acal_corr>& cur_ocorrs = cit->second;
    size_t noc = cur_ocorrs.size();

    std::vector<acal_corr> otemp;

    size_t nf = focus_corrs.size();
    size_t nco = cur_ocorrs.size();
    if (nf != nco) {
      std::cout << "Mismatch in correspondences - fatal" << std::endl;
      focus_corrs.clear();
      other_corrs.clear();
      return;
    }
    // search through original focus corrs to find vector index of
    // the other image corr in the set of other correspondences before intersection
    for (size_t f = 0; f<nif; ++f) {
      const acal_corr& ifc = inter_focus_corrs[f];
      size_t fidx = ifc.id_;
      bool found = false;
      size_t found_j = -1;
      for (size_t j = 0; j<nf&&!found; ++j) {
        found = focus_corrs[j].id_ == fidx;
        found_j = j;
      }
      if (found)
        pruned_other_corrs.push_back(cur_ocorrs[found_j]);
    }
    other_corrs[ocam_id] = pruned_other_corrs;
  }
  other_corrs[oid] = inter_other_corrs[oid];
  focus_corrs = inter_focus_corrs;
}


bool
acal_match_graph::find_joint_tracks(
    std::shared_ptr<match_vertex> const& focus_vert,
    //   track            cam_id  correspondence
    std::vector< std::map<size_t, vgl_point_2d<double> > >& joint_tracks,
    size_t min_n_tracks)
{

  //    tracks for focus vert
  std::vector<acal_corr> focus_corrs;
  //   other cam id      tracks on other cam
  std::map<size_t, std::vector<acal_corr> > other_corrs;

  size_t fid = focus_vert->cam_id_;

  for (std::vector<match_edge*>::iterator eit = focus_vert->edges_.begin();
      eit != focus_vert->edges_.end(); ++eit) {
    // temporary cache in case intersection produces no tracks
    std::vector<acal_corr> tempf = focus_corrs;
    std::map<size_t, std::vector<acal_corr> > tempo = other_corrs;

    // intersect matches of *eit with current set of tracks
    set_intersect_match_edge(focus_vert, *eit, tempf, tempo);

    // if min number of tracks survive update the current intersection set
    // otherwise skip the edge
    if (tempf.size() >= min_n_tracks) {
      focus_corrs = tempf;
      other_corrs = tempo;
    }
  }
  // check for failure
  size_t nf = focus_corrs.size();
  if (nf < min_n_tracks)
    return false;

  // convert to form suitable for ray intersection (track form) to construct 3-d points
  joint_tracks.resize(nf);
  size_t t = 0;
  for (std::vector<acal_corr >::iterator fit = focus_corrs.begin();
      fit != focus_corrs.end(); ++fit, t++)
    joint_tracks[t][fid] = (*fit).pt_;

  for (std::map<size_t, std::vector<acal_corr> >::iterator oit = other_corrs.begin();
      oit != other_corrs.end(); ++oit) {
    size_t cid = oit->first;
    const std::vector<acal_corr>& ocorrs = oit->second;
    if (ocorrs.size() != nf) {
      std::cout << "inconsistent number of correspondences" << std::endl;
      return false;
    }
    t = 0;
    for (; t<nf; ++t)
      joint_tracks[t][cid] = ocorrs[t].pt_;
  }
  return true;
}


void
acal_match_graph::compute_focus_tracks()
{
  size_t nc = conn_comps_.size();
  focus_track_metric_.resize(nc, 0.0);

  size_t V = 0;
  for (size_t c = 0; c < nc; ++c) {
    V += conn_comps_[c].size();
  }

  for (size_t c = 0; c < nc; ++c) {
    size_t nv = conn_comps_[c].size();
    if (nv == 0) {
      continue;
    }
    double metric = 0.0;
    //   focus cam id      track          cam_id   correspondence location
    std::map<size_t, std::vector< std::map<size_t, vgl_point_2d<double> > > > c_tracks;
    for (size_t i = 0; i < nv; ++i) {
      std::shared_ptr<match_vertex>  focus_v = conn_comps_[c][i];
      if (!focus_v)
        continue;

      //    track          cam_id   correspondence location
      std::vector< std::map<size_t, vgl_point_2d<double> > > joint_tracks;
      if (!find_joint_tracks(focus_v, joint_tracks, params_.min_n_tracks_))
        continue;
      size_t ntrks = joint_tracks.size();
      if (ntrks == 0) {
        metric = -10.0;
        continue;
      }
      size_t n_connected_cams = joint_tracks[0].size();
      if (n_connected_cams < params_.min_n_cams_)
        continue;
      // update metric
      metric += static_cast<double>((n_connected_cams - 2) * (n_connected_cams - 2) * ntrks);
      c_tracks[focus_v->cam_id_] = joint_tracks;
      }
    focus_track_metric_[c] = metric * (static_cast<double>(nv) / V);
    focus_tracks_[c] = c_tracks;
  }
}


void
acal_match_graph::compute_match_trees()
{
  size_t nc = conn_comps_.size();
  // iterate over all the connected components
  for (size_t c = 0; c < nc; ++c) {
    size_t nv = conn_comps_[c].size();
    if (nv == 0)
      continue;
    // iterate over all vertices in connected component c
    for (size_t v = 0; v<nv; ++v) {
      std::shared_ptr<match_vertex> f_vert = conn_comps_[c][v];
      size_t fidx = f_vert->cam_id_;
      auto mt_ptr = std::make_shared<acal_match_tree>(fidx);
      std::map<size_t, std::shared_ptr<match_vertex> > active_verts;
      active_verts[fidx] = f_vert;
      std::set<size_t> used_verts;     // vertices already explored for a given focus vertex
      std::set<match_edge*> used_edges;// graph edges already explored for a given focus vertex
      bool first = true;
      // construct the match tree from all reachable vertices
      // from f_vert in connected component c
      while (active_verts.size() > 0) {
        size_t aidx = (active_verts.begin())->first;
        // has active vertex aidx already been explored?
        std::set<size_t>::iterator uit;
        uit = std::find(used_verts.begin(), used_verts.end(), aidx);
        if (uit != used_verts.end()) {
          active_verts.erase(*uit);
          continue;
        }
        // no, so try to extend the tree
        used_verts.insert(aidx);
        std::shared_ptr<match_vertex> a_vert = (active_verts.begin())->second;
        std::vector<match_edge*>& edges = a_vert->edges_;
        size_t ne = edges.size();
        // iterate over the graph edges for the active vertex
        for (size_t e = 0; e<ne; ++e) {
          match_edge* edge = edges[e];
          // has the edge already been explored?
          std::set<match_edge*>::iterator eit;
          eit = std::find(used_edges.begin(), used_edges.end(), edge);
          if (eit != used_edges.end())
            continue;
          // no so try to extend the tree
          used_edges.insert(edge);
          size_t id0 = edge->v0_->cam_id_;
          size_t id1 = edge->v1_->cam_id_;
          std::vector<acal_match_pair>  match_pairs = edge->matches_;
          size_t parent_id = id0, child_id = id1;
          if (id1 == aidx) {
            parent_id = id1; child_id = id0;
            acal_match_utils::reverse(match_pairs);
          }
          // try to add the child to a node of the current tree
          if (mt_ptr->add_child_node(parent_id, child_id, match_pairs))
            if (id1 == aidx)  // if success then add child node as an active vertex
              active_verts[id0] = edge->v0_;
            else
              active_verts[id1] = edge->v1_;
        }// end vert edges
        active_verts.erase(aidx);
      } // end active verts
      mt_ptr->update_tree_size();
      match_trees_[c][fidx] = mt_ptr;
    }// end focus v
  } // end conn-comp
}


bool
acal_match_graph::valid_tree(std::shared_ptr<acal_match_tree> const& mtree)
{
  // valid trees must have at least 2 nodes
  if (mtree->size() < 2)
    return false;

  std::vector< std::map<size_t, vgl_point_2d<double> > > tracks = mtree->tracks();
  std::map<size_t, std::map<size_t, vgl_point_2d<double> > > proj_tracks;
  std::map<size_t, vpgl_affine_camera<double> > tree_acams;
  std::vector<size_t> tree_cam_ids = mtree->cam_ids();

  for (std::vector<size_t>::iterator cit = tree_cam_ids.begin();
       cit != tree_cam_ids.end(); ++cit)
  {
    if (all_acams_.count(*cit) == 0) {
      std::cout << "affine camera " << *cit << " not in match graph - fatal" << std::endl;
      return false;
    }
    tree_acams[*cit] = all_acams_[*cit];
  }

  std::map<size_t, vgl_point_3d<double> > inter_pts;
  if (! acal_f_utils::intersect_tracks_with_3d(tree_acams, tracks, inter_pts, proj_tracks))
    return false;

  double max_proj_error = 0.0;
  size_t max_track = -1, max_cam_id = -1;
  for (std::map<size_t, std::map<size_t, vgl_point_2d<double> > >::iterator pit = proj_tracks.begin();
       pit != proj_tracks.end(); ++pit)
  {
    size_t tidx = pit->first; //track index
    std::map<size_t, vgl_point_2d<double> > temp = pit->second;
    for (std::map<size_t, vgl_point_2d<double> >::iterator cit = temp.begin();
         cit != temp.end(); ++cit)
    {
      size_t cam_id = cit->first;
      vgl_point_2d<double>& pt      = tracks[tidx][cam_id];
      vgl_point_2d<double>& proj_pt = cit->second;
      double proj_error = (pt-proj_pt).length();
      if (proj_error > max_proj_error) {
        max_proj_error = proj_error;
        max_track = tidx;
        max_cam_id = cam_id;
      }
    }
  }

  //std::cout << max_proj_error << ' ';
  if (max_proj_error > params_.max_uncal_proj_error_) {
    //std::cout << max_proj_error << " error exceeds limit for cam " << max_cam_id << " on track " << max_track << std::endl;
    bad_track_camera_ids_[max_cam_id]++;
    return false;
  }
  return true;
}


void
acal_match_graph::print_bad_camera_ids()
{
  std::cout << "cameras that have excessive projection error in a track" << std::endl;
  for(std::map<size_t, size_t>::iterator bit = bad_track_camera_ids_.begin();
      bit != bad_track_camera_ids_.end(); ++bit) {
        size_t nbad = bit->second;
        if (nbad == 0) continue;
        std::cout << "cam id " << bit->first << " number of tracks invalidated " << bit->second << std::endl;
  }
}


void
acal_match_graph::validate_match_trees_and_set_metric()
{
  size_t ncc = conn_comps_.size();
  match_tree_metric_.clear();
  match_tree_metric_.resize(ncc, 0);

  for (size_t cc = 0; cc<ncc; ++cc)
  {
    std::map<size_t, std::shared_ptr<acal_match_tree> >& mtrees = match_trees_[cc];
    std::map<size_t, std::shared_ptr<acal_match_tree> > temp, repaired_trees;
    size_t  n_trees = mtrees.size();
    if (n_trees==0) {
      std::cout << "no match trees for connected component " << cc << std::endl;
      continue;
    }

    // initialize bad camera map
    for (std::map<size_t, std::shared_ptr<acal_match_tree> >::iterator trit = mtrees.begin();
         trit != mtrees.end(); ++trit)
    {
      std::vector<size_t> cam_ids =  trit->second->cam_ids();
      for(size_t i = 0; i<cam_ids.size(); ++i)
        bad_track_camera_ids_[cam_ids[i]] = 0;//initialize bad in track count to 0
    }

    for (std::map<size_t, std::shared_ptr<acal_match_tree> >::iterator trit = mtrees.begin();
         trit != mtrees.end(); ++trit)
    {
      if (valid_tree(trit->second))//updates bad track camera count
        temp[trit->first] = trit->second;
    }

    // no valid match trees
    if (temp.empty())
    {
      std::cout << "all match trees failed for connected component " << cc
                << " listing cams with excessive projection error" << std::endl;
      std::vector<size_t> bad_ids;
      for (std::map<size_t, size_t>::iterator bit = bad_track_camera_ids_.begin();
           bit != bad_track_camera_ids_.end(); ++bit)
      {
        size_t nbad = bit->second;
        if (nbad > 0) {
          bad_ids.push_back(bit->first);
          std::cout << "cam id " << bit->first << ", n_bad " << nbad
                    << ", n_trees " << n_trees << std::endl;
        }
      }

      std::cout << "remove bad cameras from match trees and retry to find valid trees" << std::endl;
      std::map<size_t, std::shared_ptr<acal_match_tree> > repaired_trees;
      for(std::map<size_t, std::shared_ptr<acal_match_tree> >::iterator mit = mtrees.begin();
          mit != mtrees.end(); ++mit)
      {
        size_t fid = mit->first;
        if (bad_track_camera_ids_[fid] > 0) // skip entire tree if root is bad
          continue;
        std::shared_ptr<acal_match_tree> repaired(new acal_match_tree(*(mit->second), bad_ids));
        repaired_trees[mit->first] = repaired;
      }

      // initialize bad camera map
      bad_track_camera_ids_.clear();
      for (std::map<size_t, std::shared_ptr<acal_match_tree> >::iterator trit = repaired_trees.begin();
           trit != repaired_trees.end(); ++trit)
      {
        std::vector<size_t> cam_ids =  trit->second->cam_ids();
        for(size_t i = 0; i<cam_ids.size(); ++i)
          bad_track_camera_ids_[cam_ids[i]] = 0;//initialize bad in track count to 0
      }

      for (std::map<size_t, std::shared_ptr<acal_match_tree> >::iterator trit = repaired_trees.begin();
           trit != repaired_trees.end(); ++trit)
      {
        if (valid_tree(trit->second))//updates bad track camera count
          temp[trit->first] = trit->second;
      }
    }

    if (temp.size() > 0)
    {
      match_trees_[cc] = temp;
      std::shared_ptr<acal_match_tree> best_tree = this->largest_tree(cc);
      if (!best_tree) {
        match_tree_metric_[cc] = 0;
      } else {
        match_tree_metric_[cc] = best_tree->size();
      }
    }
  }
}


// the predicate that orders based on number of cameras in the tree
static bool
tree_size_greater(std::pair<size_t, std::shared_ptr<acal_match_tree> > const a,
                  std::pair<size_t, std::shared_ptr<acal_match_tree> > const b)
{
  return a.second->n_ > b.second->n_;
}


// sort the trees on number of cameras and return the first tree in the sorted list
std::shared_ptr<acal_match_tree>
acal_match_graph::largest_tree(size_t conn_comp_index)
{
  if (conn_comp_index == -1)
    return std::shared_ptr<acal_match_tree>();
  std::map<size_t, std::shared_ptr<acal_match_tree> > & mtrees = match_trees_[conn_comp_index];
  if (mtrees.size()==0) {
    std::cout << "no match trees for connected component " << conn_comp_index << std::endl;
    return std::shared_ptr<acal_match_tree>();
  }
  std::vector<std::pair<size_t, std::shared_ptr<acal_match_tree> > > trees;
  for (std::map<size_t, std::shared_ptr<acal_match_tree> >::iterator mit = mtrees.begin();
	  mit != mtrees.end(); ++mit) {
    std::pair<size_t, std::shared_ptr<acal_match_tree> > pr(mit->first, mit->second);
    trees.push_back(pr);
  }
  std::sort(trees.begin(), trees.end(), tree_size_greater);
  return trees[0].second;
}


void
acal_match_graph::print_connected_components()
{
  size_t nc = conn_comps_.size();
  for (size_t c =0; c<nc; ++c) {
    std::cout << "\nconnected component " << c <<
      "\n( ";
    size_t nv = conn_comps_[c].size();
    for (size_t i = 0; i<nv; ++i)
      std::cout << conn_comps_[c][i]->cam_id_ << ' ';
    std::cout << ")" << std::endl;
    for (size_t i = 0; i<nv; ++i) {
      std::string img_path = image_paths_[conn_comps_[c][i]->cam_id_];
      std::string img_name = vul_file::strip_directory(img_path);
      img_name = vul_file::strip_extension(img_name);
      std::cout << conn_comps_[c][i]->cam_id_ << ' ' << img_name << std::endl;
    }
    std::cout << std::endl;
  }
}


void
acal_match_graph::print_n_tracks_for_conn_comp()
{
    size_t nc = conn_comps_.size();
  for (size_t c =0; c<nc; ++c) {
    std::cout << "\nconnected component " << c <<  std::endl;
    size_t nv = conn_comps_[c].size();
    for (size_t i = 0; i<nv; ++i) {
      const std::shared_ptr<match_vertex>& v = conn_comps_[c][i];
      std::cout << "vertex " << v->cam_id_ << "\n( ";
      for (std::vector<match_edge*>::iterator eit = v->edges_.begin();
      eit != v->edges_.end(); ++eit)
        std::cout << (*eit)->v0_->cam_id_ << ":" << (*eit)->v1_->cam_id_ << ' ' << (*eit)->matches_.size() << ' ';
      std::cout << " )" << std::endl;
    }
  }
}


void
acal_match_graph::print_focus_tracks()
{
  std::cout << "\n======focus vertex tracks=======" << std::endl;
  //         conn comp        focus vert      tracks          cam id     track point
  for (std::map<size_t, std::map<size_t, std::vector< std::map<size_t, vgl_point_2d<double> > > > >::iterator cit = focus_tracks_.begin();
      cit != focus_tracks_.end(); ++cit) {
    size_t cc = cit->first;
    std::cout << "\nconnected component " << cc << std::endl;
    const std::map<size_t, std::vector< std::map<size_t, vgl_point_2d<double> > > >& ftemp = cit->second;
    for (std::map<size_t, std::vector< std::map<size_t, vgl_point_2d<double> > > >::const_iterator fit = ftemp.begin();
        fit != ftemp.end(); ++fit) {
      size_t focus_id = fit->first;
      std::cout << "----- focus camera id " << focus_id << std::endl;
      const std::vector< std::map<size_t, vgl_point_2d<double> > >& tr_temp = fit->second;
      size_t t = 0;
      for (std::vector< std::map<size_t, vgl_point_2d<double> > >::const_iterator trit = tr_temp.begin();
          trit != tr_temp.end(); ++trit, ++t) {
        std::cout << "   track  " << t << std::endl;
        const std::map<size_t, vgl_point_2d<double> >& tcorrs = *trit;
        for (std::map<size_t, vgl_point_2d<double> >::const_iterator cmpit = tcorrs.begin();
            cmpit != tcorrs.end(); ++cmpit) {
          std::cout << "     " << cmpit->first << ' ' << cmpit->second.x() << ' ' << cmpit->second.y() << std::endl;
        }
      }
    }
    std::cout << std::endl;
  }
}


void
acal_match_graph::adjust_affine_cams(std::map<size_t, vgl_vector_2d<double> >& cam_translations)
{
  for (std::map<size_t, vgl_vector_2d<double> >::iterator cit = cam_translations.begin();
      cit != cam_translations.end(); ++cit) {
    size_t cidx = cit->first;
    if (!all_acams_.count(cidx))
      continue;
    vpgl_affine_camera<double>& acam = all_acams_[cidx];
    vnl_matrix_fixed<double, 3, 4> m = acam.get_matrix();
    m[0][3] += cit->second.x();  m[1][3] += cit->second.y();
    acam.set_matrix(m);
  }
}


bool
acal_match_graph::save_graph_dot_format(std::string const& path)
{
  std::ofstream ostr(path.c_str());
  if (!ostr) {
    std::cout << "Can't open " << path << " to write dot file" << std::endl;
    return false;
  }
  ostr << "graph" << std::endl;
  ostr << "graphN {" << std::endl;

  size_t ne = match_edges_.size();
  if (!ne) {
    std::cout << "no edges in graph" << std::endl;
    return false;
  }
  size_t ncc = conn_comps_.size();
  std::vector<std::vector<std::string> > ccomp(ncc);
  for (size_t i =0; i<ne; ++i) {
    std::shared_ptr<match_edge> e = match_edges_[i];
    if (!e)
      continue;
    size_t n_matches = e->matches_.size();
    size_t v0_id = e->v0_->cam_id_;
    size_t v1_id = e->v1_->cam_id_;
    std::stringstream ss;
    ss << v0_id << "--"<< v1_id << " [ label = \" " << n_matches << "\" ];";
    size_t cc = -1;
    for (size_t c = 0; c<ncc; ++c) {
      bool found = false;
      for (std::vector<std::shared_ptr<match_vertex> >::iterator vit = conn_comps_[c].begin();
          vit != conn_comps_[c].end()&&!found; ++vit)
        if ((*vit)->cam_id_ == v0_id ||(*vit)->cam_id_ == v1_id) {
          found = true;
          cc = c;
        }
      if (!found)
        continue;
    }
    if (cc == -1) {
      std::cout << "edge(" << v0_id << ' ' << v1_id << ") not found in any connected component" << std::endl;
      return false;
    }
    ccomp[cc].push_back(ss.str());
  }

  for (size_t c = 0; c<ncc; ++c) {
    std::stringstream scc;
    // note, a subgraph id must begin with cluster
    scc << "subgraph cluster_cc" << c << " { label = \"conn comp " << c << " m = " << focus_track_metric_[c]<< "\";";
    ostr << scc.str() << std::endl;
    for (size_t s = 0; s<ccomp[c].size();++s)
      ostr << ccomp[c][s] << std::endl;
    ostr << "}" << std::endl;
  }
  ostr << "}" << std::endl;
  ostr.close();
  return true;
}


bool
acal_match_graph::save_focus_graphs_dot_format(size_t ccomp_index, std::string const& path)
{
  std::ofstream ostr(path.c_str());
  if (!ostr) {
    std::cout << "Can't open " << path << " to write dot file" << std::endl;
    return false;
  }
  ostr << "graph" << std::endl;
  ostr << "graphN {" << std::endl;

  std::map<size_t, std::vector< std::map<size_t, vgl_point_2d<double> > > >& ftrk = focus_tracks_[ccomp_index];
  if (ftrk.size()==0) {
    std::cout << "no focus tracks for connected component " << ccomp_index << std::endl;
    return false;
  }
  for (std::map<size_t, std::vector< std::map<size_t, vgl_point_2d<double> > > >::iterator fit = ftrk.begin();
      fit != ftrk.end(); ++fit) {
    size_t cidx = fit->first;
    std::vector< std::map<size_t, vgl_point_2d<double> > >& trks = fit->second;
    size_t ntrk = trks.size();
    if (ntrk == 0)
      continue;
    std::map<size_t, vgl_point_2d<double> >& trk = trks[0];
    std::stringstream ss;
    ss << " subgraph cluster" << cidx << " {\n label = \"focus vert id " << cidx << "\";";
    ostr << ss.str() << std::endl;
    for (std::map<size_t, vgl_point_2d<double> >::iterator cit = trk.begin();
        cit != trk.end(); ++cit) {
      if (cit->first == cidx)
        continue;
      std::stringstream css;
      css << "  " << cidx << "--" << cit->first<<"." << cidx << " [ label = \" " << ntrk << "\" ];";
      ostr << css.str() << std::endl;
    }
    ostr << " }" << std::endl;
  }
  ostr << "}" << std::endl;
  ostr.close();
  return true;
}


bool
acal_match_graph::save_match_trees_dot_format(size_t ccomp_index, std::string const& path, size_t num_trees)
{
  std::ofstream ostr(path.c_str());
  if (!ostr) {
    std::cout << "Can't open " << path << " to write dot file" << std::endl;
    return false;
  }
  ostr << "graph" << std::endl;
  ostr << "graphN {" << std::endl;

  std::map<size_t, std::shared_ptr<acal_match_tree> > & mtrees = match_trees_[ccomp_index];
  if (mtrees.size()==0) {
    std::cout << "no focus trees for connected component " << ccomp_index << std::endl;
    return false;
  }
  std::vector<std::pair<size_t, std::shared_ptr<acal_match_tree> > > trees;
  for (std::map<size_t, std::shared_ptr<acal_match_tree> >::iterator mit = mtrees.begin();
	  mit != mtrees.end(); ++mit) {
	  std::pair<size_t, std::shared_ptr<acal_match_tree> > pr(mit->first, mit->second);
	  trees.push_back(pr);
  }
  std::sort(trees.begin(), trees.end(), tree_size_greater);
  size_t n = 0;
  size_t nt = trees.size();
  for (size_t i = 0; i<nt&&n<=num_trees; ++i, ++n) {
    size_t cidx = trees[i].first;
    std::shared_ptr<acal_match_tree> mt_ptr = trees[i].second;
    size_t nn = 0;
    mt_ptr->n_nodes(mt_ptr->root_, nn);
    std::cout << "n_nodes " << nn << std::endl;
    std::stringstream ss;
    ss << " subgraph cluster" << cidx << " {\n label = \"focus vert id " << cidx << "\";";
    ostr << ss.str() << std::endl;
    mt_ptr->write_dot(ostr, mt_ptr->root_, mt_ptr->root_->cam_id_);
    ostr << " }" << std::endl;
  }
  ostr << "}" << std::endl;
  ostr.close();
  return true;
}


std::map<size_t, std::string>
acal_match_graph::image_names()
{
  std::map<size_t, std::string> ret;
  for (std::map<size_t, std::string>::iterator sit = image_paths_.begin();
      sit != image_paths_.end(); ++sit) {
    size_t iit = sit->first;
    std::string path = sit->second;
    std::string temp = vul_file::strip_directory(path);
    temp = vul_file::strip_extension(temp);
    ret[iit] = temp;
  }
  return ret;
}


std::vector<std::shared_ptr<acal_match_tree> >
acal_match_graph::trees(size_t conn_comp_index)
{
  std::vector<std::shared_ptr<acal_match_tree> > ret;
  //      focus cam id               tree
  std::map<size_t, std::shared_ptr<acal_match_tree> > trees = match_trees_[conn_comp_index];

  for (std::map<size_t, std::shared_ptr<acal_match_tree> >::iterator mit = trees.begin();
      mit != trees.end(); ++mit)
    ret.push_back(mit->second);
  return ret;
}


// equality for iterable (std::vector, std::map) of std::shared_ptr
// requires special handling to compare dereferenced shared_ptr content
// std::equal pattern from https://stackoverflow.com/a/8473603
template<typename T>
bool
is_equal(std::vector<T> const& lhs, std::vector<T> const& rhs)
{
  auto pred = [] (decltype(*lhs.begin()) a, decltype(a) b)
                 { return is_equal(a, b); };

  return lhs.size() == rhs.size() &&
         std::equal(lhs.begin(), lhs.end(), rhs.begin(), pred);
}

template<typename K, typename T>
bool
is_equal(std::map<K, T> const& lhs, std::map<K, T> const& rhs)
{
  auto pred = [] (decltype(*lhs.begin()) a, decltype(a) b)
                 { return a.first == b.first && is_equal(a.second, b.second); };

  return lhs.size() == rhs.size() &&
         std::equal(lhs.begin(), lhs.end(), rhs.begin(), pred);
}

template<typename T>
bool
is_equal(std::shared_ptr<T> const& lhs, std::shared_ptr<T> const& rhs)
{
  return *lhs == *rhs;
}


// equality operator
bool
acal_match_graph::operator==(acal_match_graph const& other) const
{
  return this->params_ == other.params_
      && this->image_paths_ == other.image_paths_
      && this->all_acams_ == other.all_acams_
      && is_equal(this->match_vertices_, other.match_vertices_)
      && is_equal(this->match_edges_, other.match_edges_)
      && is_equal(this->conn_comps_, other.conn_comps_)
      && this->focus_tracks_ == other.focus_tracks_
      && this->focus_track_metric_ == other.focus_track_metric_
      && is_equal(this->match_trees_, other.match_trees_)
      && this->match_tree_metric_ == other.match_tree_metric_;
}

std::vector<std::pair<vgl_point_2d<double>, vgl_point_2d<double> > > acal_match_graph::corrs(size_t cam_id_i, size_t cam_id_j){
  std::vector<std::pair<vgl_point_2d<double>, vgl_point_2d<double> > >  ret;
  std::shared_ptr<match_vertex> vi = match_vertices_[cam_id_i],
              vj = match_vertices_[cam_id_i];
  if(!vi || !vj) return ret;
  std::vector<match_edge*>& i_edges = vi->edges_;
  size_t ne = i_edges.size();
  if(ne == 0)
    return ret;
  bool found = false;
  for(size_t e = 0; e<ne&&!found; ++e){
    std::shared_ptr<match_vertex> vv = i_edges[e]->v1_;
    if(vv->cam_id_ == cam_id_j){
      std::vector<acal_match_pair>& mch =  i_edges[e]->matches_;
      size_t nm = mch.size();
      if(nm == 0)
        return ret;
      for(size_t m = 0; m<nm; ++m){
        acal_match_pair& p = mch[m];
        ret.emplace_back(p.corr1_.pt_, p.corr2_.pt_);
      }
      found = true;
    }
  }
  return ret;
}
      
  
              
    
