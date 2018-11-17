// This is brl/bbas/volm/volm_geo_index2.cxx
#include "volm_geo_index2.h"
//:
// \file
#include <bkml/bkml_write.h>
#include <vgl/vgl_intersection.h>
#include <vpgl/vpgl_utm.h>
#include "volm_io.h"
#include <vgl/vgl_area.h>

#include <utility>

// function to check whether the given box intersect with a line defined by a vector of points
static bool is_intersect(vgl_box_2d<double> const& box, std::vector<vgl_point_2d<double> > const& line)
{
  if (box.min_x() == 0.375 && box.min_y() == 0.5)
    unsigned i = 1;
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

// write node to kml
void volm_geo_index2::write_to_kml_node(std::ofstream& ofs, const volm_geo_index2_node_sptr& n, unsigned current_depth, unsigned depth, std::string explanation)
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
    bkml_write::write_box(ofs, " ", std::move(explanation), ul, ur,ll,lr);
  }
  else {
    for (const auto & c_idx : n->children_)
      write_to_kml_node(ofs, c_idx, current_depth+1, depth);
  }
}

bool volm_geo_index2::prune_tree(const volm_geo_index2_node_sptr& root, vgl_polygon<double> const& poly)
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

bool volm_geo_index2::prune_tree(const volm_geo_index2_node_sptr& root, vgl_polygon<float> const& poly)
{
  vgl_polygon<double> poly_double;
  volm_io::convert_polygons(poly, poly_double);
  return volm_geo_index2::prune_tree(root, poly_double);
}

bool volm_geo_index2::prune_by_zone(const volm_geo_index2_node_sptr& root, unsigned utm_zone)
{
  // note the tree will not be pruned if the root is outside the utm_zone;
  vpgl_utm u;
  int zone1, zone2;  double x, y;
  u.transform(root->extent_.min_point().y(), root->extent_.min_point().x(), x, y, zone1);
  u.transform(root->extent_.max_point().y(), root->extent_.max_point().x(), x, y, zone2);
  if (zone1 != (int)utm_zone && zone2 != (int)utm_zone)  // the whole quadtree is outside utm_zone
    return false;

  for (auto & i : root->children_) {
    if (!i)
      continue;
    if (!prune_by_zone(i, utm_zone))  // the child i is not in the utm_zone
      i = nullptr;  // sptr deallocates this child
  }
  return true;
}

void volm_geo_index2::write_to_kml(const volm_geo_index2_node_sptr& root, unsigned depth, std::string const& file_name)
{
  std::ofstream ofs(file_name.c_str());
  bkml_write::open_document(ofs);
  write_to_kml_node(ofs, root, 0, depth);
  bkml_write::close_document(ofs);
}

unsigned volm_geo_index2::depth(const volm_geo_index2_node_sptr& node)
{
  if (node->children_.empty())  // alreay at leaf level
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

void write_to_text(std::ofstream& ofs, const volm_geo_index2_node_sptr& n)
{
  ofs << std::setprecision(6) << std::fixed << n->extent_.min_point().x() << ' '
      << std::setprecision(6) << std::fixed << n->extent_.min_point().y() << ' '
      << std::setprecision(6) << std::fixed << n->extent_.max_point().x() << ' '
      << std::setprecision(6) << std::fixed << n->extent_.max_point().y() << '\n'
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

void volm_geo_index2::write(const volm_geo_index2_node_sptr& root, std::string const& file_name, double const& min_size)
{
  std::ofstream ofs(file_name.c_str());
  ofs << min_size << '\n';
  write_to_text(ofs, root);
}

void volm_geo_index2::get_leaves(const volm_geo_index2_node_sptr& root, std::vector<volm_geo_index2_node_sptr>& leaves)
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

void volm_geo_index2::get_leaves(const volm_geo_index2_node_sptr& root, std::vector<volm_geo_index2_node_sptr>& leaves, vgl_box_2d<double> const& area)
{
  if (!root) // the node is empty
    return;
  if (vgl_area(vgl_intersection(root->extent_,area)) == 0.0f) // the node doesn't intersect with box
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

void volm_geo_index2::get_leaves(const volm_geo_index2_node_sptr& root, std::vector<volm_geo_index2_node_sptr>& leaves, vgl_box_2d<float> const& area)
{
  vgl_box_2d<double> area_double( (double)(area.min_point().x()), (double)(area.max_point().x()),
                                  (double)(area.min_point().y()), (double)(area.max_point().y()) );
  get_leaves(root, leaves, area_double);
}

void volm_geo_index2::get_leaves(const volm_geo_index2_node_sptr& root, std::vector<volm_geo_index2_node_sptr>& leaves, vgl_polygon<double> const& poly)
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

void volm_geo_index2::get_leaves(const volm_geo_index2_node_sptr& root, std::vector<volm_geo_index2_node_sptr>& leaves, vgl_polygon<float> const& poly)
{
  vgl_polygon<double> poly_double;
  volm_io::convert_polygons(poly, poly_double);
  get_leaves(root, leaves, poly_double);
}

void volm_geo_index2::get_leaves(const volm_geo_index2_node_sptr& root, std::vector<volm_geo_index2_node_sptr>& leaves, std::vector<vgl_point_2d<double> > const& line)
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

void volm_geo_index2::get_leaves(const volm_geo_index2_node_sptr& root, std::vector<volm_geo_index2_node_sptr>& leaves, std::vector<vgl_point_2d<float> > const& line)
{
  // transfer double to float since our tree bounding box is float
  std::vector<vgl_point_2d<double> > line_double;
  auto num_pts = (unsigned)line.size();
  for (unsigned i = 0; i < num_pts; i++)
    line_double.emplace_back((double)line[i].x(), (double)line[i].y());
  get_leaves(root, leaves, line_double);
}

void volm_geo_index2::get_leaf(const volm_geo_index2_node_sptr& root, volm_geo_index2_node_sptr& leaf, vgl_point_2d<double> const& point)
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

void volm_geo_index2::get_leaf(const volm_geo_index2_node_sptr& root, volm_geo_index2_node_sptr& leaf, vgl_point_2d<float> const& point)
{
  vgl_point_2d<double> pt_double((double)point.x(), (double)point.y());
  get_leaf(root, leaf, pt_double);
}
