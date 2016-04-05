// This is brl/bbas/volm/volm_geo_index2.hxx
#ifndef volm_geo_index2_hxx_
#define volm_geo_index2_hxx_
//:
// \file
#include "volm_geo_index2.h"
#include "volm_io.h"
#include <bkml/bkml_write.h>
#include <vgl/vgl_intersection.h>
#include <vpgl/vpgl_utm.h>

//construct a tree such that the given tile is the root, and the hierarchy of its children form a quad-tree space partition
//the stopping criterion is when a child's bounding box size is less or equal to the min_size in arcseconds
template <class Type>
void construct_sub_tree(volm_geo_index2_node_sptr parent, double const& min_size)
{
  float w = parent->extent_.width();
  float h = parent->extent_.height();
  if (w <= min_size || h <= min_size)
    return;
  // create 4 children (the arrangement follows clock wise direction, started from lower_left)
  // lower left child
  vgl_point_2d<double> p1 = parent->extent_.min_point();
  vgl_point_2d<double> p2( p1.x()+w/2.0, p1.y()+h/2.0);
  vgl_box_2d<double> bbox1(p1,p2);
  volm_geo_index2_node_sptr c1 = new volm_geo_index2_node<Type>(bbox1, parent);
  construct_sub_tree<Type>(c1, min_size);
  // upper left child
  p1 = vgl_point_2d<double>(parent->extent_.min_point().x(), parent->extent_.min_point().y()+h/2.0);
  p2 = vgl_point_2d<double>(parent->extent_.min_point().x()+w/2.0, parent->extent_.max_point().y());
  vgl_box_2d<double> bbox2(p1,p2);
  volm_geo_index2_node_sptr c2 = new volm_geo_index2_node<Type>(bbox2, parent);
  construct_sub_tree<Type>(c2, min_size);
  // upper right child
  p1 = vgl_point_2d<double>(parent->extent_.min_point().x()+w/2.0, parent->extent_.min_point().y()+h/2.0);
  p2 = vgl_point_2d<double>(parent->extent_.max_point().x(), parent->extent_.max_point().y());
  vgl_box_2d<double> bbox3(p1, p2);
  volm_geo_index2_node_sptr c3 = new volm_geo_index2_node<Type>(bbox3, parent);
  construct_sub_tree<Type>(c3, min_size);
  // lower right child
  p1 = vgl_point_2d<double>(parent->extent_.min_point().x()+w/2.0, parent->extent_.min_point().y());
  p2 = vgl_point_2d<double>(parent->extent_.max_point().x(), parent->extent_.min_point().y()+h/2.0);
  vgl_box_2d<double> bbox4(p1, p2);
  volm_geo_index2_node_sptr c4 = new volm_geo_index2_node<Type>(bbox4, parent);
  construct_sub_tree<Type>(c4, min_size);

  parent->children_.push_back(c1);
  parent->children_.push_back(c2);
  parent->children_.push_back(c3);
  parent->children_.push_back(c4);
}

template <class Type>
void construct_sub_tree_poly(volm_geo_index2_node_sptr parent, double const& min_size, vgl_polygon<double> const& poly)
{
  float w = parent->extent_.width();
  float h = parent->extent_.height();
  if (w <= min_size || h <= min_size)
    return;
  // create 4 children (the arrangement follows clock wise direction, started from lower_left)
  // lower left child
  vgl_point_2d<double> p1 = parent->extent_.min_point();
  vgl_point_2d<double> p2( p1.x()+w/2.0f, p1.y()+h/2.0f);
  vgl_box_2d<double> bbox1(p1, p2);
  if (vgl_intersection(bbox1, poly)) {
    volm_geo_index2_node_sptr c1 = new volm_geo_index2_node<Type>(bbox1, parent);
    construct_sub_tree_poly<Type>(c1, min_size, poly);
    parent->children_.push_back(c1);
  }
  // upper left child
  p1 = vgl_point_2d<double>(parent->extent_.min_point().x(), parent->extent_.min_point().y()+h/2.0f);
  p2 = vgl_point_2d<double>(parent->extent_.min_point().x()+w/2.0f, parent->extent_.max_point().y());
  vgl_box_2d<double> bbox2(p1, p2);
  if (vgl_intersection(bbox2, poly)) {
    volm_geo_index2_node_sptr c2 = new volm_geo_index2_node<Type>(bbox2, parent);
    construct_sub_tree_poly<Type>(c2, min_size, poly);
    parent->children_.push_back(c2);
  }
  // upper right child
  p1 = vgl_point_2d<double>(parent->extent_.min_point().x()+w/2, parent->extent_.min_point().y()+h/2);
  p2 = vgl_point_2d<double>(parent->extent_.max_point().x(), parent->extent_.max_point().y());
  vgl_box_2d<double> bbox3(p1, p2);
  if (vgl_intersection(bbox3, poly)) {
    volm_geo_index2_node_sptr c3 = new volm_geo_index2_node<Type>(bbox3, parent);
    construct_sub_tree_poly<Type>(c3, min_size, poly);
    parent->children_.push_back(c3);
  }
  // lower right child
  p1 = vgl_point_2d<double>(parent->extent_.min_point().x()+w/2, parent->extent_.min_point().y());
  p2 = vgl_point_2d<double>(parent->extent_.max_point().x(), parent->extent_.min_point().y()+h/2);
  vgl_box_2d<double> bbox4(p1, p2);
  if (vgl_intersection(bbox4, poly)) {
    volm_geo_index2_node_sptr c4 = new volm_geo_index2_node<Type>(bbox4, parent);
    construct_sub_tree_poly<Type>(c4, min_size, poly);
    parent->children_.push_back(c4);
  }
}

template <class Type>
volm_geo_index2_node_sptr volm_geo_index2::construct_tree(vgl_box_2d<double> bbox, double const& min_size)
{
  std::cout << "bounding box for geo_index root: " << bbox << std::endl;
  volm_geo_index2_node_sptr root = new volm_geo_index2_node<Type>(bbox);
  // recursively add children
  construct_sub_tree<Type>(root, min_size);
  return root;
}

template <class Type>
volm_geo_index2_node_sptr volm_geo_index2::construct_tree(volm_tile t, double const& min_size)
{
  // create the bbox for the root
  vgl_box_2d<double> bbox = t.bbox_double();
  std::cout << "bounding box for root: " << bbox << std::endl;
  return volm_geo_index2::construct_tree<Type>(bbox, min_size);
}

template <class Type>
volm_geo_index2_node_sptr volm_geo_index2::construct_tree(vgl_box_2d<double> bbox, double const& min_size, vgl_polygon<double> const& poly)
{
  std::cout << "bounding box for root: " << bbox << std::endl;
  volm_geo_index2_node_sptr root;
  if (vgl_intersection(bbox, poly)) {
    root = new volm_geo_index2_node<Type>(bbox);
    // recursively add children
    construct_sub_tree_poly<Type>(root, min_size, poly);
  }
  return root;
}

template <class Type>
volm_geo_index2_node_sptr volm_geo_index2::construct_tree(vgl_box_2d<double> bbox, double const& min_size, vgl_polygon<float> const& poly)
{
  vgl_polygon<double> poly_double;
  volm_io::convert_polygons(poly, poly_double);
  return volm_geo_index2::construct_tree<Type>(bbox, min_size, poly_double);
}

template <class Type>
volm_geo_index2_node_sptr volm_geo_index2::construct_tree(volm_tile t, double const& min_size, vgl_polygon<float> const& poly)
{
  vgl_box_2d<double> bbox = t.bbox_double();
  return volm_geo_index2::construct_tree<Type>(bbox, min_size, poly);
}

template <class Type>
volm_geo_index2_node_sptr volm_geo_index2::construct_tree(volm_tile t, double const& min_size, vgl_polygon<double> const& poly)
{
  vgl_box_2d<double> bbox = t.bbox_double();
  return volm_geo_index2::construct_tree<Type>(bbox, min_size, poly);
}

template <class Type>
volm_geo_index2_node_sptr read_and_construct_node(std::ifstream& ifs, volm_geo_index2_node_sptr parent)
{
  double x, y;
  ifs >> x;  ifs >> y;  vgl_point_2d<double> min_pt(x, y);
  ifs >> x;  ifs >> y;  vgl_point_2d<double> max_pt(x, y);
  vgl_box_2d<double> bbox(min_pt, max_pt);
  volm_geo_index2_node_sptr node = new volm_geo_index2_node<Type>(bbox, parent);
  unsigned nc;
  ifs >> nc;
  std::vector<unsigned> existence(nc);
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
volm_geo_index2_node_sptr volm_geo_index2::read_and_construct(std::string const& file_name, double& min_size)
{
  std::ifstream ifs(file_name.c_str());
  ifs >> min_size;
  volm_geo_index2_node_sptr dummy_parent;
  volm_geo_index2_node_sptr root = read_and_construct_node<Type>(ifs, dummy_parent);
  return root;
}

#undef VOLM_GEO_INDEX2_INSTANTIATE
#define VOLM_GEO_INDEX2_INSTANTIATE(T) \
template volm_geo_index2_node_sptr volm_geo_index2::construct_tree<T>(volm_tile t, const double& min_size);\
template volm_geo_index2_node_sptr volm_geo_index2::construct_tree<T>(vgl_box_2d<double> bbox, double const& min_size);\
template volm_geo_index2_node_sptr volm_geo_index2::construct_tree<T>(vgl_box_2d<double> bbox, double const& min_size, vgl_polygon<double> const& poly);\
template volm_geo_index2_node_sptr volm_geo_index2::construct_tree<T>(vgl_box_2d<double> bbox, double const& min_size, vgl_polygon<float> const& poly);\
template volm_geo_index2_node_sptr volm_geo_index2::construct_tree<T>(volm_tile t, const double& min_size, vgl_polygon<double> const& poly);\
template volm_geo_index2_node_sptr volm_geo_index2::construct_tree<T>(volm_tile t, const double& min_size, vgl_polygon<float> const& poly);\
template volm_geo_index2_node_sptr volm_geo_index2::read_and_construct<T>(std::string const& flie_name, double& min_size)


#endif // volm_geo_index2_hxx_
