// This is core/vil/algo/vil_suppress_non_max_edges.h
#ifndef vil_suppress_non_max_edges_h_
#define vil_suppress_non_max_edges_h_
//:
// \file
// \brief Given gradient image, compute magnitude and zero any non-maximal values
// \author Tim Cootes
//
// \verbatim
//  Modifications
//   Matt Leotta  -  Aug 22, 2008 - Added a parabolic subpixel version
// \endverbatim

#include <vil/vil_image_view.h>

//: Given gradient images, computes magnitude image containing maximal edges
//  Computes magnitude image.  Zeros any below a threshold.
//  Points with magnitude above a threshold are tested against gradient
//  along normal to the edge and retained only if they are higher than
//  their neighbours.
//
//  Note: Currently assumes single plane only.
//  2 pixel border around output set to zero.
//  If two neighbouring edges have exactly the same strength, it retains
//  both (ie an edge is eliminated if it is strictly lower than a neighbour,
//  but not if it is the same as two neighbours).
//
// \relatesalso vil_image_view
template<class srcT, class destT>
void vil_suppress_non_max_edges(const vil_image_view<srcT>& grad_i,
                                const vil_image_view<srcT>& grad_j,
                                double grad_mag_threshold,
                                vil_image_view<destT>& grad_mag);


//: Given gradient images, computes a subpixel edgemap with magnitudes and orientations
//  Computes magnitude image.  Zeros any below a threshold.
//  Points with magnitude above a threshold are tested against gradient
//  along normal to the edge and retained only if they are higher than
//  their neighbours.  The magnitude of retained points is revised using
//  parabolic interpolation in the normal direction.  The same interpolation
//  provides a subpixel offset from the integral pixel location.
//
//  This algorithm returns a 3-plane image where the planes are:
//  - 0 - The interpolated peak magnitude
//  - 1 - The orientation (in radians)
//  - 2 - The offset to the subpixel peak in the gradient direction
//  All non-maximal edge pixel have the value zero in all three planes.
//  \sa vil_orientations_at_edges
//
//  The subpixel location for pixel (i,j) is computed as
//  \code
//    double theta = grad_mag_orient_offset(i,j,1);
//    double offset = grad_mag_orient_offset(i,j,2);
//    double x = i + std::cos(theta)*offset;
//    double y = j + std::sin(theta)*offset;
//  \endcode
//
//  Note: Currently assumes single plane only.
//  2 pixel border around output set to zero.
//  If two neighbouring edges have exactly the same strength, it retains
//  both (ie an edge is eliminated if it is strictly lower than a neighbour,
//  but not if it is the same as two neighbours).
//
// \relatesalso vil_image_view
template<class srcT, class destT>
void vil_suppress_non_max_edges_subpixel(const vil_image_view<srcT>& grad_i,
                                         const vil_image_view<srcT>& grad_j,
                                         double grad_mag_threshold,
                                         vil_image_view<destT>& grad_mag_orient_offset);

#endif // vil_suppress_non_max_edges_h_
