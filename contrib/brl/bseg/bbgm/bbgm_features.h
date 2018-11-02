// This is brl/bseg/bbgm/bbgm_features.h
#ifndef bbgm_features_h_
#define bbgm_features_h_
//:
// \file
// \brief A set of features for probability image processing
// \author J.L. Mundy
// \date May 3, 2008
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vector>
#include <set>
#include <iostream>
#include <iosfwd>
#include <brip/brip_rect_mask.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_polygon.h>
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_set_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//================================================================
//=======================  mask feature ===========================
//================================================================

//: A background-foreground mask to detect boundaries or ribbons or corners
class bbgm_mask_feature
{
 public:
  bbgm_mask_feature():
    mid_(static_cast<unsigned char>(brip_rect_mask::ntypes)),
    aid_(static_cast<unsigned char>(brip_rect_mask::nangles)), p_(0.0f)
    {id_ = uid_++;}

  bbgm_mask_feature(brip_rect_mask::mask_id mid, brip_rect_mask::ang_id aid):
    mid_(mid),  aid_(aid), p_(0.0f)
    {id_ = uid_++;}

  //: set the probability value if the input probability is higher
  void set_prob(brip_rect_mask::mask_id mid, brip_rect_mask::ang_id aid, float p)
  {
    if (p>p_) {
      mid_ = static_cast<unsigned char>(mid);
      aid_ = static_cast<unsigned char>(aid);
      p_=p;
    }
  }

  //: set the probability value unconditionally
  void force_prob(float p) {p_ = p;}

  //: the mask id corresponding to current probability
  brip_rect_mask::mask_id mask_id() const
    {return static_cast<brip_rect_mask::mask_id>(mid_);}

  //: the angle id for the current mask id
  brip_rect_mask::ang_id ang_id() const
    {return static_cast<brip_rect_mask::ang_id>(aid_);}

  unsigned ni() const;

  unsigned nj() const;

  //: probability for the current template mask and angle
  float operator () () const { return p_;}

  //: mask id
  unsigned id() const {return id_;}

  //: pixels corresponding to + elements of the mask centered on (i, j)
  std::vector<vgl_point_2d<unsigned short> > pixels(unsigned i, unsigned j);

  //===========================================================================
  // Binary I/O Methods
  //===========================================================================

  //: Return a string name
  // \note this is probably not portable
  virtual std::string is_a() const;

  virtual bbgm_mask_feature* clone() const;

  //: Return IO version number
  short version() const;

  //: Binary save self to stream.
  virtual void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  virtual void b_read(vsl_b_istream &is);

 protected:
  static unsigned uid_; // a unique id
  unsigned id_;
  unsigned char mid_;
  unsigned char aid_;
  float p_;
};


void vsl_print_summary(std::ostream& os,
                       const bbgm_mask_feature& b);


//: Binary save bbgm_features
void vsl_b_write(vsl_b_ostream &os, const bbgm_mask_feature& b);


//: Binary load bbgm_features
void vsl_b_read(vsl_b_istream &is, bbgm_mask_feature& b);

//================================================================
//=======================  mask pair feature =====================
//================================================================

//: A pair of mask features with intervening foreground probability
// The pair is stored at each paired mask location
class bbgm_mask_pair_feature
{
 public:
  bbgm_mask_pair_feature(): mid_(brip_rect_mask::ntypes),
    ang0_(brip_rect_mask::nangles), ang1_(brip_rect_mask::nangles),
    i0_(0),j0_(0),i1_(0),j1_(0),p_(0.0f) { id_ = uid_++; }

  void set_prob(unsigned short i0, unsigned short j0,
                unsigned short i1, unsigned short j1, float p)
  {
    i0_ = i0; j0_ = j0;
    i1_ = i1; j1_ = j1;
    p_=p;
  }
  void set_mask_type(brip_rect_mask::mask_id mid) { mid_ = mid; }
  void set_angles(brip_rect_mask::ang_id ang0,
                  brip_rect_mask::ang_id ang1)
  {ang0_ = ang0; ang1_ = ang1;}

  //: set the unique id of each mask
  void set_ids( unsigned id0, unsigned id1) { id0_ = id0; id1_ = id1; }

  //: location of each mask in the pair
  void x0(unsigned short& i0, unsigned short& j0) const { i0 = i0_; j0 = j0_; }
  void x1(unsigned short& i1, unsigned short& j1) const { i1 = i1_; j1 = j1_; }

  //: the type of the mask pair
  brip_rect_mask::mask_id mask_id() const {return mid_;}

  //: the orientation of mask 0
  brip_rect_mask::ang_id ang0() const {return ang0_;}

  //: the orientation of mask 1
  brip_rect_mask::ang_id ang1() const {return ang1_;}

  //: the id of mask 0
  unsigned id0() const {return id0_;}

  //: the id of mask 1
  unsigned id1() const {return id1_;}

  void center(unsigned short& ic, unsigned short& jc) const
  {
    ic  = (unsigned short)((i0_+i1_)/2);
    jc  = (unsigned short)((j0_+j1_)/2);
  }
  //: probability for the mask pair
  float operator () () const { return p_;}

  //: pair id
  unsigned id() const {return id_;}

  //: The pixels in the pair (positive mask elements + path pixels)
  std::vector<vgl_point_2d<unsigned short> > pixels();

  //======================================================================
  // Binary I/O Methods
  //======================================================================

  //: Return a string name
  // \note this is probably not portable
  virtual std::string is_a() const;

  virtual bbgm_mask_pair_feature* clone() const;

  //: Return IO version number;
  short version() const;

  //: Binary save self to stream.
  virtual void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  virtual void b_read(vsl_b_istream &is);

 protected:
  static unsigned uid_; //!< a unique id
  unsigned id_;
  brip_rect_mask::mask_id mid_;  //!< type of masks in the pair
  brip_rect_mask::ang_id ang0_;  //!< orientation of mask 0
  brip_rect_mask::ang_id ang1_;  //!< orientation of mask 1
  unsigned short i0_, j0_;       //!< first mask location of the mask pair
  unsigned short i1_, j1_;       //!< second mask location of the mask pair
  unsigned id0_, id1_;
  float p_;
};


void vsl_print_summary(std::ostream& os,
                       const bbgm_mask_pair_feature& b);


//: Binary save bbgm_features
void vsl_b_write(vsl_b_ostream &os, const bbgm_mask_pair_feature& b);


//: Binary load bbgm_features
void vsl_b_read(vsl_b_istream &is, bbgm_mask_pair_feature& b);

//====================== global pair functions ==================

bool pair_intersect(bbgm_mask_pair_feature const& mp0,
                    bbgm_mask_pair_feature const& mp1,
                    bool plus_intersect_only = true);

//================================================================
//=======================  pair group feature ====================
//================================================================

//: functor to order the bbgm_pair_group_feature set
class fless
{
 public:
  fless() = default;
  bool operator ()(bbgm_mask_pair_feature const& fa,
                   bbgm_mask_pair_feature const& fb) {
    unsigned short ica, jca, icb, jcb;
    fa.center(ica, jca);
    fb.center(icb, jcb);
    if (ica!=icb)
      return ica<icb;
    return jca<jcb;
  }
};

//: A group of mask pair features with centers within a neighborhood
// The set is defined by the pair center locations
class bbgm_pair_group_feature
{
 public:
  bbgm_pair_group_feature(): mid_(brip_rect_mask::ntypes), ci_(0), cj_(0), p_(0) { id_ = uid_++; }
  //: A group with a single pair
  bbgm_pair_group_feature(bbgm_mask_pair_feature const& mp);
  //: set probability and vertices
  void set_prob(std::set<bbgm_mask_pair_feature, fless >const& pairs, float p)
  {
    pairs_ = pairs;
    std::set<bbgm_mask_pair_feature, fless >::const_iterator pit = pairs.begin();
    float cif = 0.0f, cjf = 0.0f;
    unsigned np = 0;
    for (; pit != pairs.end(); ++pit, ++np)
    {
      unsigned short ci, cj;
      pit->center(ci, cj);
      cif += (float)ci;
      cjf += (float)cj;
    }
    if (!np) return;
    ci_ = static_cast<unsigned short>(cif/(float)np);
    cj_ = static_cast<unsigned short>(cjf/(float)np);
    p_=p;
  }
  //: set probability with existing vertices
  void set_prob(float p) { p_ = p; }

  //: set the mask type of the group
  void set_mask_id(brip_rect_mask::mask_id mid) { mid_ = mid; }

  unsigned n_pairs() const {return pairs_.size();}

  const std::set<bbgm_mask_pair_feature, fless>& pairs() const
  { return pairs_; }

  //: probability for the pair group
  float operator () () const { return p_;}

  unsigned id() const {return id_;}

  //: the type of the mask group
  brip_rect_mask::mask_id mask_id() const {return mid_;}

  void center(unsigned short& ci, unsigned short& cj) const
  { ci = ci_; cj = cj_; }

  //: Bounding box
  vgl_box_2d<unsigned> bounding_box() const;

  //: Convex hull
  vgl_polygon<double> convex_hull() const;

  //: List of pixels in group ( + mask elements and paths)
  std::vector<vgl_point_2d<unsigned short> > pixels();

  //===========================================================================
  // Binary I/O Methods
  //===========================================================================

  //: Return a string name
  // \note this is probably not portable
  virtual std::string is_a() const;

  virtual bbgm_pair_group_feature* clone() const;

  //: Return IO version number;
  short version() const;

  //: Binary save self to stream.
  virtual void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  virtual void b_read(vsl_b_istream &is);

  virtual ~bbgm_pair_group_feature() = default; // virtual destructor, because presence of virtual methods
 protected:
  static unsigned uid_; //!< a unique id
  unsigned id_;
  brip_rect_mask::mask_id mid_; //!< the mask type of the group
  unsigned short ci_;           //!< the center of the group
  unsigned short cj_;
  std::set<bbgm_mask_pair_feature, fless > pairs_;
  float p_;
};


void vsl_print_summary(std::ostream& os,
                       const bbgm_pair_group_feature& b);


//: Binary save bbgm_features
void vsl_b_write(vsl_b_ostream &os, const bbgm_pair_group_feature& b);


//: Binary load bbgm_features
void vsl_b_read(vsl_b_istream &is, bbgm_pair_group_feature& b);

//====================== global pair_group functions ==================

bbgm_pair_group_feature pair_group_merge(bbgm_pair_group_feature const& pg0,
                                         bbgm_pair_group_feature const& pg1,
                                         float p_path = 1.0f);

bbgm_pair_group_feature
pair_group_max_union(bbgm_pair_group_feature const& pg0,
                     bbgm_pair_group_feature const& pg1);


#endif // bbgm_features to bbgm_features_h_
