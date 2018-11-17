// This is brl/bseg/bapl/bapl_mi_matcher.h
#ifndef bapl_mi_matcher_h_
#define bapl_mi_matcher_h_
//:
// \file
// \brief A class to find a matching ROI using mutual information
// \author Matt Leotta
// \date Oct 14, 2003
//
// \verbatim
//  Modifications
//   None
// \endverbatim

#include <iostream>
#include <vector>
#include <bapl/bapl_affine_transform.h>
#include <bapl/bapl_mi_matcher_params.h>
#include <vil/vil_image_view.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class bapl_affine_roi;

//:A class to match a base ROI (rectangular) to an affine ROI using mutual information.
//

class bapl_mi_matcher
{
 public:
  //: Constructor
  bapl_mi_matcher(const vil_image_view<vxl_byte>& src_image,
                  const vil_image_view<vxl_byte>& target_patch,
                  bapl_affine_transform init_xform,
                  bapl_mi_matcher_params params = bapl_mi_matcher_params());

  //: Constructor
  bapl_mi_matcher(const vil_image_view<vxl_byte>& src_image,
                  bapl_affine_roi& roi,
                  bapl_mi_matcher_params params = bapl_mi_matcher_params());

  void set_params(bapl_mi_matcher_params& params) { params_ = params; }

  //: Generate random candidate matches
  void generate();

  //: Return the transformation of the best match
  bapl_affine_transform best_xform();

 private:
  //: Default Constructor
  bapl_mi_matcher() = delete;

  //: Generate a random transform (close to init_xform_)
  bapl_affine_transform rand_transform();

  //: Calculate the mutual information for the image at the transformed location
  double mutual_info(const bapl_affine_transform& T);

  class bapl_match
  {
  public:
    bapl_match(double mi, bapl_affine_transform x) : mut_info(mi), xform(x) {}
    bool operator< (const bapl_match& rhs) const { return this->mut_info > rhs.mut_info; }
    double mut_info;
    bapl_affine_transform xform;
  };

  //: The source image
  vil_image_view<vxl_byte> src_img_;
  //: The target image patch
  vil_image_view<vxl_byte> tgt_img_;
  //: The entropy of the target patch
  double tgt_entropy_;
  //: The affine transformation
  bapl_affine_transform init_xform_;
  //: The generated matches
  std::vector<bapl_match> matches_;
  //: The parameters for generating matches
  bapl_mi_matcher_params params_;
};

#endif // bapl_mi_matcher_h_
