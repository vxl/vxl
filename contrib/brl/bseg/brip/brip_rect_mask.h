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
#include <string>
#include <iostream>
#include <iosfwd>
#include <map>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_matrix.h>

class brip_rect_mask
{
 public:
  //: construct from radius, aspect ratio and orientation (degrees)
  enum mask_id { cr1 = 0, s1, b0, b1, b0s, b1s, r10, r30, r31, r32, r51, r52,
                 c4_90_0, c4_45_0, c4_45_45, e2, e3, e5, e11, m1, m4, m4s,m7,
                 f3, b3, f5, b5, b7, ntypes};
  enum ang_id {a0 = 0, a22, a45, a67, a90, a112, a135, a157, a180,a202, a225,
               a247, a270, a292, a315, a337, nangles};
  brip_rect_mask(mask_id mid);
  ~brip_rect_mask() = default;

  //: initialize static quantities
  static void init();

  //: string name of angle id
  static std::string name(ang_id aid);

  //: floating point angle from id
  static float angle(ang_id aid);

  //: angle difference angle(aid1)-angle(aid0)
  static float ang_diff(ang_id aid0,ang_id aid1);

  //: Intersect '+' symbols of two masks of the same type
  static bool intersect(mask_id mid,
                        ang_id aid0,
                        unsigned short i0, unsigned short j0,
                        ang_id aid1,
                        unsigned short i1, unsigned short j1);
  //: Intersect full domain of two masks of the same type (0, ni, 0-nj)
  static bool intersect_domain(mask_id mid,
                               ang_id aid0,
                               unsigned short i0, unsigned short j0,
                               ang_id aid1,
                               unsigned short i1, unsigned short j1);
  //: the number of columns needed to span the mask
  unsigned ni() const {return current_mask_.cols();}

  //: the number of rows needed to span the mask
  unsigned nj() const {return current_mask_.rows();}

  //: the number of orientations
  unsigned n_angles() const {return masks_.size();}

  //: the orientation code
  ang_id angle_id(unsigned angle_index) const;

  //: the number of plus symbols for the current mask
  unsigned nplus() const;
  //: the number of minus symbols for the current mask
  unsigned nminus() const;

  //: set current angle id
  bool set_angle_id(ang_id aid);

  //: du and dv are measured from the center of the mask, returns +1, -1, 0;
  int operator () (int du, int dv) const
  {
    if (du>ru_||du<-ru_) return 0;
    if (dv>rv_||dv<-rv_) return 0;
    return current_mask_[dv+rv_][du+ru_];
  }
  //: print the mask with angle id, aid
  void print(ang_id aid);
 protected:
  bool find_ait(ang_id aid,
                std::map<ang_id, vnl_matrix<int> >::const_iterator& ait) const;
  brip_rect_mask();
  vnl_matrix<int> current_mask_;
  int ru_, rv_;
  std::map<ang_id, vnl_matrix<int> > masks_;
  static std::map<ang_id, std::string > names_;
  static std::map<ang_id, float > angles_;
  static bool is_init_;
};

std::ostream& operator<<(std::ostream& s, brip_rect_mask const& m);

#endif // brip_rect_mask_h_
