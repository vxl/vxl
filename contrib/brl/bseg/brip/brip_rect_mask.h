// This is brl/bseg/brip/brip_rect_mask.h
#ifndef brip_rect_mask_h_
#define brip_rect_mask_h_
//-----------------------------------------------------------------------------
//:
// \file
// \author J.L. Mundy
// \brief A rectangular pixel mask with positive interior negative surround
//
// \verbatim
//  Modifications
//   Initial version April 17, 2008
// \endverbatim
//
//-----------------------------------------------------------------------------
//  An example mask
//
//
//       0 0 0 0 - - 0
//       0 0 0 - + - 0
//       0 0 - + - 0 0
//       0 - + - 0 0 0
//       0 - - 0 0 0 0
//
#include <vcl_iosfwd.h>
#include <vnl/vnl_matrix.h>
#include <vcl_map.h>

class brip_rect_mask
{
 public:
  //: construct from radius, aspect ratio and orientation (degrees)
  enum mask_id { cr1 = 0, s1, b0, b1, b0s, b1s, r10, r30, r31, r32, r51, r52,
                 c4_90_0, c4_45_0, c4_45_45, e5, e11, ntypes};
  enum ang_id {a0 = 0, a22, a45, a67, a90, a112, a135, a157, a180, a225,
               a270, a315, nangles};
  brip_rect_mask(mask_id mid);
  ~brip_rect_mask() {}

  //: the number of columns needed to span the mask
  unsigned ni() const {return current_mask_.cols();}

  //: the number of rows needed to span the mask
  unsigned nj() const {return current_mask_.rows();}

  //: the number of orientations
  unsigned n_angles() const {return masks_.size();}

  //: the orientation code
  ang_id angle_id(unsigned angle_index) const;

  //: set current angle id
  bool set_angle_id(ang_id aid);

  //: du and dv are measured from the center of the mask, returns +1, -1, 0;
  int operator () (int du, int dv) const
  {
    if (du>ru_||du<-ru_) return 0;
    if (dv>rv_||dv<-rv_) return 0;
    return current_mask_[dv+rv_][du+ru_];
  }

 protected:
  bool find_ait(ang_id aid,
                vcl_map<ang_id, vnl_matrix<int> >::const_iterator& ait) const;
  brip_rect_mask();
  vnl_matrix<int> current_mask_;
  int ru_, rv_;
  vcl_map<ang_id, vnl_matrix<int> > masks_;
};

vcl_ostream& operator<<(vcl_ostream& s, brip_rect_mask const& m);

#endif // brip_rect_mask_h_
