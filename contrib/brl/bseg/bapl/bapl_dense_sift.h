// This is brl/bseg/bapl/bapl_dense_sift.h
#ifndef bapl_dense_sift_h_
#define bapl_dense_sift_h_
//:
// \file
// \brief Extract SIFT features from every pixel in an Image or a specific region.
// \author Brandon Mayer (b.mayer1@gmail.com)
// \date Dec 10, 2010
//
// \verbatim
//  Modifications
// \endverbatim

#include <iostream>
#include <cmath>
#include <map>
#include <vector>
#include <bapl/bapl_keypoint_extractor.h>
#include <bapl/bapl_keypoint_sptr.h>
#include <bapl/bapl_lowe_pyramid_set.h>
#include <bapl/bapl_lowe_pyramid_set_sptr.h>

#include <vbl/vbl_ref_count.h>

#include <vgl/vgl_point_2d.h>

#include <vil/vil_image_resource_sptr.h>


#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class bapl_dense_sift:public vbl_ref_count
{
 public:
  bapl_dense_sift():pyramid_valid_(false) {}

  bapl_dense_sift(const vil_image_resource_sptr& image,
                  unsigned octave_size = 6,
                  unsigned num_octaves = 1 );

  ~bapl_dense_sift() override = default;

  void create_pyramid(const vil_image_resource_sptr& image, unsigned octave_size = 6, unsigned num_octaves = 1);

  //output := keypoint
  //parameters := (sub)pixel location (i,j)
  bool make_keypoint( bapl_lowe_keypoint_sptr& keypoint, double const& i, double const& j);

  //output := keypoints
  //parameters := istep,jstep
  bool make_dense_keypoints(std::vector<bapl_lowe_keypoint_sptr>& keypoints, unsigned const istep = 1, unsigned const jstep = 1 );

  //output := keypoints
  //parameters := target image locations (vgl_point_2d<unsigned> > pts)
  bool make_keypoints( std::vector<bapl_lowe_keypoint_sptr>& keypoints, std::vector<vgl_point_2d<unsigned> > const& pts );

  //input := vector of keypoints with locations specified in the keypoint
  //output := the modified keypoints
  bool make_keypoints( std::vector<bapl_lowe_keypoint_sptr>& keypoints );

  bapl_lowe_pyramid_set_sptr pyramid_sptr() {return this->pyramid_sptr_;}

  unsigned ni() const {return this->ni_;}

  unsigned nj() const {return this->nj_;}

  unsigned octave_size() const {return this->pyramid_sptr_->num_octaves();}

  unsigned num_octaves() const {return this->pyramid_sptr_->octave_size();}

 private:
  bapl_lowe_pyramid_set_sptr pyramid_sptr_;
  unsigned ni_;
  unsigned nj_;
  bool pyramid_valid_;
  static unsigned keypoint_id_;
};

#endif//bapl_dense_sift_h_
