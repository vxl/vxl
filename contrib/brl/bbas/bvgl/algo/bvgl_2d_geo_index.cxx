// This is brl/bbas/bvgl/algo/bvgl_2d_geo_index.cxx
#include "bvgl_2d_geo_index.h"
//:
// \file
#include <bkml/bkml_write.h>
#include <vgl/vgl_intersection.h>
#include <vpgl/vpgl_utm.h>
#include <vgl/vgl_line_segment_2d.h>
#include <vgl/vgl_area.h>

#include <utility>

// function to check whether the given box intersect with a line defined by a vector of points
static bool is_intersect(vgl_box_2d<double> const& box, std::vector<vgl_point_2d<double> > const& line)
{
  // Is the loop that follows correct?
  unsigned n_line_segs = line.size()-1;
  for (unsigned i = 0; i < n_line_segs; i++) {
    vgl_point_2d<double> s = line[i];  vgl_point_2d<double> e = line[i+1];
    vgl_line_segment_2d<double> line_segment(s,e);
    vgl_point_2d<double> p0, p1;
    if (vgl_intersection(box, line_segment, p0, p1) != 0)
      return true;
    if (box.contains(s) || box.contains(e))
      return true;
  }
  return false;
}

// convert a polygon from float to double
bool bvgl_2d_geo_index::convert_polygon(vgl_polygon<float> const& in_poly, vgl_polygon<double>& out_poly)
{
  out_poly.clear();
  for (unsigned i = 0; i < in_poly.num_sheets(); i++)
    out_poly.new_sheet();
  for (unsigned i = 0; i < in_poly.num_sheets(); i++)
    for (unsigned j = 0; j < (unsigned)in_poly[i].size(); j++)
      out_poly[i].push_back(vgl_point_2d<double>((double)in_poly[i][j].x(), (double)in_poly[i][j].y()));
  return true;
}

// convert a polygon from double to float
bool bvgl_2d_geo_index::convert_polygon(vgl_polygon<double> const& in_poly, vgl_polygon<float>& out_poly)
{
  out_poly.clear();
  for (unsigned i = 0; i < in_poly.num_sheets(); i++)
    out_poly.new_sheet();
  for (unsigned i = 0; i < in_poly.num_sheets(); i++)
    for (unsigned j = 0; j < (unsigned)in_poly[i].size(); j++)
      out_poly[i].push_back(vgl_point_2d<float>((float)in_poly[i][j].x(), (float)in_poly[i][j].y()));
  return true;
}

// convert a 2d box from float to double
bool bvgl_2d_geo_index::convert_box(vgl_box_2d<float> const& in, vgl_box_2d<double>& out)
{
  out.empty();
  out.set_min_x((double)(in.min_x()));  out.set_max_x((double)(in.max_x()));
  out.set_min_y((double)(in.min_y()));  out.set_max_y((double)(in.max_y()));
  return true;
}

// convert a 2d box from double to float
bool bvgl_2d_geo_index::convert_box(vgl_box_2d<double> const& in, vgl_box_2d<float>& out)
{
  out.empty();
  out.set_min_x((float)(in.min_x()));  out.set_max_x((float)(in.max_x()));
  out.set_min_y((float)(in.min_y()));  out.set_max_y((float)(in.max_y()));
  return true;
}

// write node and its children to kml
void bvgl_2d_geo_index::write_to_kml_node(std::ofstream& ofs, const bvgl_2d_geo_index_node_sptr& n, unsigned const& current_depth, unsigned const& depth, const std::string& explanation, const std::string& name)
{
  if (!n)
    return;
  if (current_depth == depth) {
    // note that in the extent bounding box, x -- lon, y -- lat (kml format: lat lon)
    vnl_double_2 ul, ll, lr, ur;
    ll[0] = n->extent_.min_point().y(); ll[1] = n->extent_.min_point().x();
    ul[0] = n->extent_.max_point().y(); ul[1] = n->extent_.min_point().x();
    lr[0] = n->extent_.min_point().y(); lr[1] = n->extent_.max_point().x();
    ur[0] = n->extent_.max_point().y(); ur[1] = n->extent_.max_point().x();
    bkml_write::write_box(ofs, name, explanation, ul, ur, ll, lr);
  }
  else {
    for (const auto & c_idx : n->children_)
      write_to_kml_node(ofs, c_idx, current_depth+1, depth, explanation, name);
  }
}

// write the quadtree node structure at certain depth
void bvgl_2d_geo_index::write_to_kml(const bvgl_2d_geo_index_node_sptr& root, unsigned const& depth, std::string const& kml_file, std::string explanation, std::string name)
{
  std::ofstream ofs(kml_file.c_str());
  bkml_write::open_document(ofs);
  write_to_kml_node(ofs, root, 0, depth, std::move(explanation), std::move(name));
  bkml_write::close_document(ofs);
}

// write node and its children to kml for quadtree having non geo coordinates
void bvgl_2d_geo_index::write_to_kml_node(std::ofstream& ofs, const bvgl_2d_geo_index_node_sptr& n, unsigned const& current_depth, unsigned const& depth, vpgl_lvcs_sptr const& lvcs, const std::string& explanation, const std::string& name)
{
  if (!n)
    return;
  if (current_depth == depth) {
    double min_lon, min_lat, max_lon, max_lat, gz;
    lvcs->local_to_global(n->extent_.min_point().x(), n->extent_.min_point().y(), 0.0, vpgl_lvcs::wgs84, min_lon, min_lat, gz);
    lvcs->local_to_global(n->extent_.max_point().x(), n->extent_.max_point().y(), 0.0, vpgl_lvcs::wgs84, max_lon, max_lat, gz);
    vnl_double_2 ul, ll, lr, ur;
    ll[0] = min_lat;  ll[1] = min_lon;
    ul[0] = max_lat;  ul[1] = min_lon;
    lr[0] = min_lat;  lr[1] = max_lon;
    ur[0] = max_lat;  ur[1] = max_lon;
    bkml_write::write_box(ofs, name, explanation, ul, ur, ll, lr);
  }
  else {
    for (const auto & c_idx : n->children_)
      write_to_kml_node(ofs, c_idx, current_depth+1, depth, lvcs, explanation, name);
  }
}

void bvgl_2d_geo_index::write_to_kml(const bvgl_2d_geo_index_node_sptr& root, unsigned const& depth, std::string const& kml_file, vpgl_lvcs_sptr const& lvcs, std::string explanation, std::string name)
{
  std::ofstream ofs(kml_file.c_str());
  bkml_write::open_document(ofs);
  write_to_kml_node(ofs, root, 0, depth, lvcs, std::move(explanation), std::move(name));
  bkml_write::close_document(ofs);
}

// return the depth of the tree
unsigned bvgl_2d_geo_index::depth(const bvgl_2d_geo_index_node_sptr& node)
{
  if (node->children_.empty())  // already at leaf level
    return 0;
  unsigned d = 0;
  for (auto & i : node->children_) {
    if (!i)
      continue;
    unsigned dd = depth(i);
    if (dd > d)
      d = dd;
  }
  return d+1;
}

// write the tree structure
void write_to_text(std::ofstream& ofs, const bvgl_2d_geo_index_node_sptr& n)
{
  ofs << std::setprecision(8) << std::fixed << n->extent_.min_point().x() << ' '
      << std::setprecision(8) << std::fixed << n->extent_.min_point().y() << ' '
      << std::setprecision(8) << std::fixed << n->extent_.max_point().x() << ' '
      << std::setprecision(8) << std::fixed << n->extent_.max_point().y() << '\n'
      << n->children_.size() << '\n';
  for (auto & i : n->children_) {
    if (!i) ofs << " 0";
    else ofs << " 1";
  }
  ofs << '\n';
  for (auto & i : n->children_) {
    if (i)
      write_to_text(ofs, i);
  }
}

void bvgl_2d_geo_index::write(const bvgl_2d_geo_index_node_sptr& root, std::string const& file_name, double const& min_size)
{
  std::ofstream ofs(file_name.c_str());
  ofs << min_size << '\n';
  write_to_text(ofs, root);
}

// write the tree structure using lvcs
void write_to_text(std::ofstream& ofs, const bvgl_2d_geo_index_node_sptr& n, vpgl_lvcs_sptr const& lvcs)
{
  // transfer the extents
  double min_lon, min_lat, max_lon, max_lat, gz;
  lvcs->local_to_global(n->extent_.min_point().x(), n->extent_.min_point().y(), 0.0, vpgl_lvcs::wgs84, min_lon, min_lat, gz);
  lvcs->local_to_global(n->extent_.max_point().x(), n->extent_.max_point().y(), 0.0, vpgl_lvcs::wgs84, max_lon, max_lat, gz);
  ofs << std::setprecision(6) << std::fixed << min_lon << ' '
      << std::setprecision(6) << std::fixed << min_lat << ' '
      << std::setprecision(6) << std::fixed << max_lon << ' '
      << std::setprecision(6) << std::fixed << max_lat << ' '
      << n->children_.size() << '\n';
  for (auto & i : n->children_) {
    if (!i) ofs << " 0";
    else ofs << " 1";
  }
  ofs << '\n';
  for (auto & i : n->children_) {
    if (i)
      write_to_text(ofs, i, lvcs);
  }
}

void bvgl_2d_geo_index::write(const bvgl_2d_geo_index_node_sptr& root, std::string const& file_name, double const& min_size, vpgl_lvcs_sptr const& lvcs)
{
  std::ofstream ofs(file_name.c_str());
  double min_size_x, min_size_y, gz;
  lvcs->local_to_global(min_size, min_size, 0.0, vpgl_lvcs::wgs84, min_size_x, min_size_y, gz);
  ofs << min_size_x << '\n';
  write_to_text(ofs, root, lvcs);
}

// prune the tree leaves by given polygon
bool bvgl_2d_geo_index::prune_tree(const bvgl_2d_geo_index_node_sptr& root, vgl_polygon<double> const& poly)
{
  // note that the tree will not be pruned if the root bounding box does not intersect with the polygon
  if (!vgl_intersection(root->extent_, poly))
    return false;

  for (auto & i : root->children_) {
    if (!i)
      continue;
    if (!prune_tree(i, poly))  // the child does not intersect with the polygon
      i = nullptr; // sptr de-allocates this child
  }
  return true;
}

bool bvgl_2d_geo_index::prune_tree(const bvgl_2d_geo_index_node_sptr& root, vgl_polygon<float> const& poly)
{
  vgl_polygon<double> poly_double;
  bvgl_2d_geo_index::convert_polygon(poly, poly_double);
  return bvgl_2d_geo_index::prune_tree(root, poly_double);
}

// return all the leaves
void bvgl_2d_geo_index::get_leaves(const bvgl_2d_geo_index_node_sptr& root, std::vector<bvgl_2d_geo_index_node_sptr>& leaves)
{
  if (!root)
    return;
  if (!root->children_.size())
    leaves.push_back(root);
  else {
    bool at_least_one_child = false;
    for (auto & i : root->children_) {
      if (!i)
        continue;
      else {
        get_leaves(i, leaves);
        at_least_one_child = true;
      }
    }
    if (!at_least_one_child)
      leaves.push_back(root);
  }
}

// return the leaves that intersect with given rectangular region
void bvgl_2d_geo_index::get_leaves(const bvgl_2d_geo_index_node_sptr& root, std::vector<bvgl_2d_geo_index_node_sptr>& leaves, vgl_box_2d<double> const& area)
{
  if (!root)  // empty tree
    return;
  if (vgl_area(vgl_intersection(root->extent_, area)) == 0.0f) // tree doesn't intersect with given region
    return;

  if (!root->children_.size())  // the node has no children and it intersects with box
    leaves.push_back(root);
  else {
    bool at_least_one_child = false;
    for (auto & i : root->children_) {
      if (!i)
        continue;
      else {
        get_leaves(i, leaves, area);
        at_least_one_child = true;
      }
    }
    if (!at_least_one_child)  // the node has children but all children are empty
      leaves.push_back(root);
  }
}

void bvgl_2d_geo_index::get_leaves(const bvgl_2d_geo_index_node_sptr& root, std::vector<bvgl_2d_geo_index_node_sptr>& leaves, vgl_box_2d<float> const& area)
{
  vgl_box_2d<double> box_double;
  bvgl_2d_geo_index::convert_box(area, box_double);
  get_leaves(root, leaves, box_double);
}

// return all leaves that intersect with polygon
void bvgl_2d_geo_index::get_leaves(const bvgl_2d_geo_index_node_sptr& root, std::vector<bvgl_2d_geo_index_node_sptr>& leaves, vgl_polygon<double> const& poly)
{
  if (!root) // the node is empty
    return;
  // check whether the polygon intersects with current root
  if (!vgl_intersection(root->extent_, poly)) // the node does not intersect with given polygon
    return;
  // the node intersects with the polygon
  if (!root->children_.size()) {  // the node intersects with the polygon and has no child
    leaves.push_back(root);
  }
  else {                          // the node has children, go inside to its children
    bool at_least_one_child = false;
    for (auto & i : root->children_) {
      if (!i)    // the node has children but child i is empty
        continue;
      else {
        get_leaves(i, leaves, poly);    // check the intersection of child i and its following children with poly
        at_least_one_child = true;
      }
    }
    if (!at_least_one_child) // the node has children but all children are empty
      leaves.push_back(root);
  }
}

void bvgl_2d_geo_index::get_leaves(const bvgl_2d_geo_index_node_sptr& root, std::vector<bvgl_2d_geo_index_node_sptr>& leaves, vgl_polygon<float> const& poly)
{
  vgl_polygon<double> poly_double;
  bvgl_2d_geo_index::convert_polygon(poly, poly_double);
  get_leaves(root, leaves, poly_double);
}

void bvgl_2d_geo_index::get_leaves(const bvgl_2d_geo_index_node_sptr& root, std::vector<bvgl_2d_geo_index_node_sptr>& leaves, std::vector<vgl_point_2d<double> > const& line)
{
  if (!root) // the tree is empty
    return;
  if (!is_intersect(root->extent_, line))  // the root does not intersect with the line
    return;
  // the node intersects with the line
  if (!root->children_.size()) {  // the node intersects with the line and has no child
    leaves.push_back(root);
  }
  else {                          // the node has children and check the intersection recursively
    bool at_least_one_child = false;
    for (auto & i : root->children_) {
      if (!i)   // the node has children but child i is empty
        continue;
      else {
        get_leaves(i, leaves, line);  // check the intersection of child i and its following children
        at_least_one_child = true;
      }
    }
    if (!at_least_one_child) // the node has children but all children are empty
      leaves.push_back(root);
  }
}

void bvgl_2d_geo_index::get_leaves(const bvgl_2d_geo_index_node_sptr& root, std::vector<bvgl_2d_geo_index_node_sptr>& leaves, std::vector<vgl_point_2d<float> > const& line)
{
  std::vector<vgl_point_2d<double> > line_double;
  auto num_pts = (unsigned)line.size();
  for (unsigned i = 0; i < num_pts; i++)
    line_double.emplace_back((double)line[i].x(), (double)line[i].y());
  get_leaves(root, leaves, line_double);
}

void bvgl_2d_geo_index::get_leaf(const bvgl_2d_geo_index_node_sptr& root, bvgl_2d_geo_index_node_sptr& leaf, vgl_point_2d<double> const& point)
{
  if (!root)
    return;
  if (!root->extent_.contains(point))
    return;
  if (!root->children_.size()) {
    leaf = root;
  }
  else {
    bool at_least_one_child = false;
    for (auto & i : root->children_) {
      if (!i)
        continue;
      else {
        get_leaf(i, leaf, point);
        at_least_one_child = true;
      }
    }
    if (!at_least_one_child)
      leaf = root;
  }
}

void bvgl_2d_geo_index::get_leaf(const bvgl_2d_geo_index_node_sptr& root, bvgl_2d_geo_index_node_sptr& leaf, vgl_point_2d<float> const& point)
{
  vgl_point_2d<double> pt_double( (double)(point.x()), (double)(point.y()) );
  get_leaf(root, leaf, pt_double);
}
