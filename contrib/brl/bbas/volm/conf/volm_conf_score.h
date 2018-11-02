//This is brl/bbas/volm/conf/volm_conf_score.h
#ifndef volm_conf_score_h_
#define volm_conf_score_h_
//:
// \file
// \brief  A simple class to warp output of volumetric configurational matcher
//
//
// \author Yi Dong
// \date August 28, 2014
// \verbatim
//   Modifications
// \endverbatim

#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
#include <vsl/vsl_binary_io.h>
#include <volm/conf/volm_conf_object.h>

class volm_conf_score;
typedef vbl_smart_ptr<volm_conf_score> volm_conf_score_sptr;

class volm_conf_score : public vbl_ref_count
{
 public:
   //: constructors
   volm_conf_score(): score_(0.0f), theta_(0.0f) { landmarks_.clear(); }
   volm_conf_score(float const& score, float const& theta, std::vector<volm_conf_object>  landmarks);
   //: destructor
   ~volm_conf_score() override = default;
   //: set method
   void set_score(float const& score) { score_ = score; }
   void set_theta(float const& theta) { theta_ = theta; }
   void set_landmarks(std::vector<volm_conf_object> const& landmarks) { landmarks_ = landmarks; }
   //: access
   float score() const { return score_; }
   float theta() const { return theta_; }
   std::vector<volm_conf_object>& landmarks() { return landmarks_; }
   float theta_in_deg() const;
   //: return the angular value in degree relative to north
   float theta_to_north() const;


   //: print
   void print(std::ostream& os) const
   {
     os << "boxm2_volm_conf_socre -- score: " << score_ << ", theta: " << theta_ << " (" << this->theta_in_deg() << " degree)" << std::endl;
   }

  // ================  binary I/O ===================
  //: version
  unsigned char version() const { return (unsigned char)2; }
  //: binary IO write
  void b_write(vsl_b_ostream& os) const;
  //: binary IO read
  void b_read(vsl_b_istream& is);

 private:
  //: a float number normally from 0 to 1 that quantifies the matcher result
  float score_;
  //: a direction relative to a location origin which defines the favored heading (from 0 to 2*pi and 0 refers to east direction)
  float theta_;
  //: a list of volm_conf_objects which are the best matched landmarks
  std::vector<volm_conf_object> landmarks_;
};

void vsl_b_write(vsl_b_ostream& os, volm_conf_score const&           socre);
void vsl_b_write(vsl_b_ostream& os, volm_conf_score const*       score_ptr);
void vsl_b_write(vsl_b_ostream& os, volm_conf_score_sptr const& score_sptr);

void vsl_b_read(vsl_b_istream& is, volm_conf_score&           socre);
void vsl_b_read(vsl_b_istream& is, volm_conf_score*&      score_ptr);
void vsl_b_read(vsl_b_istream& is, volm_conf_score_sptr& score_sptr);


#endif // volm_conf_score_h_
