#ifndef brct_corr_h_
#define brct_corr_h_
//:
// \file
// \brief A class to store image correspondences
// \author J.L. Mundy
// \date Initial version Feb. 2004
//
// \verbatim
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
  brct_corr(const int n_cams, const int plane,
            const int index);
  brct_corr(brct_corr const& x)
    : vbl_ref_count(), n_cams_(x.n_cams_), plane_(x.plane_),
      index_(x.index_), matches_(x.matches_) {}
  ~brct_corr();
  //:Accessors
  int plane(){return plane_;}
  int index(){return index_;}
  bool valid(const int cam){return matches_[cam].x()>=0;}
  vgl_homg_point_2d<double> match(const int cam){return matches_[cam];}

  //:Mutators
  void set_match(const int cam, const double x, const double y);
  //: Utility functions
  friend vcl_ostream&  operator<<(vcl_ostream& s, brct_corr const& c);
 protected:
  brct_corr();//shouldn't use this constructor
  //members
  int n_cams_;//!< number of views
  int plane_; //!< the world plane
  int index_; //!< the point index in the plane
  //: camera          match point
  vcl_vector<vgl_homg_point_2d<double> > matches_;//match in each camera
};

#endif // brct_corr_h_
