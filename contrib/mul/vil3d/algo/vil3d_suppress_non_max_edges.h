// This is contrib/mul/vil3d/algo/vil3d_suppress_non_max_edges.h
#ifndef vil3d_suppress_non_max_edges_h_
#define vil3d_suppress_non_max_edges_h_
//:
// \file
// \brief Given gradient image, compute magnitude and zero any non-maximal values
// \author Tim Cootes

#include <vil3d/vil3d_image_view.h>

//: Given gradient images, computes magnitude image containing maximal edges
//  Points with gradient magnitude above a threshold are tested against gradient
//  along normal to the edge and retained only if they are higher than
//  their neighbours.
//
//  Gradient images are assumed to be in units of (intensity change) per world unit.
//  (ie the output of vil3d_world_gradients)
//
//  Note: Currently assumes single plane only.
//  2 pixel border around output set to zero.
//  If two neighbouring edges have exactly the same strength, it retains
//  both (ie an edge is eliminated if it is strictly lower than a neighbour,
//  but not if it is the same as two neighbours).
//
// \relatesalso vil3d_image_view
template<class srcT>
void vil3d_suppress_non_max_edges(const vil3d_image_view<srcT>& world_grad,
                                const vil3d_image_view<srcT>& grad_mag,
                                double voxel_width_i,
                                double voxel_width_j,
                                double voxel_width_k,
                                srcT grad_mag_threshold,
                                vil3d_image_view<srcT>& max_grad_mag);

#endif // vil3d_suppress_non_max_edges_h_
