#ifndef brct_corr_h_
#define brct_corr_h_
//:
// \file
// \brief A class to store image correspondences
// \author J.L. Mundy
// \date Initial version Feb. 2004
//
// \verbatim
// Initial version Feb. 2004
// May 1, 2004
// modified to make more generic using a correspondence base class - JLM
//  Modifications
//   10-sep-2004 Peter Vanroose Added copy ctor with explicit vbl_ref_count init
// \endverbatim
//
//////////////////////////////////////////////////////////////////////

#include <vbl/vbl_ref_count.h>
#include <vcl_vector.h>
#include <vcl_iosfwd.h>
#include <vgl/vgl_homg_point_2d.h>

class brct_corr : public vbl_ref_count
{
 public:
  brct_corr(const int n_cams);
  brct_corr(brct_corr const& x)
    : vbl_ref_count(), n_cams_(x.n_cams_){}
  ~brct_corr();
  //:Accessors
  bool valid(const int cam){return valid_[cam];}
  vgl_homg_point_2d<double> match(const int cam){return matches_[cam];}

  //:Mutators
  void set_match(const int cam, const double x, const double y);
  //: Utility functions
  friend vcl_ostream&  operator<<(vcl_ostream& s, brct_corr const& c);
 protected:
  brct_corr();//shouldn't use this constructor
  //members
  int n_cams_;//number of views
  //camera          match point
  vcl_vector<vgl_homg_point_2d<double> > matches_;//match in each camera
  vcl_vector<bool> valid_; //has match been set?
};

#endif // brct_corr_h_
