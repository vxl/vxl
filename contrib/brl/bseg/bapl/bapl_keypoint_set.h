// This is brl/bseg/bapl/bapl_keypoint_set.h
#ifndef bapl_keypoint_set_h_
#define bapl_keypoint_set_h_
//:
// \file
// \brief A wrapper class to hold a vector of keypoints
// \author Ozge C. Ozcanli, (ozge@lems.brown.edu)
// \date Sep 16, 2010
//
//
// \verbatim
//   none
// \endverbatim

#include <iostream>
#include <vector>
#include <vbl/vbl_ref_count.h>
#include <bapl/bapl_keypoint_sptr.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_io.h>

class bapl_keypoint_set : public vbl_ref_count
{
 public:
  //: Constructor
  bapl_keypoint_set(std::vector<bapl_keypoint_sptr> & keys) : keys_(keys) {}

  std::vector<bapl_keypoint_sptr> keys_;
};

// Binary io, NOT IMPLEMENTED, signatures defined to use bapl_keypoint_set as a brdb_value
void vsl_b_write(vsl_b_ostream & os, bapl_keypoint_set const &ph);
void vsl_b_read(vsl_b_istream & is, bapl_keypoint_set &ph);
void vsl_b_read(vsl_b_istream& is, bapl_keypoint_set* ph);
void vsl_b_write(vsl_b_ostream& os, const bapl_keypoint_set* &ph);

//: the id of the keypoint is its order in the bapl_keypoint_set of the image
typedef std::pair<bapl_keypoint_sptr, bapl_keypoint_sptr> bapl_key_match;

class bapl_keypoint_match_set : public vbl_ref_count
{
 public:
  //: Constructor
  bapl_keypoint_match_set(int id_left, int id_right, std::vector<bapl_key_match> & matches) : id_left_(id_left), id_right_(id_right), matches_(matches) {}

  //: used to sort matches of an image (left image) wrt ids of other (right) images that it matches to
  bool operator< (const bapl_keypoint_match_set &other) const { return id_right_ < other.id_right_;  }

  //: remove spurious matches, i.e remove if a keypoint from J is shared : (i1,j) (i2,j), remove (i2,j) since one of them is definitely spurious
  static void prune_spurious_matches(std::vector<bapl_key_match>& matches);

  //: refine matches by computing F
  void refine_matches(float outlier_threshold, std::vector<bapl_key_match>& refined_matches);

  int id_left_;   // id of the left image
  int id_right_;  // id of the right image
  std::vector<bapl_key_match> matches_;
};

// Binary io, NOT IMPLEMENTED, signatures defined to use bapl_keypoint_match_set as a brdb_value
void vsl_b_write(vsl_b_ostream & os, bapl_keypoint_match_set const &ph);
void vsl_b_read(vsl_b_istream & is, bapl_keypoint_match_set &ph);
void vsl_b_read(vsl_b_istream& is, bapl_keypoint_match_set* ph);
void vsl_b_write(vsl_b_ostream& os, const bapl_keypoint_match_set* &ph);

#endif // bapl_keypoint_set_h_
