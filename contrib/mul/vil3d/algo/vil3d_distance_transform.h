#ifndef vil3d_distance_transform_h_
#define vil3d_distance_transform_h_

//:
//	\file
//	\brief Compute 3D distance transform
//	\author Kola Babalola

#include <vil3d/vil3d_image_view.h>

//:	Compute 3d distance transform from zeros in original image
//	Image is assumed to be filled with max_dist where there is
//	background, and zero at the places of interest. On exit,
//	the values are the 8-connected distance to the nearest
//	original zero voxel.
void vil3d_distance_transform(vil3d_image_view<float>& image);

//:	Compute 3d distance transform from zeros in original image
//	Image is assumed to be filled with max_dist where there is
//	background, and zero at the places of interest. On exit,
//	the values are the 8-connected distance to the nearest
//	original zero voxel.
//	One pass of distance transform, goins from low to high i,j,k.
void vil3d_distance_transform_one_way(vil3d_image_view<float>& image);

//:	Compute 3D distance function from true elements in mask
//	On exit, the values are the 26-connected distance to the
//	nearest original zero region (or max_dist, if that is smaller).
void vil_distance_transform(const vil3d_image_view<bool>& mask,
							vil3d_image_view<float>& distance_image,
							float max_dist);

//: Compute signed distance transform in 3d from zeros in 
//	original image. Image is assumed to be filled with max_dist 
//	where there is background, and zero in the places of interest.
//	On exit, the values are the signed 26-connected distance 
//	to the nearest original zero region. Positive values are
//	outside the bounded region and negative values are inside.
//	The values on the boundary are zero
void vil3d_signed_distance_transform(vil3d_image_view<float>& image);

//: Compute 3d signed distance transform from true elements in mask 
//	On exit, values are 26 connected distance from the 'true' boundary.
//	There are no zero values because the true boundary is the infinitismally
//	thin edge of the true and false regions of the mask. The values inside
//	the mask are negative and those outside are positive
void vil3d_signed_distance_transform(const vil3d_image_view<bool>& mask,
									 vil3d_image_view<float>& image,
									 float max_dist);

#endif