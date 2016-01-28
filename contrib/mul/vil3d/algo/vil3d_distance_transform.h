#ifndef vil3d_distance_transform_h_
#define vil3d_distance_transform_h_
// :
// \file
// \brief Compute 3D distance transform
// \author Kola Babalola

#include <vil3d/vil3d_image_view.h>
#include <vil/vil_rgb.h>

// : Compute 3d distance transform from zeros in original image.
//  Image is assumed to be filled with max_dist where there is
//  background, and zero at the places of interest. On exit,
//  the values are the 8-connected distance to the nearest
//  original zero voxel.
// \param distance_link_i use when calculating connected distance between i and i+1
// \param distance_link_j use when calculating connected distance between j and j+1
// \param distance_link_k use when calculating connected distance between k and k+1
void vil3d_distance_transform(vil3d_image_view<float>& image, const float distance_link_i = 1,
                              const float distance_link_j = 1, const float distance_link_k = 1);

// : Compute 3d distance transform from zeros in original image.
//  Image is assumed to be filled with max_dist where there is
//  background, and zero at the places of interest. On exit,
//  the values are the 8-connected distance to the nearest
//  original zero voxel.
//  One pass of distance transform, going from low to high i,j,k.
// \param distance_link_i use when calculating connected distance between i and i+1
// \param distance_link_j use when calculating connected distance between j and j+1
// \param distance_link_k use when calculating connected distance between k and k+1
void vil3d_distance_transform_one_way(vil3d_image_view<float>& image, const float distance_link_i = 1,
                                      const float distance_link_j = 1, const float distance_link_k = 1);

// : Compute 3D distance function from true elements in mask.
//  On exit, the values are the 26-connected distance to the
//  nearest original zero region (or max_dist, if that is smaller).
void vil_distance_transform(const vil3d_image_view<bool>& mask, vil3d_image_view<float>& distance_image,
                            float max_dist);

// : Compute signed distance transform in 3d from zeros in original image.
//  Image is assumed to be filled with max_dist
//  where there is background, and zero in the places of interest.
//  On exit, the values are the signed 26-connected distance
//  to the nearest original zero region. Positive values are
//  outside the bounded region and negative values are inside.
//  The values on the boundary are zero
// \param distance_link_i use when calculating connected distance between i and i+1
// \param distance_link_j use when calculating connected distance between j and j+1
// \param distance_link_k use when calculating connected distance between k and k+1
void vil3d_signed_distance_transform(vil3d_image_view<float>& image, const float distance_link_i = 1,
                                     const float distance_link_j = 1, const float distance_link_k = 1);

float vil3d_min_equal(float const& a, float const& b, bool& eq);

void vil3d_distance_transform_one_way_with_dir(vil3d_image_view<float>& image,
                                               vil3d_image_view<vil_rgb<float> >& orient,
                                               const float distance_link_i = 1, const float distance_link_j = 1,
                                               const float distance_link_k = 1);

void vil3d_distance_transform_with_dir(vil3d_image_view<float>& image, vil3d_image_view<vil_rgb<float> >& orient,
                                       const float distance_link_i, const float distance_link_j,
                                       const float distance_link_k);

// : Compute 3d signed distance transform from true elements in mask.
//  On exit, values are 26 connected distance from the 'true' boundary.
//  There are no zero values because the true boundary is the infinitesimally
//  thin edge of the true and false regions of the mask. The values inside
//  the mask are negative and those outside are positive
// \param distance_link_i use when calculating connected distance between i and i+1
// \param distance_link_j use when calculating connected distance between j and j+1
// \param distance_link_k use when calculating connected distance between k and k+1
void vil3d_signed_distance_transform(const vil3d_image_view<bool>& mask, vil3d_image_view<float>& image, float max_dist,
                                     const float distance_link_i = 1, const float distance_link_j = 1,
                                     const float distance_link_k = 1);

#endif // vil3d_distance_transform_h_
