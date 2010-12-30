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

#include<vbl/vbl_ref_count.h>

#include<vcl_cmath.h>
#include<vcl_map.h>
#include<vcl_vector.h>

#include<vgl/vgl_point_2d.h>

#include<vil/vil_image_resource_sptr.h>

//Compute a sift descriptor at a given location. Can be given a sub-pixel location.
//bool bapl_dense_sift(const vil_image_resource_sptr& image, 
//						double const& i, double const& j, 
//						bapl_keypoint_sptr& keypoint, unsigned const& octave_size = 3, unsigned const& num_octaves = 0);

//function to compute sift descriptors at each pixel. 
//Can specify an integer pixel spacing meaning, compute a descriptor every pixel_spacing pixels.
//bool bapl_dense_sift( const vil_image_resource_sptr & image, 
//					  vcl_vector<bapl_keypoint_sptr>& keypoints, 
//				      unsigned const& pixel_spacing = 1, unsigned const& octave_size = 3, unsigned const& num_octaves = 0);

class bapl_dense_sift:public vbl_ref_count
{
public:
	bapl_dense_sift():pyramid_valid_(false){}

	bapl_dense_sift( const vil_image_resource_sptr& image, unsigned octave_size = 6, unsigned num_octaves = 1 );

	~bapl_dense_sift(){}

	void create_pyramid(const vil_image_resource_sptr& image, unsigned octave_size = 6, unsigned num_octaves = 1);

	bapl_lowe_keypoint_sptr make_keypoint( double const& i, double const& j);

	vcl_vector<bapl_lowe_keypoint_sptr> make_keypoint( unsigned const istep = 1, unsigned const jstep = 1 );

	vcl_vector<bapl_lowe_keypoint_sptr> make_keypoint( vcl_vector<vgl_point_2d<unsigned> > const& pts );

	bapl_lowe_pyramid_set_sptr pyramid_sptr(){return this->pyramid_sptr;}

private:
	bapl_lowe_pyramid_set_sptr pyramid_sptr_;
	unsigned octave_size_;
	unsigned num_octaves_;
	bool pyramid_valid_;

};

#endif//bapl_dense_sift_h_