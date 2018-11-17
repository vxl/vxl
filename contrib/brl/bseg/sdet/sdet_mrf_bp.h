#ifndef sdet_mrf_bp_h_
#define sdet_mrf_bp_h_
//:
// \file
// \brief  A class for representing a set of sites in an MRF
// \author J.L. Mundy
// \date   26 March 2011
//
//  Each MRF site has a 4-neighborhood,
//     u
//   l x  r
//     d
// with index (u, l, r, d) ==> (0, 1, 2, 3)
//
#include <iostream>
#include <vector>
#include <sdet/sdet_mrf_site_bp_sptr.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vbl/vbl_array_2d.h>
#include <vbl/vbl_ref_count.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class sdet_mrf_bp : public vbl_ref_count
{
 public:
  //: simple constructor for testing
  sdet_mrf_bp(unsigned ni, unsigned nj, unsigned n_labels);

  //: constructor with observed labels provided by resc
  // In this case the data cost is just $\lambda \times (fp-x)^2$
  sdet_mrf_bp(const vil_image_resource_sptr& obs_labels, unsigned n_labels,
              float discontinuity_cost, float truncation_cost,
              float kappa, float lambda);

  //: constructor with observed labels view
  sdet_mrf_bp(vil_image_view<float> const& obs_labels, unsigned n_labels,
              float discontinuity_cost, float truncation_cost,
              float kappa, float lambda);

  //: constructor with observed labels and variance resources
  sdet_mrf_bp(const vil_image_resource_sptr& obs_labels,
              const vil_image_resource_sptr& var,  unsigned n_labels,
              float discontinuity_cost, float truncation_cost,
              float kappa, float lambda);

  //: constructor with observed label and variance views.
  sdet_mrf_bp(vil_image_view<float> const& obs_labels,
              vil_image_view<float> const& var,  unsigned n_labels,
              float discontinuity_cost, float truncation_cost,
              float kappa, float lambda);

  //: limit cost at a discontinuity
  void set_discontinuity_cost(float discontinuity_cost)
    { discontinuity_cost_ = discontinuity_cost; }

  //: truncation of data cost
  void set_truncation_cost(float truncation_cost)
    { truncation_cost_ =truncation_cost; }

  //: contribution of data to cost
  void set_lambda(float lambda) { lambda_ = lambda; }

  //: the contribution of neighbor label difference to cost
  void set_kappa(float kappa) { kappa_ = kappa; }

  //: transform from image coordinates to node indices
  unsigned image_to_index(unsigned i, unsigned j) { return i + ni_*j; }

  //: transform from node indices to image coordinates
  void index_to_image(unsigned p, unsigned& i, unsigned& j)
    { j = p/ni_; i = p-j*ni_; }

  //: mrf dimension (columns)
  unsigned ni() const { return ni_; }
  //: mrf dimension (rows)
  unsigned nj() const { return nj_; }

  //: retrieve a site by image index
  sdet_mrf_site_bp_sptr site(unsigned i, unsigned j) { return sites_[j][i]; }

  //: retrieve a site by linear index
  sdet_mrf_site_bp_sptr site(unsigned p) { unsigned i, j; index_to_image(p,i,j); return sites_[j][i]; }

  //: get the contents of a prior message buffer
  std::vector<float> prior_message(unsigned i, unsigned j, unsigned n);

  //: set the contents of a prior message buffer
  void set_prior_message(unsigned i, unsigned j, unsigned n,
                         std::vector<float> const& msg);

  //:
  // all sites send messages to current buffer of neighbors,
  // using an O(n_labels) algorithm based on the lower envelope
  void send_messages_optimized();

  //: clear messages from all sites
  void clear();

  //: all sites print the contents of the prior buffers
  void print_prior_messages();

  //: all sites print their belief vector
  void print_belief_vectors();

  //: output
  vil_image_resource_sptr belief_image();


 protected:
  //members
  unsigned ni_;
  unsigned nj_;
  unsigned n_labels_;
  float discontinuity_cost_;
  float truncation_cost_;
  float kappa_;
  float lambda_;
  float min_;
  float max_;
  //the array of sites
  vbl_array_2d<sdet_mrf_site_bp_sptr> sites_;
  sdet_mrf_bp();
};

// === public functions ===

//: computes the lower envelope of a message array with $V(fp, fq) = w|fp-fq|$
//  Computes in place
void lower_envelope_linear(float w, std::vector<float>& msg);

//: computes the lower envelope of a message array with $V(fp, fq) = w(fp-fq)^2$
//  Used in current implementation
std::vector<float> lower_envelope_quadratic(float w,
                                           std::vector<float> const& h);

#include <sdet/sdet_mrf_bp_sptr.h>
#endif // sdet_mrf_bp_h_
