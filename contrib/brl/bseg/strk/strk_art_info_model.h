// This is brl/bseg/strk/strk_art_info_model.h
#ifndef strk_art_info_model_h_
#define strk_art_info_model_h_
//:
// \file
// \brief an articulated model to support information-based tracking
//
//  The model has three regions:
//   - a body region
//   - a long tip region
//   - a short tip region
//
//  The model has a number of degrees of freedom
//   - global translation and rotation about the center of the body
//   - Each tip region can pivot around the body center at a fixed radius
//   - Each tip region can rotate about its own center
//
//  The regions are instances of strk_tracking_face and so can
//  compute mutual information based on both intensity and
//  gradient statistics.
//
// \author
//    Joseph L. Mundy - November 05, 2003
//    Brown University
//
// \verbatim
//  Modifications
//   10-sep-2004 Peter Vanroose Added copy ctor with explicit vbl_ref_count init
// \endverbatim
//
//-----------------------------------------------------------------------------
#include<vcl_vector.h>
#include<vbl/vbl_ref_count.h>
#include<vsol/vsol_point_2d_sptr.h>
#include<vtol/vtol_face_2d_sptr.h>
#include<vil1/vil1_memory_image_of.h>
#include<strk/strk_tracking_face_2d_sptr.h>
#include<strk/strk_art_info_model_sptr.h>

class strk_art_info_model : public vbl_ref_count
{
 public:
  enum model_part_id{STEM=0, LONG_TIP, SHORT_TIP};
  //:constructors/destructors
  strk_art_info_model(vcl_vector<vtol_face_2d_sptr> const& faces,
                      vsol_point_2d_sptr const stem_pivot,
                      vil1_memory_image_of<float> & image);

  strk_art_info_model(vcl_vector<vtol_face_2d_sptr> const& face,
                      vsol_point_2d_sptr const stem_pivot,
                      vil1_memory_image_of<float>& image,
                      vil1_memory_image_of<float>& Ix,
                      vil1_memory_image_of<float>& Iy);

  strk_art_info_model(strk_art_info_model_sptr const& im);

  strk_art_info_model(strk_art_info_model const& im);

  ~strk_art_info_model();

  // accessors
  unsigned int n_faces() const { return faces_.size(); }
  strk_tracking_face_2d_sptr face(int i) const;
  strk_tracking_face_2d_sptr stem() const;
  strk_tracking_face_2d_sptr left_tip() const;
  strk_tracking_face_2d_sptr right_tip() const;
  vsol_point_2d_sptr stem_pivot() const;
  vcl_vector<vtol_face_2d_sptr> vtol_faces() const;
  // information
  double total_model_info() const { return total_model_info_; }
  // utility methods
  bool transform(const double stem_tx, const double stem_ty,
                 const double stem_angle,
                 const double long_arm_pivot_angle,
                 const double short_arm_pivot_angle,
                 const double long_tip_angle,
                 const double short_tip_angle);

  bool compute_mutual_information(vil1_memory_image_of<float> const& image);

  bool compute_mutual_information(vil1_memory_image_of<float> const& image,
                                  vil1_memory_image_of<float> const& Ix,
                                  vil1_memory_image_of<float> const& Iy);
 private:
  // methods
  strk_art_info_model();
  double arm_radius(strk_tracking_face_2d_sptr const& face);
  bool pivot_tip_face(strk_tracking_face_2d_sptr const& face,
                      const double angle, const double radius);

  // members
  double total_model_info_;
  double long_arm_radius_; //these are invariant
  double short_arm_radius_;
  vsol_point_2d_sptr stem_pivot_;
  vcl_vector<strk_tracking_face_2d_sptr> faces_;
};

#endif // strk_art_info_model_h_
