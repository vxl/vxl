#ifndef brct_plane_corr_h_
#define brct_plane_corr_h_
//:
// \file
// \brief A class to store image correspondences
//
//
//
// \author J.L. Mundy
// \verbatim
// Initial version Feb. 2004
// May 1, 2004
// modified to make more generic using a correspondence base class - JLM
// \endverbatim
//
//////////////////////////////////////////////////////////////////////

#include <vbl/vbl_ref_count.h>
#include <vcl_vector.h>
#include <vcl_iosfwd.h>
#include <vgl/vgl_homg_point_2d.h>
#include <brct/brct_corr.h>
class brct_plane_corr : public brct_corr
{
 public:
  brct_plane_corr(const int n_cams, const int plane,
                  const int index);
  ~brct_plane_corr();
  //:Accessors
  int plane(){return plane_;}
  int index(){return index_;}

  //:Mutators

  //: Utility functions
  friend vcl_ostream&  operator<<(vcl_ostream& s, brct_plane_corr const& c);

 protected:
  brct_plane_corr();//shouldn't use this constructor
  //members
  int plane_; // the world plane
  int index_;//the point index in the plane
  //camera          match point
};

#endif // brct_plane_corr_h_
