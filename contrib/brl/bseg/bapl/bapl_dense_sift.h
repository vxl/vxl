// This is bapl/bapl_dense_sift.h
#ifndef bapl_dense_sift_h_
#define bapl_dense_sift_h_
//:
// \file
// \brief Extract SIFT features from every pixel in an Image or a specific region.
// \author Brandon Mayer (b.mayer1@gmail.com)
// \date 12/10/2010
//
// \verbatim
//  Modifications
// \endverbatim

#include<bapl/bapl_keypoint_extractor.h>
#include<bapl/bapl_keypoint_sptr.h>
#include<bapl/bapl_lowe_pyramid_set.h>
#include<bapl/bapl_lowe_pyramid_set_sptr.h>

#include<vcl_cmath.h>
#include<vcl_map.h>
#include<vcl_vector.h>

#include<vil/vil_image_resource_sptr.h>

//Compute a sift descriptor at a given location. Can be given a sub-pixel location.
bool bapl_dense_sift(const vil_image_resource_sptr& image, 
						double const& i, double const& j, 
						bapl_keypoint_sptr& keypoint, unsigned const& octave_size = 3, unsigned const& num_octaves = 0);

//function to compute sift descriptors at each pixel. 
//Can specify an integer pixel spacing meaning, compute a descriptor every pixel_spacing pixels.
//bool bapl_dense_sift( const vil_image_resource_sptr & image, 
//					  vcl_vector<bapl_keypoint_sptr>& keypoints, 
//				      unsigned const& pixel_spacing = 1, unsigned const& octave_size = 3, unsigned const& num_octaves = 0);

#endif//bapl_dense_sift_h_