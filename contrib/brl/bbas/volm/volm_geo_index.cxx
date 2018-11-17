#include "volm_geo_index.h"
#include "volm_io.h"
#include <iomanip>
#include <iostream>
#include <utility>
//:
// \file

#include <bkml/bkml_write.h>
#include <vpgl/vpgl_utm.h>
#include <vgl/vgl_intersection.h>
#include <volm/volm_loc_hyp.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_area.h>

volm_geo_index_node::~volm_geo_index_node()
{
  children_.clear();
}

std::string volm_geo_index_node::get_label_index_name(std::string const& geo_index_name_pre, std::string const& identifier)
{
  if (identifier.compare("") == 0)
    return geo_index_name_pre + "_" + this->get_string() + "_index_label.bin";
  else
    return geo_index_name_pre + "_" + this->get_string() + "_index_label_"+identifier+".bin";
}

void construct_sub_tree(volm_geo_index_node_sptr parent, float min_size)
{
  float w = parent->extent_.width();
  float h = parent->extent_.height();
  if (w <= min_size || h <= min_size)
    return;
  // create 4 children
  vgl_point_2d<double> p1 = parent->extent_.min_point();
  vgl_point_2d<double> p2(p1.x()+w/2, p1.y()+h/2);
  vgl_box_2d<double> b1(p1, p2);
  volm_geo_index_node_sptr c1 = new volm_geo_index_node(b1, parent);
  construct_sub_tree(c1, min_size);

  p1 = vgl_point_2d<double>(parent->extent_.min_point().x(), parent->extent_.min_point().y()+h/2);
  p2 = vgl_point_2d<double>(parent->extent_.min_point().x()+w/2, parent->extent_.max_point().y());
  vgl_box_2d<double> b2(p1, p2);
  volm_geo_index_node_sptr c2 = new volm_geo_index_node(b2, parent);
  construct_sub_tree(c2, min_size);

  p1 = vgl_point_2d<double>(parent->extent_.min_point().x()+w/2, parent->extent_.min_point().y()+h/2);
  p2 = vgl_point_2d<double>(parent->extent_.max_point().x(), parent->extent_.max_point().y());
  vgl_box_2d<double> b3(p1, p2);
  volm_geo_index_node_sptr c3 = new volm_geo_index_node(b3, parent);
  construct_sub_tree(c3, min_size);

  p1 = vgl_point_2d<double>(parent->extent_.min_point().x()+w/2, parent->extent_.min_point().y());
  p2 = vgl_point_2d<double>(parent->extent_.max_point().x(), parent->extent_.min_point().y()+h/2);
  vgl_box_2d<double> b4(p1, p2);
  volm_geo_index_node_sptr c4 = new volm_geo_index_node(b4, parent);
  construct_sub_tree(c4, min_size);

  parent->children_.push_back(c1);
  parent->children_.push_back(c2);
  parent->children_.push_back(c3);
  parent->children_.push_back(c4);
}

// construct a tree such that the given tile is the root, and the hierarchy of its children form a quadtree space partition
// the stopping criterion is when a child's box is less equal than min_size arcseconds
volm_geo_index_node_sptr volm_geo_index::construct_tree(volm_tile t, float min_size)
{
  vgl_point_2d<double> p1(t.lower_left_lon(), t.lower_left_lat());
  vgl_point_2d<double> p2(p1.x()+t.scale_i_, p1.y()+t.scale_j_);
  vgl_box_2d<double> box(p1, p2);
  std::cout << "box: " << box << std::endl;
  volm_geo_index_node_sptr root = new volm_geo_index_node(box);
  // recursively add children
  construct_sub_tree(root, min_size);
  return root;
}


void construct_sub_tree_poly(volm_geo_index_node_sptr parent, float min_size, vgl_polygon<double> const& poly)
{
  float w = parent->extent_.width();
  float h = parent->extent_.height();
  if (w <= min_size || h <= min_size)
    return;
  // create 4 children
  vgl_point_2d<double> p1(parent->extent_.min_point().x(), parent->extent_.min_point().y());
  vgl_point_2d<double> p2(p1.x()+w/2, p1.y()+h/2);
  vgl_box_2d<double> b1(p1, p2);
  if (vgl_intersection(b1, poly)) {
    volm_geo_index_node_sptr c1 = new volm_geo_index_node(b1, parent);
    construct_sub_tree_poly(c1, min_size, poly);
    parent->children_.push_back(c1);
  }

  p1 = vgl_point_2d<double>(parent->extent_.min_point().x(), parent->extent_.min_point().y()+h/2);
  p2 = vgl_point_2d<double>(parent->extent_.min_point().x()+w/2, parent->extent_.max_point().y());
  vgl_box_2d<double> b2(p1, p2);
  if (vgl_intersection(b2, poly)) {
    volm_geo_index_node_sptr c2 = new volm_geo_index_node(b2, parent);
    construct_sub_tree_poly(c2, min_size, poly);
    parent->children_.push_back(c2);
  }

  p1 = vgl_point_2d<double>(parent->extent_.min_point().x()+w/2, parent->extent_.min_point().y()+h/2);
  p2 = vgl_point_2d<double>(parent->extent_.max_point().x(), parent->extent_.max_point().y());
  vgl_box_2d<double> b3(p1, p2);
  if (vgl_intersection(b3, poly)) {
    volm_geo_index_node_sptr c3 = new volm_geo_index_node(b3, parent);
    construct_sub_tree_poly(c3, min_size, poly);
    parent->children_.push_back(c3);
  }

  p1 = vgl_point_2d<double>(parent->extent_.min_point().x()+w/2, parent->extent_.min_point().y());
  p2 = vgl_point_2d<double>(parent->extent_.max_point().x(), parent->extent_.min_point().y()+h/2);
  vgl_box_2d<double> b4(p1, p2);
  if (vgl_intersection(b4, poly)) {
    volm_geo_index_node_sptr c4 = new volm_geo_index_node(b4, parent);
    construct_sub_tree_poly(c4, min_size, poly);
    parent->children_.push_back(c4);
  }
}

// construct with a polygon with possibly multiple sheets, only keep the children who intersect one of the sheets of the polygon
volm_geo_index_node_sptr volm_geo_index::construct_tree(volm_tile t, float min_size, vgl_polygon<double> const& poly)
{
  vgl_box_2d<float> box = t.bbox();
  vgl_box_2d<double> box_d = t.bbox_double();
  std::cout << "box: " << box << std::endl;
  volm_geo_index_node_sptr root;
  if (vgl_intersection(box_d, poly)) {
    root = new volm_geo_index_node(box_d);
    // recursively add children
    construct_sub_tree_poly(root, min_size, poly);
  }
  return root;
}

//: prune the children which do not intersect the poly
bool volm_geo_index::prune_tree(const volm_geo_index_node_sptr& root, vgl_polygon<double> const& poly)
{
  if (!vgl_intersection(root->extent_, poly))
    return false;

  for (auto & i : root->children_) {
    if (!i)
      continue;
    if (!prune_tree(i, poly)) // the child does not intersect poly
      i = nullptr;  // sptr deallocates this child
  }

  return true;
}

bool volm_geo_index::prune_tree(const volm_geo_index_node_sptr& root, vgl_polygon<float> const& poly)
{
  vgl_polygon<double> poly2;
  volm_io::convert_polygons(poly, poly2);
  return volm_geo_index::prune_tree(root, poly2);
}

//: prune nodes whose bbox is not in the utm_zone
bool volm_geo_index::prune_by_zone(const volm_geo_index_node_sptr& root, unsigned utm_zone)
{
  vpgl_utm u; int zone1, zone2;  double x, y;
  u.transform(root->extent_.min_point().y(), root->extent_.min_point().x(), x, y, zone1);
  u.transform(root->extent_.max_point().y(), root->extent_.max_point().x(), x, y, zone2);
  if (zone1 != (int)utm_zone && zone2 != (int)utm_zone)
    return false;

  for (auto & i : root->children_) {
    if (!i)
      continue;
    if (!prune_by_zone(i, utm_zone)) // the child is not in zone
      i = nullptr;  // sptr deallocates this child
  }
  return true;
}

unsigned volm_geo_index::depth(const volm_geo_index_node_sptr& node)
{
  if (!node->children_.size())
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

//: kml requires first lat, then lon
void volm_geo_index::write_to_kml_node(std::ofstream& ofs, const volm_geo_index_node_sptr& n, unsigned current_depth, unsigned depth, std::string explanation)
{
  if (!n)
    return;
  if (current_depth == depth) {
    vnl_double_2 ul, ll, lr, ur;
    // lat is y, lon is x
    ll[0] = n->extent_.min_point().y(); ll[1] = n->extent_.min_point().x();
    ul[0] = n->extent_.max_point().y(); ul[1] = n->extent_.min_point().x();
    lr[0] = n->extent_.min_point().y(); lr[1] = n->extent_.max_point().x();
    ur[0] = n->extent_.max_point().y(); ur[1] = n->extent_.max_point().x();
    bkml_write::write_box(ofs, " ", std::move(explanation), ul, ur,ll,lr);
  }
  else {
    for (const auto & i : n->children_)
      write_to_kml_node(ofs, i, current_depth + 1, depth);
  }
}

//: write the bboxes of the nodes at the given depth to kml file
void volm_geo_index::write_to_kml(const volm_geo_index_node_sptr& root, unsigned depth, std::string const& file_name)
{
  std::ofstream ofs(file_name.c_str());
  bkml_write::open_document(ofs);
  write_to_kml_node(ofs, root, 0, depth);
  bkml_write::close_document(ofs);
}

void write_to_text(std::ofstream& ofs, const volm_geo_index_node_sptr& n)
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

void volm_geo_index::write(const volm_geo_index_node_sptr& root, std::string const& file_name, float min_size)
{
  std::ofstream ofs(file_name.c_str());
  ofs << min_size << '\n';
  write_to_text(ofs, root);
}

volm_geo_index_node_sptr read_and_construct_node(std::ifstream& ifs, volm_geo_index_node_sptr parent)
{
  float x,y;
  ifs >> x; ifs >> y; vgl_point_2d<double> min_pt(x,y);
  ifs >> x; ifs >> y; vgl_point_2d<double> max_pt(x,y);
  vgl_box_2d<double> b(min_pt, max_pt);
  volm_geo_index_node_sptr n = new volm_geo_index_node(b, parent);
  unsigned nc;
  ifs >> nc;
  std::vector<unsigned> existence(nc);
  for (unsigned i = 0; i < nc; i++)
    ifs >> existence[i];
  for (unsigned i = 0; i < nc; i++) {
    volm_geo_index_node_sptr child;
    if (existence[i])
      child = read_and_construct_node(ifs, n);
    n->children_.push_back(child);
  }
  return n;
}

// even if a child has zero pointer, it's order in the children_ array is the same, this is to make sure that the children have consistent geographic meaning
volm_geo_index_node_sptr volm_geo_index::read_and_construct(std::string const& file_name, float& min_size)
{
  std::ifstream ifs(file_name.c_str());
  ifs >> min_size;
  volm_geo_index_node_sptr dummy_parent;
  volm_geo_index_node_sptr root = read_and_construct_node(ifs, dummy_parent);
  return root;
}

void volm_geo_index::get_leaves(const volm_geo_index_node_sptr& root, std::vector<volm_geo_index_node_sptr>& leaves)
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

//: return all the leaves that intersect a given rectangular area
void volm_geo_index::get_leaves(const volm_geo_index_node_sptr& root, std::vector<volm_geo_index_node_sptr>& leaves, vgl_box_2d<double>& area)
{
  if (!root)
    return;

  if (vgl_area(vgl_intersection(root->extent_, area)) == 0.0f)
    return;

  if (!root->children_.size())
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
    if (!at_least_one_child)
      leaves.push_back(root);
  }
}

std::string volm_geo_index_node::get_string()
{
  std::stringstream str;
  str << "node_"
      << std::setprecision(6) << std::fixed << this->extent_.min_point().x() << '_'
      << std::setprecision(6) << std::fixed << this->extent_.min_point().y() << '_'
      << std::setprecision(6) << std::fixed << this->extent_.max_point().x() << '_'
      << std::setprecision(6) << std::fixed << this->extent_.max_point().y();
  return str.str();
}

void volm_geo_index::write_hyps(const volm_geo_index_node_sptr& root, std::string const& file_name_pre)
{
  if (!root)
    return;
  if (root->hyps_ && root->hyps_->size() > 0) {
    std::string str = root->get_hyp_name(file_name_pre);
    root->hyps_->write_hypotheses(str);
  }
  for (const auto & i : root->children_)
    write_hyps(i, file_name_pre);
}

void volm_geo_index::read_hyps(const volm_geo_index_node_sptr& root, std::string const& file_name_pre)
{
  if (!root)
    return;
  std::string str = file_name_pre + "_" + root->get_string() + ".bin";
  if (vul_file::exists(str)) {
    volm_loc_hyp_sptr hyp = new volm_loc_hyp(str);
    root->hyps_ = hyp;
  }
  for (const auto & i : root->children_)
    read_hyps(i, file_name_pre);
}

unsigned volm_geo_index::hypo_size(const volm_geo_index_node_sptr& root)
{
  if (!root)
    return 0;
  unsigned cnt = 0;
  if (root->hyps_)
    cnt = root->hyps_->size();
  for (const auto & i : root->children_)
    cnt += hypo_size(i);
  return cnt;
}

bool volm_geo_index::add_hypothesis(const volm_geo_index_node_sptr& root, double lon, double lat, double elev)
{
  if (!root)
    return false;

  if (root->hyps_) {
    if (root->extent_.contains(lon, lat)) {
      root->hyps_->add(lat, lon, elev);
      return true;
    }
  } else if (!root->children_.size()) { // it's a leaf
    if (root->extent_.contains(lon, lat)) {
      root->hyps_ = new volm_loc_hyp;
      root->hyps_->add(lat, lon, elev);
      return true;
    }
  }

  bool added = false;
  for (unsigned i = 0; i < root->children_.size(); i++) {
    if (root->children_[i] && root->children_[i]->extent_.contains(lon, lat))
      added = add_hypothesis(root->children_[i], lon, lat, elev);
  }
  return added;
}

void volm_geo_index::get_leaves_with_hyps(const volm_geo_index_node_sptr& root, std::vector<volm_geo_index_node_sptr>& leaves)
{
  if (!root)
    return;
  if (!root->children_.size() && root->hyps_ && root->hyps_->size() > 0) {
    leaves.push_back(root);
    return;
  }
  bool at_least_one_child = false;
  for (auto & i : root->children_) {
    if (!i)
      continue;
    else {
      get_leaves_with_hyps(i, leaves);
      at_least_one_child = true;
    }
  }
  if (!at_least_one_child && root->hyps_ && root->hyps_->size() > 0)
    leaves.push_back(root);
}

//: return the leaf and the hyp id of the closest hyp to the given location
volm_geo_index_node_sptr volm_geo_index::get_closest(const volm_geo_index_node_sptr& root, double lat, double lon, unsigned& hyp_id)
{
  if (!root)
    return nullptr;

  vgl_point_3d<double> h;
  if (root->hyps_) {
    root->hyps_->get_closest(lat, lon, h, hyp_id);
    return root;
  }
  if (!root->children_.size())
    return nullptr;
  for (unsigned i = 0; i < root->children_.size(); i++) {
    if (!root->children_[i])
      continue;
    if (root->children_[i]->extent_.contains(lon, lat)) {
      volm_geo_index_node_sptr c = get_closest(root->children_[i], lat, lon, hyp_id);
      if (c)
        return c;
    }
  }
  return nullptr;
}

//: return true if a hyp exists within the given radius to the given point
bool volm_geo_index::exists(const volm_geo_index_node_sptr& root, double lat, double lon, double inc_in_sec_rad)
{
  if (!root)
    return false;

  vgl_point_3d<double> h;
  if (root->hyps_) {
    unsigned id;
    if (root->hyps_->exist(lat, lon, inc_in_sec_rad, id))
      return true;
  }
  if (!root->children_.size())
    return false;
  for (unsigned i = 0; i < root->children_.size(); i++) {
    if (!root->children_[i])
      continue;
    if (root->children_[i]->extent_.contains(lon, lat)) {
      if (exists(root->children_[i], lat, lon, inc_in_sec_rad))
        return true;
    }
  }

  return false;
}
