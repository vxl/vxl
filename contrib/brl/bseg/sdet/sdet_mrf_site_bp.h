#ifndef sdet_mrf_site_bp_h_
#define sdet_mrf_site_bp_h_
//:
// \file
// \brief  A class for representing a site node in an MRF
// \author J.L. Mundy
// \date   26 March 2011
//
// Stores two sets of buffer arrays,one for messages received at the last step
//  and one for messages coming in during the current iteration
// the MRF has a 4-neighborhood (u, l, r, d) with the neighbor index
// in the order (0 1 2 3).
//
// The data cost for each site is D(fp) = lambda_*(fp-x)^2, where x is the
// observed data and fp is a site label.
//
// D(fp) is set to min(lambda_*(fp-x)^2, truncation_cost_);
//
//  Each site stores a pair of label buffers for each of the neighbors
//  One buffer in the pair stores the message received on the last
//  iteration (p), the other receives the current messages (c). On each
//  iteration, the buffers are swapped. A pair of buffers is allocated for
//  each of the (u, l, r, d) neighbors in the 4-connected neighborhood
//  as shown below.
//
//          (c p)
//            u
//    (c,p) l x r (c,p)
//            d
//          (c,p)
//
#include <iostream>
#include <vector>
#include <vbl/vbl_ref_count.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
class sdet_mrf_site_bp : public vbl_ref_count
{
 public:

  sdet_mrf_site_bp(unsigned n_labels, float lambda, float truncation_cost);
  void switch_buffers() { prior_ = 1-prior_; }
  int prior() const { return prior_; }
  int current() const { return 1-prior_; }
  //: set the observed label
  void set_label(float obs_label) { obs_label_ = obs_label; }

  // === cost functions ===

  //:data cost due to observed continuous label value
  float D(unsigned fp);

  //:sum over stored prior messages, except the message from neighbor nq
  float M(unsigned nq, unsigned fp);

  //:total of D and M
  float h(unsigned nq, unsigned fp) { return D(fp) + M(nq, fp); }

  //:belief, sum of data cost and sum of all four prior messages
  float b(unsigned fp);

  //:the most probable label, label with minimum belief
  unsigned believed_label();

  //set the current message from neighbor nq
  void set_cur_message(unsigned nq, unsigned fp, float msg);

  //:the current message value
  float cur_message(unsigned nq, unsigned fp) const { return msg_[1-prior_][nq][fp]; }

  //:the prior message value
  float prior_message(unsigned nq, unsigned fp) const { return msg_[prior_][nq][fp]; }

  //:entire prior message
  std::vector<float> prior_message(unsigned nq);

  //:set prior message
  void set_prior_message(unsigned nq, std::vector<float>const& msg);

  //:clear messages
  void clear();

  //: print the value of the messages held in the prior queue.
  void print_prior_messages();
  void print_current_messages();
  void print_belief_vector();

 protected:
  // parameters for computing message values
  float lambda_;
  float truncation_cost_;

  //: this index is toggled to swap the buffers
  int prior_;

  //: typically 256
  unsigned n_labels_;

  //: currently 4, but might change in the future
  unsigned n_ngbh_;

  // define the neighbor index
  //     0
  //   1 x 2
  //     3

  //: a set of 2 message buffers, prior and current, one for each neighbor
  // (p, c)     n_ngbh_    n_labels_
  std::vector< std::vector<std::vector<short> > > msg_;
  // cut down storage using short (for byte images should be adequate)

  //: the label represented by the data
  float obs_label_;
};

#include <sdet/sdet_mrf_site_bp_sptr.h>
#endif // sdet_mrf_site_bp_h_
