#ifndef bwm_3d_corr_h_
#define bwm_3d_corr_h_
//:
// \file
// \brief A class to store 3-d to 3-d correspondences
// \author J.L. Mundy
// \date Initial version December, 4, 2010
//
// \verbatim
//  no modifications
// \endverbatim
//
//////////////////////////////////////////////////////////////////////

#include <vbl/vbl_ref_count.h>

#include <vcl_vector.h>
#include <vcl_string.h>
#include <vcl_map.h>
#include <vcl_utility.h>
#include <vcl_iosfwd.h>
#include <vgl/vgl_point_3d.h>

//: Correspondence between 3-d points, each in different bwm sites.
// The site is designated by its string name. Typically there will be
// two sites, the cameras for each generated from independent bundle
// adjustment processes. Thus, the 3-d coordinate frames of the two
// sites are not the same and differ by a similarity transform in most cases.
// The transform can be computed given a set of bwm_3d_corr(s).
class bwm_3d_corr : public vbl_ref_count
{
 public:
   bwm_3d_corr() {}
  ~bwm_3d_corr() {}
  //: find a matching 3-d point
  bool match(vcl_string const& site, vgl_point_3d<double> &pt) const;
  //: size
  int num_matches() const { return matches_.size(); }

  //: erase a match
  void erase(vcl_string const& site);

  //: list of sites being corresponded
  vcl_vector<vcl_string> sites() const;

  //: returns true and the 3-d point, if the site is corresponded
  bool site_in(vcl_string const& site, vgl_point_3d<double> &corr) const;

  //: returns the match list as site name mapped to the 3-d points
  vcl_vector<vcl_pair<vcl_string, vgl_point_3d<double> > > match_list() const;

  //: return a vector of corresponding 3-d points
  vcl_vector<vgl_point_3d<double> > matching_pts() const;

  // === Mutators ===

  //: replace a 3-d point
  bool update_match(vcl_string const& site,
                    vgl_point_3d<double> old_pt,
                    vgl_point_3d<double> new_pt);
  //:set a match
  void set_match(vcl_string const& site,
                 const double x, const double y, const double z);

  //: Utility functions
  friend vcl_ostream&  operator<<(vcl_ostream& s, bwm_3d_corr const& c);

 protected:
  // === Members ===

  vcl_map<vcl_string, vgl_point_3d<double> > matches_;//!< match in each site
};

#endif // bwm_3d_corr_h_
