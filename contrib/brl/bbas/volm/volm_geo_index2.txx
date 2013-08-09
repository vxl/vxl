// This is brl/bbas/volm/volm_geo_index2.txx
#ifndef volm_geo_index2_txx_
#define volm_geo_index2_txx_
//:
// \file
#include "volm_geo_index2.h"
#include "volm_io.h"
#include <bkml/bkml_write.h>
#include <vgl/vgl_intersection.h>
#include <vpgl/vpgl_utm.h>

#if 1
// simple deref functions
template <class Type>
Type& deref(Type* ptr) { return *ptr; }
template <class Type>
Type& deref(Type& ref) { return ref; }
template <class Type>
Type& deref(vbl_smart_ptr<Type> sptr) { return *(sptr.ptr()); }

// simple ref function
template <class Type>
Type* ref(Type& ref) { return &ref; }
template <class Type>
Type* ref(Type* ptr) { return ptr; }
template <class Type>
Type* ref(vbl_smart_ptr<Type> sptr) {return sptr.ptr(); }
#endif


//construct a tree such that the given tile is the root, and the hierarchy of its children form a quadtree space partition
//the stopping criterion is when a child's bounding box size is less or eqaul to the min_size in arcseconds
template <class Type>
void construct_sub_tree(volm_geo_index2_node_sptr parent, float const& min_size)
{
  float w = parent->extent_.width();
  float h = parent->extent_.height();
  if (w <= min_size || h <= min_size)
    return;
  // create 4 children (the arrangement follows clock wise direction, started from lower_left)
  // lower left child
  vgl_point_2d<float> p1 = parent->extent_.min_point();
  vgl_point_2d<float> p2( p1.x()+w/2.0f, p1.y()+h/2.0f);
  vgl_box_2d<float> bbox1(p1,p2);
  volm_geo_index2_node_sptr c1 = new volm_geo_index2_node<Type>(bbox1, parent);
  construct_sub_tree<Type>(c1, min_size);
  // upper left child
  p1 = vgl_point_2d<float>(parent->extent_.min_point().x(), parent->extent_.min_point().y()+h/2.0f);
  p2 = vgl_point_2d<float>(parent->extent_.min_point().x()+w/2.0f, parent->extent_.max_point().y());
  vgl_box_2d<float> bbox2(p1,p2);
  volm_geo_index2_node_sptr c2 = new volm_geo_index2_node<Type>(bbox2, parent);
  construct_sub_tree<Type>(c2, min_size);
  // upper right child
  p1 = vgl_point_2d<float>(parent->extent_.min_point().x()+w/2, parent->extent_.min_point().y()+h/2);
  p2 = vgl_point_2d<float>(parent->extent_.max_point().x(), parent->extent_.max_point().y());
  vgl_box_2d<float> bbox3(p1, p2);
  volm_geo_index2_node_sptr c3 = new volm_geo_index2_node<Type>(bbox3, parent);
  construct_sub_tree<Type>(c3, min_size);
  // lower right child
  p1 = vgl_point_2d<float>(parent->extent_.min_point().x()+w/2, parent->extent_.min_point().y());
  p2 = vgl_point_2d<float>(parent->extent_.max_point().x(), parent->extent_.min_point().y()+h/2); 
  vgl_box_2d<float> bbox4(p1, p2);
  volm_geo_index2_node_sptr c4 = new volm_geo_index2_node<Type>(bbox4, parent);
  construct_sub_tree<Type>(c4, min_size);

  parent->children_.push_back(c1);
  parent->children_.push_back(c2);
  parent->children_.push_back(c3);
  parent->children_.push_back(c4);
}

template <class Type>
void construct_sub_tree_poly(volm_geo_index2_node_sptr parent, float const& min_size, vgl_polygon<float> const& poly)
{
  float w = parent->extent_.width();
  float h = parent->extent_.height();
  if (w <= min_size || h <= min_size)
    return;
  // create 4 children (the arrangement follows clock wise direction, started from lower_left)
  // lower left child
  vgl_point_2d<float> p1 = parent->extent_.min_point();
  vgl_point_2d<float> p2( p1.x()+w/2.0f, p1.y()+h/2.0f);
  vgl_box_2d<float> bbox1(p1, p2);
  if (vgl_intersection(bbox1, poly)) {
    volm_geo_index2_node_sptr c1 = new volm_geo_index2_node<Type>(bbox1, parent);
    construct_sub_tree_poly<Type>(c1, min_size, poly);
    parent->children_.push_back(c1);
  }
  // upper left child
  p1 = vgl_point_2d<float>(parent->extent_.min_point().x(), parent->extent_.min_point().y()+h/2.0f);
  p2 = vgl_point_2d<float>(parent->extent_.min_point().x()+w/2.0f, parent->extent_.max_point().y());
  vgl_box_2d<float> bbox2(p1, p2);
  if (vgl_intersection(bbox2, poly)) {
    volm_geo_index2_node_sptr c2 = new volm_geo_index2_node<Type>(bbox2, parent);
    construct_sub_tree_poly<Type>(c2, min_size, poly);
    parent->children_.push_back(c2);
  }
  // upper right child
  p1 = vgl_point_2d<float>(parent->extent_.min_point().x()+w/2, parent->extent_.min_point().y()+h/2);
  p2 = vgl_point_2d<float>(parent->extent_.max_point().x(), parent->extent_.max_point().y());
  vgl_box_2d<float> bbox3(p1, p2);
  if (vgl_intersection(bbox3, poly)) {
    volm_geo_index2_node_sptr c3 = new volm_geo_index2_node<Type>(bbox3, parent);
    construct_sub_tree_poly<Type>(c3, min_size, poly);
    parent->children_.push_back(c3);
  }
  // lower right child
  p1 = vgl_point_2d<float>(parent->extent_.min_point().x()+w/2, parent->extent_.min_point().y());
  p2 = vgl_point_2d<float>(parent->extent_.max_point().x(), parent->extent_.min_point().y()+h/2); 
  vgl_box_2d<float> bbox4(p1, p2);
  if (vgl_intersection(bbox4, poly)) {
    volm_geo_index2_node_sptr c4 = new volm_geo_index2_node<Type>(bbox4, parent);
    construct_sub_tree_poly<Type>(c4, min_size, poly);
    parent->children_.push_back(c4);
  }
}

// write node to kml
void write_to_kml_node(vcl_ofstream& ofs, volm_geo_index2_node_sptr n, unsigned current_depth, unsigned depth)
{
  if (!n)
    return;
  if (current_depth == depth) {
    // note that in the extent bouding box, x -- lon, y -- lat (kml format: lat lon)
    vnl_double_2 ul, ll, lr, ur;
    ll[0] = n->extent_.min_point().y(); ll[1] = n->extent_.min_point().x();
    ul[0] = n->extent_.max_point().y(); ul[1] = n->extent_.min_point().x();
    lr[0] = n->extent_.min_point().y(); lr[1] = n->extent_.max_point().x();
    ur[0] = n->extent_.max_point().y(); ur[1] = n->extent_.max_point().x();
    bkml_write::write_box(ofs, " ", "location", ul, ur,ll,lr);
  }
  else {
    for (unsigned c_idx = 0; c_idx < n->children_.size(); c_idx++)
      write_to_kml_node(ofs, n->children_[c_idx], current_depth+1, depth);
  }
}

template <class Type>
volm_geo_index2_node_sptr volm_geo_index2::construct_tree(vgl_box_2d<float> bbox, float const& min_size)
{
  vcl_cout << "bounding box for geo_index root: " << bbox << vcl_endl;
  volm_geo_index2_node_sptr root = new volm_geo_index2_node<Type>(bbox);
  // recursively add children
  construct_sub_tree<Type>(root, min_size);
  return root;
}

template <class Type>
volm_geo_index2_node_sptr volm_geo_index2::construct_tree(volm_tile t, float const& min_size)
{
  // create the bbox for the root
  vgl_box_2d<float> bbox = t.bbox();
  vcl_cout << "bounding box for root: " << bbox << vcl_endl;
  return volm_geo_index2::construct_tree<Type>(bbox, min_size);
}

template <class Type>
volm_geo_index2_node_sptr volm_geo_index2::construct_tree(vgl_box_2d<float> bbox, float const& min_size, vgl_polygon<float> const& poly)
{
  vcl_cout << "bounding box for root: " << bbox << vcl_endl;
  volm_geo_index2_node_sptr root;
  if (vgl_intersection(bbox, poly)) {
    root = new volm_geo_index2_node<Type>(bbox);
    // recursively add children
    construct_sub_tree_poly<Type>(root, min_size, poly);
  }
  return root;
}

template <class Type>
volm_geo_index2_node_sptr volm_geo_index2::construct_tree(vgl_box_2d<float> bbox, float const& min_size, vgl_polygon<double> const& poly)
{
  vgl_polygon<float> poly_float;
  volm_io::convert_polygons(poly, poly_float);
  return volm_geo_index2::construct_tree<Type>(bbox, min_size, poly_float);
}

template <class Type>
volm_geo_index2_node_sptr volm_geo_index2::construct_tree(volm_tile t, float const& min_size, vgl_polygon<float> const& poly)
{
  vgl_box_2d<float> bbox = t.bbox();
  return volm_geo_index2::construct_tree<Type>(bbox, min_size, poly);
}

template <class Type>
volm_geo_index2_node_sptr volm_geo_index2::construct_tree(volm_tile t, float const& min_size, vgl_polygon<double> const& poly)
{
  vgl_polygon<float> poly_float;
  volm_io::convert_polygons(poly, poly_float);
  return volm_geo_index2::construct_tree<Type>(t, min_size, poly_float);
}

template <class Type>
volm_geo_index2_node_sptr read_and_construct_node(vcl_ifstream& ifs, volm_geo_index2_node_sptr parent)
{
  float x, y;
  ifs >> x;  ifs >> y;  vgl_point_2d<float> min_pt(x, y);
  ifs >> x;  ifs >> y;  vgl_point_2d<float> max_pt(x, y);
  vgl_box_2d<float> bbox(min_pt, max_pt);
  volm_geo_index2_node_sptr node = new volm_geo_index2_node<Type>(bbox, parent);
  unsigned nc;
  ifs >> nc;
  vcl_vector<unsigned> existence(nc);
  for (unsigned i = 0; i < nc; i++)
    ifs >> existence[i];
  for (unsigned i = 0; i < nc; i++) {
    volm_geo_index2_node_sptr child;
    if (existence[i])
      child = read_and_construct_node<Type>(ifs, node);
    node->children_.push_back(child);
  }
  return node;
}

template <class Type>
volm_geo_index2_node_sptr volm_geo_index2::read_and_construct(vcl_string const& file_name, float& min_size)
{
  vcl_ifstream ifs(file_name.c_str());
  ifs >> min_size;
  volm_geo_index2_node_sptr dummy_parent;
  volm_geo_index2_node_sptr root = read_and_construct_node<Type>(ifs, dummy_parent);
  return root;
}

bool volm_geo_index2::prune_tree(volm_geo_index2_node_sptr root, vgl_polygon<float> const& poly)
{
  // note that the tree will not be pruned if the root bounding box does not intersect with the polygon
  if (!vgl_intersection(root->extent_, poly))
    return false;

  for (unsigned i = 0; i < root->children_.size(); i++) {
    if (!root->children_[i])
      continue;
    if (!prune_tree(root->children_[i], poly))  // the child does not intersect with the polygon
      root->children_[i] = 0; // sptr deallocates this child
  }
  return true;
}

bool volm_geo_index2::prune_tree(volm_geo_index2_node_sptr root, vgl_polygon<double> const& poly)
{
  vgl_polygon<float> poly_float;
  volm_io::convert_polygons(poly, poly_float);
  return volm_geo_index2::prune_tree(root, poly_float);
}

bool volm_geo_index2::prune_by_zone(volm_geo_index2_node_sptr root, unsigned utm_zone)
{
  // note the tree will not be pruned if the root is outside the utm_zone;
  vpgl_utm u;
  int zone1, zone2;  double x, y;
  u.transform(root->extent_.min_point().y(), root->extent_.min_point().x(), x, y, zone1);
  u.transform(root->extent_.max_point().y(), root->extent_.max_point().x(), x, y, zone2);
  if (zone1 != (int)utm_zone && zone2 != (int)utm_zone)  // the whole quadtree is outside utm_zone
    return false;

  for (unsigned i = 0; i < root->children_.size(); i++) {
    if (!root->children_[i])
      continue;
    if (!prune_by_zone(root->children_[i], utm_zone))  // the child i is not in the utm_zone
      root->children_[i] = 0;  // sptr deallocates this child
  }
  return true;
}

void volm_geo_index2::write_to_kml(volm_geo_index2_node_sptr root, unsigned depth, vcl_string const& file_name)
{
  vcl_ofstream ofs(file_name.c_str());
  bkml_write::open_document(ofs);
  write_to_kml_node(ofs, root, 0, depth);
  bkml_write::close_document(ofs);
}

unsigned volm_geo_index2::depth(volm_geo_index2_node_sptr node)
{
  if (node->children_.empty())  // alreay at leaf level
    return 0;
  unsigned d = 0;
  for (unsigned i = 0; i < node->children_.size(); i++) {
    if (!node->children_[i])
      continue;
    unsigned dd = depth(node->children_[i]);
    if (dd > d)
      d = dd;
  }
  return d+1;
}

void write_to_text(vcl_ofstream& ofs, volm_geo_index2_node_sptr n)
{
  ofs << vcl_setprecision(6) << vcl_fixed << n->extent_.min_point().x() << ' '
      << vcl_setprecision(6) << vcl_fixed << n->extent_.min_point().y() << ' '
      << vcl_setprecision(6) << vcl_fixed << n->extent_.max_point().x() << ' '
      << vcl_setprecision(6) << vcl_fixed << n->extent_.max_point().y() << '\n'
      << n->children_.size() << '\n';
  for (unsigned i = 0; i < n->children_.size(); i++) {
    if (!n->children_[i]) ofs << " 0";
    else ofs << " 1";
  }
  ofs << '\n';
  for (unsigned i = 0; i < n->children_.size(); i++) {
    if (n->children_[i])
      write_to_text(ofs, n->children_[i]);
  }
}

void volm_geo_index2::write(volm_geo_index2_node_sptr root, vcl_string const& file_name, float const& min_size)
{
  vcl_ofstream ofs(file_name.c_str());
  ofs << min_size << '\n';
  write_to_text(ofs, root);
}

void volm_geo_index2::get_leaves(volm_geo_index2_node_sptr root, vcl_vector<volm_geo_index2_node_sptr>& leaves)
{
  if (!root)
    return;
  if (!root->children_.size())
    leaves.push_back(root);
  else {
    bool at_least_one_child = false;
    for (unsigned i = 0; i < root->children_.size(); i++) {
      if (!root->children_[i])
        continue;
      else {
        get_leaves(root->children_[i], leaves);
        at_least_one_child = true;
      }
    }
    if (!at_least_one_child)
      leaves.push_back(root);
  }
}

void volm_geo_index2::get_leaves(volm_geo_index2_node_sptr root, vcl_vector<volm_geo_index2_node_sptr>& leaves, vgl_box_2d<float> const& area)
{
  if (!root) // the node is empty
    return;
  if (vgl_intersection(root->extent_,area).area() == 0.0f) // the node doesn't intersect with box
    return;

  if (!root->children_.size())  // the node has no children and it intersects with box
    leaves.push_back(root);
  else {
    bool at_least_one_child = false;
    for (unsigned i = 0; i < root->children_.size(); i++) {
      if (!root->children_[i])
        continue;
      else {
        get_leaves(root->children_[i], leaves, area);
        at_least_one_child = true;
      }
    }
    if (!at_least_one_child)  // the node has children but all children are empty
      leaves.push_back(root);
  }
}

void volm_geo_index2::get_leaves(volm_geo_index2_node_sptr root, vcl_vector<volm_geo_index2_node_sptr>& leaves, vgl_box_2d<double> const& area)
{
  vgl_box_2d<float> area_float( (float)(area.min_point().x()), (float)(area.max_point().x()), (float)(area.min_point().y()), (float)(area.max_point().y()));
  get_leaves(root, leaves, area_float);
}

void volm_geo_index2::get_leaves(volm_geo_index2_node_sptr root, vcl_vector<volm_geo_index2_node_sptr>& leaves, vgl_polygon<float> const& poly)
{
  if (!root) // the node is empty
    return;
  
  // check whether the polygon 
  

  if (!vgl_intersection(root->extent_, poly)) // the node does not intersect with given polygon
    return;
  
  // the node intersects with the polyon
  if (!root->children_.size()) {  // the node intersects with the polygon and has no child
    leaves.push_back(root);
  }
  else {                          // the node has children, go inside to its children
    bool at_least_one_child = false;
    for (unsigned i = 0; i < root->children_.size(); i++) {
      if (!root->children_[i])    // the node has children but child i is empty
        continue;
      else { 
        get_leaves(root->children_[i], leaves, poly);    // check the interection of child i and its following children with poly
        at_least_one_child = true;
      }
    }
    if (!at_least_one_child) // the node has children but all children are empty
      leaves.push_back(root);
  }
}

void volm_geo_index2::get_leaves(volm_geo_index2_node_sptr root, vcl_vector<volm_geo_index2_node_sptr>& leaves, vgl_polygon<double> const& poly)
{
  vgl_polygon<float> poly_float;
  volm_io::convert_polygons(poly, poly_float);
  get_leaves(root, leaves, poly_float);
}

void volm_geo_index2::get_leaves(volm_geo_index2_node_sptr root, vcl_vector<volm_geo_index2_node_sptr>& leaves, vcl_vector<vgl_point_2d<float> > const& line)
{
  if (!root) // the tree is empy
    return;
  if (vgl_intersection(root->extent_, line).empty())  // the root does not intersect with the line
    return;

  // the node intersects with the line
  if (!root->children_.size()) {  // the node intersects with the line and has no child
    leaves.push_back(root);
  }
  else {                          // the node has children and check the intersection recursively
    bool at_least_one_child = false;
    for (unsigned i = 0; i < root->children_.size(); i++) {
      if (!root->children_[i])   // the node has children but child i is empty
        continue;
      else {
        get_leaves(root->children_[i], leaves, line);  // check the intersection of child i and its following children
        at_least_one_child = true;
      }
    }
    if (!at_least_one_child) // the node has children but all children are empty
      leaves.push_back(root);
  }
}

void volm_geo_index2::get_leaves(volm_geo_index2_node_sptr root, vcl_vector<volm_geo_index2_node_sptr>& leaves, vcl_vector<vgl_point_2d<double> > const& line)
{
  // transfer double to float since our tree bounding box is float
  vcl_vector<vgl_point_2d<float> > line_float;
  unsigned num_pts = (unsigned)line.size();
  for (unsigned i = 0; i < num_pts; i++)
    line_float.push_back(vgl_point_2d<float>((float)line[i].x(), (float)line[i].y()));
  get_leaves(root, leaves, line_float);
}

void volm_geo_index2::get_leaf(volm_geo_index2_node_sptr root, volm_geo_index2_node_sptr& leaf, vgl_point_2d<float> const& point)
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
    for (unsigned i = 0; i < root->children_.size(); i++) {
      if (!root->children_[i])
        continue;
      else {
        get_leaf(root->children_[i], leaf, point);
        at_least_one_child = true;
      }
    }
    if (!at_least_one_child)
      leaf = root;
  }
}

void volm_geo_index2::get_leaf(volm_geo_index2_node_sptr root, volm_geo_index2_node_sptr& leaf, vgl_point_2d<double> const& point)
{
  vgl_point_2d<float> pt_float((float)point.x(), (float)point.y());
  get_leaf(root, leaf, pt_float);
}

#undef VOLM_GEO_INDEX2_INSTANTIATE
#define VOLM_GEO_INDEX2_INSTANTIATE(T) \
template volm_geo_index2_node_sptr volm_geo_index2::construct_tree<T>(volm_tile t, const float& min_size);\
template volm_geo_index2_node_sptr volm_geo_index2::construct_tree<T>(volm_tile t, const float& min_size, vgl_polygon<double> const& poly);\
template volm_geo_index2_node_sptr volm_geo_index2::construct_tree<T>(volm_tile t, const float& min_size, vgl_polygon<float> const& poly);\
template volm_geo_index2_node_sptr volm_geo_index2::read_and_construct<T>(vcl_string const& flie_name, float& min_size);


#endif // volm_geo_index2_txx_
