#ifndef vil_distance_transform_h_
#define vil_distance_transform_h_
//:
//  \file
//  \brief Compute distance function
//  \author Tim Cootes

#include <vil/vil_image_view.h>

//: Compute distance function from zeros in original image
//  Image is assumed to be filled with max_dist where there
//  is background, and zero at the places of interest.
//  On exit, the values are the 8-connected distance to the
//  nearest original zero region.
// \relatesalso vil_image_view
void vil_distance_transform(vil_image_view<float>& image);

//: Compute directed distance function from zeros in original image
//  Image is assumed to be filled with max_dist where there
//  is background, and zero at the places of interest.
//  On exit, the values are the 8-connected distance to the
//  nearest original zero region above or to the left of current point.
//  One pass of distance transform, going from low to high i,j.
// \relatesalso vil_image_view
void vil_distance_transform_one_way(vil_image_view<float>& image);

//: Compute distance function from true elements in mask
//  On exit, the values are the 8-connected distance to the
//  nearest original zero region (or max_dist, if that is smaller).
// \relatesalso vil_image_view
void vil_distance_transform(const vil_image_view<bool>& mask,
                            vil_image_view<float>& distance_image,
                            float max_dist);

//: Compute distance function from zeros in original image
//  Image is assumed to be filled with max_dist where there
//  is background, and zero at the places of interest.
//  On exit, the values are the 24-connected distance to the
//  nearest original zero region. (ie considers neighbours in
//  a +/-2 pixel region around each point).
//  More accurate than vil_distance_transform(image), but
//  approximately twice the processing required.
// \relatesalso vil_image_view
void vil_distance_transform_r2(vil_image_view<float>& image);

#endif
