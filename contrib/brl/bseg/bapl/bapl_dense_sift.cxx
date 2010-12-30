#include "bapl/bapl_dense_sift.h"

//bool bapl_dense_sift( const vil_image_resource_sptr& image,
//					  vcl_vector<bapl_keypoint_sptr>& keypoints,
//					  unsigned const& pixel_spacing, unsigned const& octave_size, unsigned const& num_octaves)
//{
//	unsigned n_pix = image->ni()*image->nj();
//	unsigned nrows = image->ni();
//	for(unsigned linear_index = 0; linear_index < n_pix; linear_index+=pixel_spacing)
//	{
//		unsigned col = linear_index/nrows;
//		unsigned row = linear_index%nrows;
//		bapl_keypoint_sptr keypoint;
//		bapl_dense_sift(image,col,row,keypoint,octave_size,num_octaves);
//		keypoints.push_back(keypoint);
//	}
//	return true;
//}//end bapl_dense_sift

//bool bapl_dense_sift(const vil_image_resource_sptr& image,
//						double const& i, double const& j, 
//						bapl_keypoint_sptr& keypoint, unsigned const& octave_size, unsigned const& num_octaves)
//{
//	static unsigned keypoint_id = 0;
//
//	//construct image pyramids
//	bapl_lowe_pyramid_set_sptr pyramid_set = new bapl_lowe_pyramid_set(image, octave_size, num_octaves);
//
//	//find maximal scale for the given pixel location
//
//	vcl_map<float,float> scale_index_map;
//
//	//const vil_image_view<float>& base_dog = pyramid_set->dog_pyramid(0,0); //the base case;
//	//float current_scale = 1;
//	//float dog_value = base_dog(i,j);
//	//scale_index_map[dog_value] = current_scale;
//
//
//	//debugging
//	float max_dog_value= -100000;
//	int max_scale_index = 0;
//
//	for( int scale_index = 0; scale_index < (octave_size*num_octaves)-1; ++scale_index )
//	{
//		const vil_image_view<float>& current_dog = pyramid_set->dog_pyramid(scale_index/octave_size, scale_index%octave_size);
//
//		float current_octave = scale_index/octave_size;
//		float current_relative_scale = scale_index%octave_size;
//
//		float current_scale = (float)vcl_pow(2.0f,(float(scale_index)/octave_size)-1); //old formula
//		//float current_scale = vcl_pow(2.0f,current_octave+(current_relative_scale/octave_size));
//		//current_scale = vcl_pow(2.0f, (current_octave + current_relative_scale));
//
//		
//
//		//debugging
//		double log2_scale = vcl_log(current_scale*2.0)/vcl_log(2.0);
//		int index = int(log2_scale*octave_size + 0.5);
//		int octave = index/octave_size;
//		int sub_index = index%octave_size;
//		//note the if we are looking for pixel (i,j) in the base scale, in a given scale,
//		//the pixel coordinates will be is = i/scale, js = j/scale
//
//		//int loc_scale = 1 << (scale_index/octave_size)-1;
//		float loc_scale = (float)vcl_pow(2.0f,float(scale_index/octave_size) - 1);
//		double ri = i/loc_scale;
//		double rj = j/loc_scale;
//		//double ri = i*loc_scale;
//		//double rj = j*loc_scale;
//
//		float dog_value = current_dog(ri,rj);
//
//		if( dog_value > max_dog_value )
//		{
//			max_scale_index = scale_index;
//			max_dog_value = dog_value;
//		}
//
//		scale_index_map[dog_value] = current_scale;
//
//	}
//
//	//the map stores the pairs from lowest to highest key, so the maximal scale/index pair should be the last element of the map
//	vcl_map<float,float>::iterator maximal_scale_map_itr = scale_index_map.end();
//	--maximal_scale_map_itr;//will balk if map is empty.
//	
//
//	
//	//actual scale is the closest image to the maximal scale available in the pyramid. They should be the same in our case
//	//but adding this just to make sure.
//	float actual_scale;
//	const vil_image_view<float>& orient_img = pyramid_set->grad_orient_at(maximal_scale_map_itr->second, &actual_scale);
//	const vil_image_view<float>& mag_img = pyramid_set->grad_mag_at(maximal_scale_map_itr->second);
//	double key_x = i/actual_scale;
//	double key_y = j/actual_scale;
//
//	bapl_lowe_orientation orientor(3.0,36);
//	vcl_vector<float> orientations;
//	orientor.orient_at(key_x,key_y,maximal_scale_map_itr->second,orient_img,mag_img,orientations);
//
//	//there will be many possible orientations, just use the first one.
//	keypoint = bapl_lowe_keypoint_new(pyramid_set,i,j,maximal_scale_map_itr->second,orientations[0]);
//	keypoint->set_id(keypoint_id);
//	++keypoint_id;
//	return true;
//}//end bapl_dense_sift

bapl_dense_sift::bapl_dense_sift( const vil_image_resource_sptr& image, unsigned octave_size, unsigned num_octaves )
{
	//because the pyramid set is a sptr don't have to worry about delete and memory leaks, just point to the new set.
	this->pyramid_sptr_ = new bapl_lowe_pyramid_set(image, octave_size, num_octaves);
	this->pyramid_valid_ = true;
}//end bapl_dense_sift::bapl_dense_sift

