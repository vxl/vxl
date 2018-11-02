#include "bapl_dense_sift.h"

#include <vil/algo/vil_orientations.h>
#include <iomanip>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

unsigned bapl_dense_sift::keypoint_id_ = 0;

bapl_dense_sift::bapl_dense_sift(
  const vil_image_resource_sptr& image,
  unsigned octave_size, unsigned num_octaves )
{
  this->create_pyramid(image,octave_size,num_octaves);
}//end bapl_dense_sift::bapl_dense_sift


void bapl_dense_sift::create_pyramid(
  const vil_image_resource_sptr& image,
  unsigned octave_size, unsigned num_octaves)
{
  this->ni_ = image->ni();
  this->nj_ = image->nj();
  //because the pyramid set is a sptr don't have to worry
  //about delete and memory leaks, just point to the new set.
  this->pyramid_sptr_ = new bapl_lowe_pyramid_set(image, octave_size, num_octaves);
  this->pyramid_valid_ = true;
}

bool bapl_dense_sift::make_keypoint(bapl_lowe_keypoint_sptr& keypoint,
                                    double const& i, double const& j)
{
  if ( this->pyramid_valid_ == true )
  {
    std::map<float,float> scale_index_map;

    for ( unsigned int scale_index = 0;
          scale_index < this->octave_size()*this->num_octaves(); ++scale_index )
    {

      std::cout << "scale " << scale_index + 1 << " of "
               << this->octave_size()*this->num_octaves()
               << std::endl;

      const vil_image_view<float>& current_dog = this->pyramid_sptr_->
        dog_pyramid(scale_index / this->num_octaves(),
                    scale_index % this->num_octaves());

      std::cout << "current_dog.size() = " << current_dog.ni()
               << ", " << current_dog.nj() << std::endl;

      //this value is used in the orientation and grad_mag images
      //in the pyramid to recover the scale_index value which is a linear index
      //into a 2d coordinate system, a (scale,octave).
      //We maximize over the linear scale_index then use this value
      //to retrieve the appropriate ancillary images.
      auto current_scale = (float)std::pow(
        2.0f,float(scale_index)/this->octave_size()-1);

      //the first level in the pyramid is an 2x upsampled version
      //of the original image with each resulting octave, the image resolution
      //is reduced by half. Therefore we need to divide the
      //image coordinates by the correct power of two of the resolution.
      float resolution = 1.0f / current_scale;
      auto ri = (unsigned int)(i*resolution);
      auto rj = (unsigned int)(j*resolution);

      std::cout << "current_dog size = "
               << current_dog.ni() << ", "
               << current_dog.nj() << std::endl;

      scale_index_map[std::fabs(current_dog(ri,rj))] = current_scale;
    }//end scale iteration

    //map stores pairs from lowest to highest key.
    //The maximal scale/index pair should be the last element of the map
    auto maximal_scale_map_itr =
      scale_index_map.rbegin();

    //actual scale is the closest image to the maximal scale available in the pyramid.
    //Describes the resolution of the image at a given scale.
    float actual_scale;
    const vil_image_view<float>& orient_img =
      this->pyramid_sptr_->grad_orient_at(
        maximal_scale_map_itr->second, &actual_scale);

    const vil_image_view<float>& mag_img = this->pyramid_sptr_->
      grad_mag_at(maximal_scale_map_itr->second);

    float key_x = float(i)/actual_scale;
    float key_y = float(j)/actual_scale;

    bapl_lowe_orientation orientor(3.0,36);//same parameters matt used.
    std::vector<float> orientations;
    orientor.orient_at(key_x, key_y, maximal_scale_map_itr->second,
                       orient_img, mag_img, orientations);

    //there will be many possible orientations,
    //normally we would make a new keypoint for each orientation but for
    //dense sift, we will only use the first orientation.
    keypoint = bapl_lowe_keypoint_new(this->pyramid_sptr_, i, j,
      maximal_scale_map_itr->second, orientations[0]);

    keypoint->set_id(bapl_dense_sift::keypoint_id_);
    ++bapl_dense_sift::keypoint_id_;
    return true;
  }
  else
  {
    std::cerr << "ERROR: bapl_dense_sift::make_keypoint, pyramid is not valid\n";
    return false;
  }
}//end bapl_dense_sift::make_keypoint

bool bapl_dense_sift::make_keypoints( std::vector<bapl_lowe_keypoint_sptr>& keypoints, std::vector<vgl_point_2d<unsigned> > const& pts)
{
  std::vector<vgl_point_2d<unsigned> >::const_iterator target_itr,target_end;
  target_end = pts.end();

  for (target_itr = pts.begin(); target_itr != target_end; ++target_itr)
  {
    bapl_lowe_keypoint_sptr keypoint;
    if (!this->make_keypoint( keypoint, target_itr->x(), target_itr->y() ))
      return false;
    keypoints.push_back(keypoint);
  }//end target iteration
  return true;
}//end bapl_dense_sift::make_keypoints

bool bapl_dense_sift::make_dense_keypoints(std::vector<bapl_lowe_keypoint_sptr>& keypoints, unsigned const istep, unsigned const jstep)
{
  //the original image resolution is on the second level of the pyramid.
  for (unsigned i = 0; i < this->ni_; i+=istep)
    for (unsigned j = 0; j < this->nj_; j+=jstep)
    {
      bapl_lowe_keypoint_sptr keypoint;
      if (!this->make_keypoint(keypoint,i,j))
        return false;
      keypoints.push_back(keypoint);
    }

  return true;
}//end bapl_dense_sift::make_keypoints

bool bapl_dense_sift::make_keypoints(std::vector<bapl_lowe_keypoint_sptr>& keypoints)
{
  std::vector<bapl_lowe_keypoint_sptr>::iterator k_itr, k_end= keypoints.end();

  for ( k_itr = keypoints.begin(); k_itr != k_end; ++k_itr )
    if ( !this->make_keypoint(*k_itr, (*k_itr)->location_i(), (*k_itr)->location_j()) )
      return false;

  return true;
}//end bapl_dense_sift::make_keypoints
