// This is mul/fhs/fhs_searcher.h
#ifndef fhs_searcher_h_
#define fhs_searcher_h_
//:
// \file
// \author Tim Cootes
// \brief Use F&H's DP style algorithm to search for global solutions

#include <fhs/fhs_arc.h>
#include <vimt/vimt_image_2d_of.h>

//: Use F&H's DP style algorithm to search for global solutions to model match
//  Model consists of a set of features, together with a tree of neighbour
//  relationships of the form pos(j) = pos(i) + (N(mx,var_x),N(my,var_y))
//  where N(m,var) is a gaussian with mean m and variance var.
//
//  The aim is to find a set of points {p(i)} which minimise
//  sum_i F_i(p(i)) + sum_k shape_cost(arc(k))
//  where k indexes the set of arcs defining neighbour relationships,
//  and shape_cost(arc) = dx*dx/arc.var_x + dy*dy.var_y  (dx=p(arc_j).x()-p(arc_i).x()-arc.mx)
//  This is achieved using a combination of a quadratic distance function
//  applied to each feature response image F(i), and a dynamic programming
//  approach to combining the data.
class fhs_searcher
{
private:
  //: Arcs defining neighbour relationships between features
  //  Ordered so that parents precede children
  vcl_vector<fhs_arc> arc_;

  //: arc_to_j_[j] gives index of arc ending at given j
  vcl_vector<unsigned> arc_to_j_;

  //: children_[i] gives list of child nodes of node i in tree
  vcl_vector<vcl_vector<unsigned> > children_;

  //: Workspace for accumulated sum of responses
  vcl_vector<vimt_image_2d_of<float> > sum_im_;

  //: Workspace for sum of responses, transformed by distance function
  vcl_vector<vimt_image_2d_of<float> > dist_im_;

  //: pos_[i](x,y,0),pos_[i](x,y,1) is position of best response for (x,y)
  //  Result is in image co-ordinates.
  vcl_vector<vimt_image_2d_of<int> > pos_im_;

  //: Combine responses for image im_index, given supplied feature_response for that node
  void combine_responses(unsigned im_index,
                         const vimt_image_2d_of<float>& feature_response);

public:
  //: Default constructor
  fhs_searcher();

  //: Set tree defining relationships between features
  //  Input arcs define neighbour relationships in any order.
  //  root_node defines which feature to be used as the root
  void set_tree(const vcl_vector<fhs_arc>& arcs, unsigned root_node);

  //: Index of root node (set by last call to set_tree()
  unsigned root_node() const;

  //: Number of points represented
  unsigned n_points() const { return arc_.size()+1; }

  //: Perform global search
  //  Images of feature response supplied.  The transformation
  //  (world2im()) for each image can be used to indicate regions
  //  which don't necessarily overlap.  However, effective displacements
  //  are assumed to be in pixel sized steps.
  //
  //  After calling search(), results can be obtained using
  //  points() and best_points() etc
  void search(const vcl_vector<vimt_image_2d_of<float> >& feature_response);

  //: Compute optimal position of all points given position of root
  //  Assumes search() has been called first
  void points_from_root(const vgl_point_2d<double>& root_pt,
                        vcl_vector<vgl_point_2d<double> >& pts) const;

  //: Compute optimal position of all points
  //  Assumes search() has been called first
  //  Returns cost at optimal position
  double best_points(vcl_vector<vgl_point_2d<double> >& pts) const;
};

#endif // fhs_searcher_h_
