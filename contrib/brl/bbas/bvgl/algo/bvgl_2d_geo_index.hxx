// This is brl/bbas/bvgl/algo/bvgl_2d_geo_index.hxx
#ifndef bvgl_2d_geo_index_hxx_
#define bvgl_2d_geo_index_hxx_
//:
// \file
#include "bvgl_2d_geo_index.h"
#include <bkml/bkml_write.h>
#include <vgl/vgl_intersection.h>
#include <vpgl/vpgl_utm.h>

//construct a tree such that the given tile is the root, and the hierarchy of its children form a quad-tree space partition
//the stopping criterion is when a child's bounding box size is less or equal to the min_size in arcseconds
template <class Type>
void construct_sub_tree(bvgl_2d_geo_index_node_sptr parent, double const& min_size)
{
  double w = parent->extent_.width();
  double h = parent->extent_.height();
  if (w <= min_size || h <= min_size)
    return;
  // create 4 children (the arrangement follows clock wise direction, started from lower_left)
  // lower left child
  vgl_point_2d<double> p1 = parent->extent_.min_point();
  vgl_point_2d<double> p2( p1.x()+w/2.0, p1.y()+h/2.0);
  vgl_box_2d<double> bbox1(p1,p2);
  bvgl_2d_geo_index_node_sptr c1 = new bvgl_2d_geo_index_node<Type>(bbox1, parent);
  construct_sub_tree<Type>(c1, min_size);
  // upper left child
  p1 = vgl_point_2d<double>(parent->extent_.min_point().x(), parent->extent_.min_point().y()+h/2.0);
  p2 = vgl_point_2d<double>(parent->extent_.min_point().x()+w/2.0, parent->extent_.max_point().y());
  vgl_box_2d<double> bbox2(p1,p2);
  bvgl_2d_geo_index_node_sptr c2 = new bvgl_2d_geo_index_node<Type>(bbox2, parent);
  construct_sub_tree<Type>(c2, min_size);
  // upper right child
  p1 = vgl_point_2d<double>(parent->extent_.min_point().x()+w/2.0, parent->extent_.min_point().y()+h/2.0);
  p2 = vgl_point_2d<double>(parent->extent_.max_point().x(), parent->extent_.max_point().y());
  vgl_box_2d<double> bbox3(p1, p2);
  bvgl_2d_geo_index_node_sptr c3 = new bvgl_2d_geo_index_node<Type>(bbox3, parent);
  construct_sub_tree<Type>(c3, min_size);
  // lower right child
  p1 = vgl_point_2d<double>(parent->extent_.min_point().x()+w/2.0, parent->extent_.min_point().y());
  p2 = vgl_point_2d<double>(parent->extent_.max_point().x(), parent->extent_.min_point().y()+h/2.0);
  vgl_box_2d<double> bbox4(p1, p2);
  bvgl_2d_geo_index_node_sptr c4 = new bvgl_2d_geo_index_node<Type>(bbox4, parent);
  construct_sub_tree<Type>(c4, min_size);

  parent->children_.push_back(c1);
  parent->children_.push_back(c2);
  parent->children_.push_back(c3);
  parent->children_.push_back(c4);
}

template <class Type>
void construct_sub_tree_poly(bvgl_2d_geo_index_node_sptr parent, double const& min_size, vgl_polygon<double> const& poly)
{
  double w = parent->extent_.width();
  double h = parent->extent_.height();
  if (w <= min_size || h <= min_size)
    return;
  // create 4 children (the arrangement follows clock wise direction, started from lower_left)
  // lower left child
  vgl_point_2d<double> p1 = parent->extent_.min_point();
  vgl_point_2d<double> p2( p1.x()+w/2.0f, p1.y()+h/2.0f);
  vgl_box_2d<double> bbox1(p1, p2);
  if (vgl_intersection(bbox1, poly)) {
    bvgl_2d_geo_index_node_sptr c1 = new bvgl_2d_geo_index_node<Type>(bbox1, parent);
    construct_sub_tree_poly<Type>(c1, min_size, poly);
    parent->children_.push_back(c1);
  }
  // upper left child
  p1 = vgl_point_2d<double>(parent->extent_.min_point().x(), parent->extent_.min_point().y()+h/2.0f);
  p2 = vgl_point_2d<double>(parent->extent_.min_point().x()+w/2.0f, parent->extent_.max_point().y());
  vgl_box_2d<double> bbox2(p1, p2);
  if (vgl_intersection(bbox2, poly)) {
    bvgl_2d_geo_index_node_sptr c2 = new bvgl_2d_geo_index_node<Type>(bbox2, parent);
    construct_sub_tree_poly<Type>(c2, min_size, poly);
    parent->children_.push_back(c2);
  }
  // upper right child
  p1 = vgl_point_2d<double>(parent->extent_.min_point().x()+w/2, parent->extent_.min_point().y()+h/2);
  p2 = vgl_point_2d<double>(parent->extent_.max_point().x(), parent->extent_.max_point().y());
  vgl_box_2d<double> bbox3(p1, p2);
  if (vgl_intersection(bbox3, poly)) {
    bvgl_2d_geo_index_node_sptr c3 = new bvgl_2d_geo_index_node<Type>(bbox3, parent);
    construct_sub_tree_poly<Type>(c3, min_size, poly);
    parent->children_.push_back(c3);
  }
  // lower right child
  p1 = vgl_point_2d<double>(parent->extent_.min_point().x()+w/2, parent->extent_.min_point().y());
  p2 = vgl_point_2d<double>(parent->extent_.max_point().x(), parent->extent_.min_point().y()+h/2);
  vgl_box_2d<double> bbox4(p1, p2);
  if (vgl_intersection(bbox4, poly)) {
    bvgl_2d_geo_index_node_sptr c4 = new bvgl_2d_geo_index_node<Type>(bbox4, parent);
    construct_sub_tree_poly<Type>(c4, min_size, poly);
    parent->children_.push_back(c4);
  }
}

// construct tree from a bounding box
template <class Type>
bvgl_2d_geo_index_node_sptr bvgl_2d_geo_index::construct_tree(vgl_box_2d<double> const& bbox, double const& min_size)
{
  std::cout << "bounding box for bvgl_2d_geo_index root: " << bbox << std::endl;
  bvgl_2d_geo_index_node_sptr root = new bvgl_2d_geo_index_node<Type>(bbox);
  // recursively add children
  construct_sub_tree<Type>(root, min_size);
  return root;
}

template <class Type>
bvgl_2d_geo_index_node_sptr bvgl_2d_geo_index::construct_tree(vgl_box_2d<float> const& bbox, float const& min_size)
{
  std::cout << "bounding box for bvgl_2d_geo_index root: " << bbox << std::endl;
  vgl_box_2d<double> bbox_double;
  bvgl_2d_geo_index::convert_box(bbox, bbox_double);
  bvgl_2d_geo_index_node_sptr root = new bvgl_2d_geo_index_node<Type>(bbox_double);
  // recursively add children
  construct_sub_tree<Type>(root, min_size);
  return root;
}

// construct tree from a polygon
template <class Type>
bvgl_2d_geo_index_node_sptr bvgl_2d_geo_index::construct_tree(vgl_box_2d<double> const& bbox, double const& min_size, vgl_polygon<double> const& poly)
{
  bvgl_2d_geo_index_node_sptr root;
  if (vgl_intersection(bbox, poly)) {
    std::cout << "bounding box for bvgl_2d_geo_index root: " << bbox << std::endl;
    root = new bvgl_2d_geo_index_node<Type>(bbox);
    construct_sub_tree_poly<Type>(root, min_size, poly);
  }
  else
    std::cout << "bounding box " << bbox << " and polygon does not intersect, return an empty tree" << std::endl;
  return root;
}

template <class Type>
bvgl_2d_geo_index_node_sptr bvgl_2d_geo_index::construct_tree(vgl_box_2d<float> const& bbox, float const& min_size, vgl_polygon<float> const& poly)
{
  bvgl_2d_geo_index_node_sptr root;
  if (vgl_intersection(bbox, poly)) {
    std::cout << "bounding box for bvgl_2d_geo_index root: " << bbox << std::endl;
    vgl_box_2d<double> bbox_double;
    vgl_polygon<double> poly_double;
    double min_sz_double = (double)min_size;
    bvgl_2d_geo_index::convert_polygon(poly, poly_double);
    bvgl_2d_geo_index::convert_box(bbox,bbox_double);
    root = new bvgl_2d_geo_index_node<Type>(bbox_double);
    construct_sub_tree_poly<Type>(root, min_sz_double, poly_double);
  }
  else
    std::cout << "bounding box " << bbox << " and polygon does not intersect, return an empty tree" << std::endl;
  return root;
}

// construct a tree from text file
template <class Type>
bvgl_2d_geo_index_node_sptr read_and_construct_node(std::ifstream& ifs, bvgl_2d_geo_index_node_sptr parent)
{
  double x, y;
  ifs >> x;  ifs >> y;  vgl_point_2d<double> min_pt(x, y);
  ifs >> x;  ifs >> y;  vgl_point_2d<double> max_pt(x, y);
  vgl_box_2d<double> bbox(min_pt, max_pt);
  bvgl_2d_geo_index_node_sptr node = new bvgl_2d_geo_index_node<Type>(bbox, parent);
  unsigned nc;
  ifs >> nc;
  std::vector<unsigned> existence(nc);
  for (unsigned i = 0; i < nc; i++)
    ifs >> existence[i];
  for (unsigned i = 0; i < nc; i++) {
    bvgl_2d_geo_index_node_sptr child;
    if (existence[i])
      child = read_and_construct_node<Type>(ifs, node);
    node->children_.push_back(child);
  }
  return node;
}

template <class Type>
bvgl_2d_geo_index_node_sptr bvgl_2d_geo_index::read_and_construct(std::string const& file_name, double& min_size)
{
  std::ifstream ifs(file_name.c_str());
  ifs >> min_size;
  bvgl_2d_geo_index_node_sptr dummy_parent;
  bvgl_2d_geo_index_node_sptr root = read_and_construct_node<Type>(ifs, dummy_parent);
  return root;
}

#undef BVGL_2D_GEO_INDEX_INSTANTIATE
#define BVGL_2D_GEO_INDEX_INSTANTIATE(T) \
template bvgl_2d_geo_index_node_sptr bvgl_2d_geo_index::construct_tree<T>(vgl_box_2d<double> const& bbox, double const& min_size);\
template bvgl_2d_geo_index_node_sptr bvgl_2d_geo_index::construct_tree<T>(vgl_box_2d<float> const& bbox, float const& min_size);\
template bvgl_2d_geo_index_node_sptr bvgl_2d_geo_index::construct_tree<T>(vgl_box_2d<double> const& bbox, double const& min_size, vgl_polygon<double> const& poly);\
template bvgl_2d_geo_index_node_sptr bvgl_2d_geo_index::construct_tree<T>(vgl_box_2d<float> const& bbox, float const& min_size, vgl_polygon<float> const& poly);\
template bvgl_2d_geo_index_node_sptr bvgl_2d_geo_index::read_and_construct<T>(std::string const& file_name, double& min_size);

#endif // bvgl_2d_geo_index_hxx_
