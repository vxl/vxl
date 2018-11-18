// This is brl/bbas/imesh/algo/imesh_kd_tree.cxx
#include <iostream>
#include <algorithm>
#include <limits>
#include <cmath>
#include <utility>
#include "imesh_kd_tree.h"
#include "imesh_kd_tree.hxx"
//:
// \file

#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_point_3d.h>
#include <imesh/algo/imesh_intersect.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


//: The minimum square distance between \a p and any point in \a b
double imesh_min_sq_dist(const vgl_point_3d<double>& p,
                         const vgl_box_3d<double>& b)
{
  double sum_sq = 0;

  if (p.x() < b.min_x()) {
    double dist = b.min_x() - p.x();
    sum_sq += dist * dist;
  }
  else if (p.x() > b.max_x()) {
    double dist = b.max_x() - p.x();
    sum_sq += dist*dist;
  }

  if (p.y() < b.min_y()) {
    double dist = b.min_y() - p.y();
    sum_sq += dist * dist;
  }
  else if (p.y() > b.max_y()) {
    double dist = b.max_y() - p.y();
    sum_sq += dist*dist;
  }

  if (p.z() < b.min_z()) {
    double dist = b.min_z() - p.z();
    sum_sq += dist * dist;
  }
  else if (p.z() > b.max_z()) {
    double dist = b.max_z() - p.z();
    sum_sq += dist*dist;
  }

  return sum_sq;
}


//: The maximum square distance between \a p and any point in \a b
double imesh_max_sq_dist(const vgl_point_3d<double>& p,
                         const vgl_box_3d<double>& b)
{
  vgl_point_3d<double> mp( (p.x() > b.centroid_x()) ? b.min_x() : b.max_x(),
                           (p.y() > b.centroid_y()) ? b.min_y() : b.max_y(),
                           (p.z() > b.centroid_z()) ? b.min_z() : b.max_z() );
  return (mp - p).sqr_length();
}


namespace {

//: Functor for sorting boxes by increasing X centroid
class less_box_centroid_x
{
  std::vector<vgl_box_3d<double> > boxes;
 public:
  less_box_centroid_x(std::vector<vgl_box_3d<double> >  b) : boxes(std::move(b)) {}
  bool operator () (unsigned int i1, unsigned int i2) const
  {
    return boxes[i1].centroid_x() < boxes[i2].centroid_x();
  }
};

//: Functor for sorting boxes by increasing Y centroid
class less_box_centroid_y
{
  std::vector<vgl_box_3d<double> > boxes;
 public:
  less_box_centroid_y(std::vector<vgl_box_3d<double> >  b) : boxes(std::move(b)) {}
  bool operator () (unsigned int i1, unsigned int i2) const
  {
    return boxes[i1].centroid_y() < boxes[i2].centroid_y();
  }
};

//: Functor for sorting boxes by increasing Z centroid
class less_box_centroid_z
{
  std::vector<vgl_box_3d<double> > boxes;
 public:
  less_box_centroid_z(std::vector<vgl_box_3d<double> >  b) : boxes(std::move(b)) {}
  bool operator () (unsigned int i1, unsigned int i2) const
  {
    return boxes[i1].centroid_z() < boxes[i2].centroid_z();
  }
};

bool
best_split(const std::vector<vgl_box_3d<double> >& boxes,
           const std::vector<unsigned int>& indices,
           double& min_volume, unsigned int& split_ind)
{
  const unsigned int ind_size = indices.size();
  std::vector<double> volume_seq;
  vgl_box_3d<double> box(boxes[indices[0]]);
  for (unsigned int i=1; i<ind_size; ++i)
  {
    volume_seq.push_back(box.volume());
    box.add(boxes[indices[i]]);
  }
  box = boxes[indices[ind_size-1]];
  bool found_min = false;
  double best_ratio_factor = 1.0;
  for (unsigned int i=ind_size-1; i>0;)
  {
    double volume = box.volume();
    box.add(boxes[indices[--i]]);
    volume += volume_seq[i];
    if (volume <= min_volume) {
      double ratio_factor =  std::abs(0.5 - double(i+1)/ind_size);
      if ( ratio_factor < 0.25 && ratio_factor < best_ratio_factor) {
        min_volume = volume;
        split_ind = i+1;
        found_min = true;
        best_ratio_factor = ratio_factor;
      }
    }
  }
  return found_min;
}
// end of namespace
};


//: recursively construct a kd-tree of mesh triangles
std::unique_ptr<imesh_kd_tree_node>
imesh_build_kd_tree_rec(const std::vector<vgl_box_3d<double> >& boxes,
                        const vgl_box_3d<double>& outer_box,
                        const vgl_box_3d<double>& inner_box,
                        const std::vector< unsigned int >& indices)
{
  // If only one triangle is left, create and return a leaf node.
  if (indices.size() == 1) {
    return std::unique_ptr<imesh_kd_tree_node>(new imesh_kd_tree_node( outer_box,
                                                                    inner_box,
                                                                    indices[0] ));
  }

  // find best direction for splitting
  std::vector<std::vector<unsigned int> > sorted(3,indices);
  std::sort(sorted[0].begin(),sorted[0].end(), less_box_centroid_x(boxes));
  std::sort(sorted[1].begin(),sorted[1].end(), less_box_centroid_y(boxes));
  std::sort(sorted[2].begin(),sorted[2].end(), less_box_centroid_z(boxes));
  double min_volume = std::numeric_limits<double>::infinity();
  unsigned int split_ind=0;
  int dim = -1;
  if (best_split(boxes,sorted[0],min_volume,split_ind))
    dim = 0;
  if (best_split(boxes,sorted[1],min_volume,split_ind))
    dim = 1;
  if (best_split(boxes,sorted[2],min_volume,split_ind))
    dim = 2;

  // split the sorted indices
  std::vector<unsigned int> left_indices(sorted[dim].begin(),
                                        sorted[dim].begin()+split_ind);
  std::vector<unsigned int> right_indices(sorted[dim].begin()+split_ind,
                                         sorted[dim].end());

  // compute the left and right inner and outer boxes
  vgl_box_3d<double> left_inner_box, right_inner_box;
  typedef std::vector<unsigned int>::const_iterator index_iterator;
  for ( index_iterator itr=left_indices.begin(); itr!=left_indices.end(); ++itr )
    left_inner_box.add(boxes[*itr]);
  for ( index_iterator itr=right_indices.begin(); itr!=right_indices.end(); ++itr )
    right_inner_box.add(boxes[*itr]);
  vgl_box_3d<double> left_outer_box(outer_box), right_outer_box(outer_box);
  switch (dim)
  {
    case 0:
      left_outer_box.set_max_x(left_inner_box.max_x());
      right_outer_box.set_min_x(right_inner_box.min_x());
      if (left_outer_box.max_x() < right_outer_box.min_x()) {
        double mean = (left_outer_box.max_x() + right_outer_box.min_x())/2.0;
        left_outer_box.set_max_x(mean);
        right_outer_box.set_min_x(mean);
      }
      break;
    case 1:
      left_outer_box.set_max_y(left_inner_box.max_y());
      right_outer_box.set_min_y(right_inner_box.min_y());
      if (left_outer_box.max_y() < right_outer_box.min_y()) {
        double mean = (left_outer_box.max_y() + right_outer_box.min_y())/2.0;
        left_outer_box.set_max_y(mean);
        right_outer_box.set_min_y(mean);
      }
      break;
    case 2:
      left_outer_box.set_max_z(left_inner_box.max_z());
      right_outer_box.set_min_z(right_inner_box.min_z());
      if (left_outer_box.max_z() < right_outer_box.min_z()) {
        double mean = (left_outer_box.max_z() + right_outer_box.min_z())/2.0;
        left_outer_box.set_max_z(mean);
        right_outer_box.set_min_z(mean);
      }
      break;
    default:
      break;
  }

  // recursively construct child nodes
  std::unique_ptr<imesh_kd_tree_node>
      left_child(imesh_build_kd_tree_rec(boxes,left_outer_box,left_inner_box,left_indices));
  std::unique_ptr<imesh_kd_tree_node>
      right_child(imesh_build_kd_tree_rec(boxes,right_outer_box,right_inner_box,right_indices));

  return std::unique_ptr<imesh_kd_tree_node>(new imesh_kd_tree_node(outer_box,
                                                                 inner_box,
                                                                 std::move(left_child),
                                                                 std::move(right_child) ));
}


//: Construct a kd-tree (in 3d) of axis aligned boxes
std::unique_ptr<imesh_kd_tree_node>
imesh_build_kd_tree(const std::vector<vgl_box_3d<double> >& boxes)
{
  // make the outer box
  double minv = -std::numeric_limits<double>::infinity();
  double maxv = std::numeric_limits<double>::infinity();
  vgl_box_3d<double> outer_box(minv,minv,minv, maxv,maxv,maxv);

  // create the vector of ids
  std::vector< unsigned int > indices( boxes.size() );
  for ( unsigned int i=0; i<indices.size(); ++i)
    indices[i] = i;

  // make the inner box
  vgl_box_3d<double> inner_box;
  for (const auto & boxe : boxes) {
    inner_box.add(boxe);
  }

  // call recursive function to do the real work
  std::unique_ptr<imesh_kd_tree_node> root(imesh_build_kd_tree_rec(boxes, outer_box, inner_box, indices));

  // assign additional indices to internal nodes
  // reverse breadth first (root gets last index)
  std::vector<imesh_kd_tree_node*> internal_nodes;
  internal_nodes.push_back(root.get());
  for (unsigned int c=0; c<internal_nodes.size(); ++c) {
    if (!internal_nodes[c]->left_->is_leaf())
      internal_nodes.push_back(internal_nodes[c]->left_.get());
    if (!internal_nodes[c]->right_->is_leaf())
      internal_nodes.push_back(internal_nodes[c]->right_.get());
  }
  typedef std::vector<imesh_kd_tree_node*>::const_reverse_iterator critr;
  unsigned int index = boxes.size();
  for (critr itr = internal_nodes.rbegin(); itr != critr(internal_nodes.rend()); ++itr)
    (*itr)->index_ = index++;

  return root;
}


//: construct a kd-tree of mesh faces
std::unique_ptr<imesh_kd_tree_node>
imesh_build_kd_tree(const imesh_vertex_array<3>& verts,
                    const imesh_face_array_base& faces)
{
  // build face bounding boxes
  std::vector<vgl_box_3d<double> > boxes(faces.size());
  for ( unsigned int i=0; i<boxes.size(); ++i ) {
    for ( unsigned int j=0; j<faces.num_verts(i); ++j) {
      boxes[i].add(verts[faces(i,j)]);
    }
  }

  // construct a kd-tree using the boxes
  return imesh_build_kd_tree(boxes);
}


namespace {
class tri_dist_func
{
 public:
  tri_dist_func(const imesh_vertex_array<3>& v,
                const imesh_regular_face_array<3>& t)
  : verts(v), tris(t), closest_index(static_cast<unsigned int>(-1)),
    closest_dist(std::numeric_limits<double>::infinity()),
    closest_u(0), closest_v(0) {}
  const imesh_vertex_array<3>& verts;
  const imesh_regular_face_array<3>& tris;
  unsigned int closest_index;
  double closest_dist, closest_u, closest_v;

  vgl_point_3d<double> closest_point() const
  {
    const imesh_regular_face<3>& tri = tris[closest_index];
    const double& u = closest_u;
    const double& v = closest_v;
    double t = 1.0-u-v;
    const imesh_vertex<3>& p0 = verts[tri[0]];
    const imesh_vertex<3>& p1 = verts[tri[1]];
    const imesh_vertex<3>& p2 = verts[tri[2]];
    return {t*p0[0] + u*p1[0] + v*p2[0],
                                t*p0[1] + u*p1[1] + v*p2[1],
                                t*p0[2] + u*p1[2] + v*p2[2]};
  }

  double operator () (const vgl_point_3d<double>& pt, unsigned int i)
  {
    const imesh_regular_face<3>& tri = tris[i];
    double dist,u,v;
    /* unsigned char s = */
        imesh_triangle_closest_point(pt, verts[tri[0]],
                                     verts[tri[1]], verts[tri[2]],
                                     dist, u, v);
    if (dist < closest_dist) {
      closest_dist = dist;
      closest_index = i;
      closest_u = u;
      closest_v = v;
    }
    return dist;
  }
};
// end of namespace
}

//: compute the closest point on the mesh using the kd-tree
//  \returns the index of the closest triangle
unsigned int
imesh_kd_tree_closest_point(const vgl_point_3d<double>& query,
                            const imesh_mesh& mesh,
                            const std::unique_ptr<imesh_kd_tree_node>& root,
                            vgl_point_3d<double>& cp,
                            std::vector<imesh_kd_tree_queue_entry>* dists)
{
  const imesh_vertex_array<3>& verts = mesh.vertices<3>();
  const auto& faces = static_cast<const imesh_regular_face_array<3>&>(mesh.faces());
  tri_dist_func dist(verts,faces);
  unsigned int ind = imesh_closest_index<tri_dist_func&>(query,root,dist,dists);
  cp = dist.closest_point();
  return ind;
}


//: Traverse the tree looking for leaf nodes that contain the query
//  Returns a vector of leaf nodes paired with min square distance to the inner box
//  Also returns a vector of the unexplored internal node-distance^2 pairs
//  Resulting vectors are unsorted
void imesh_kd_tree_traverse(const vgl_point_3d<double>& query,
                            const std::unique_ptr<imesh_kd_tree_node>& root,
                            std::vector<imesh_kd_tree_queue_entry>& leaf,
                            std::vector<imesh_kd_tree_queue_entry>& internal_node)
{
  // find the root leaves containing the query point
  std::vector<imesh_kd_tree_node*> to_examine;
  to_examine.push_back(root.get());
  internal_node.clear();
  leaf.clear();
  for (unsigned int i=0; i<to_examine.size(); ++i) {
    imesh_kd_tree_node* current = to_examine[i];
    if (current->is_leaf()) {
      leaf.emplace_back(
                     imesh_min_sq_dist(query,current->inner_box_),
                     current );
      continue;
    }

    if (imesh_min_sq_dist(query,current->left_->outer_box_) <=0)
      to_examine.push_back(current->left_.get());
    else
      internal_node.emplace_back(
                              imesh_min_sq_dist(query,current->left_->inner_box_),
                              current->left_.get());

    if (imesh_min_sq_dist(query,current->right_->outer_box_) <=0)
      to_examine.push_back(current->right_.get());
    else
      internal_node.emplace_back(
                              imesh_min_sq_dist(query,current->right_->inner_box_),
                              current->right_.get());
  }
}
