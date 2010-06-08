// This is contrib/mul/vil3d/algo/vil3d_world_gradients.h
#ifndef vil3d_world_gradients_h_
#define vil3d_world_gradients_h_
//:
// \file
// \brief Given image gradients compute world gradients and gradient magnitude
// \author Tim Cootes

#include <vil3d/vil3d_image_view.h>

//: Given image gradients compute world gradients and gradient magnitude
//  Input gradient images are assumed to be un-normalised pixel gradients
//  (ie no scaling has been done to take account of world pixel widths).
//  Divides each by corresponding pixel dimension to give gradient in world units
//  (ie intensity change per unit world length) in world_grad (3 plane image)
//  The gradient magnitude output is in units of intensity change per world length
//  (ie it does take account of voxel sizes).
//
//  Note: Currently assumes single plane only.
//  1 pixel border around output set to zero.
//
// \relatesalso vil3d_image_view
template<class srcT, class destT>
void vil3d_world_gradients(const vil3d_image_view<srcT>& grad_i,
                                const vil3d_image_view<srcT>& grad_j,
                                const vil3d_image_view<srcT>& grad_k,
                                double voxel_width_i,
                                double voxel_width_j,
                                double voxel_width_k,
                                vil3d_image_view<destT>& world_grad,
                                vil3d_image_view<destT>& grad_mag);
#endif // vil3d_world_gradients_h_
