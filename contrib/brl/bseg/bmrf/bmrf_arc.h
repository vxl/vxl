// This is brl/bseg/bmrf/bmrf_arc.h
#ifndef bmrf_arc_h_
#define bmrf_arc_h_
//:
// \file
// \brief An arc in a Markov Random Field (MRF) network
// \author Matt Leotta, (mleotta@lems.brown.edu)
// \date 6/7/04
//
// The MRF arc contains a directed link from one node to another
//
// \verbatim
//  Modifications
// \endverbatim


#include <vbl/vbl_ref_count.h>
#include <vsl/vsl_binary_io.h>
#include <bmrf/bmrf_node_sptr.h>
#include <bmrf/bmrf_arc_sptr.h>
#include <bmrf/bmrf_gamma_func_sptr.h>


//: Directed arc from one node to another
class bmrf_arc : public vbl_ref_count
{
 public:
  friend class bmrf_node;

  //: Constructor
  bmrf_arc();
  //: Copy constructor
  bmrf_arc(bmrf_arc const& a);
  //: Constructor
  bmrf_arc( const bmrf_node_sptr& f, const bmrf_node_sptr& t);
  //: Destructor
  ~bmrf_arc() {}

  //: Produce a new arc which is the reverse of this one efficiently
  bmrf_arc_sptr reverse() const;

  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  void b_read(vsl_b_istream &is);

  //: Return the probability of this arc
  double probability();

  //: Return the minimum alpha in common with both nodes
  double min_alpha() const { return min_alpha_; }

  //: Return the maximum alpha in common with both nodes
  double max_alpha() const { return max_alpha_; }

  //: Return the average intesity error
  double avg_intensity_error() const { return avg_intensity_error_; }

  //: Return the constant gamma value induced by the segment pair
  //  \note this maps the "from" arc to the "to" arc
  double induced_gamma() const { return gamma_; }

  //: Return the constant inverse gamma value induced by the segment pair
  //  \note this maps the "to" arc to the "from" arc
  double induced_gamma_inv() const { return inv_gamma_; }

  //: Return the piecewise linear gamma function fit to the pair
  bmrf_gamma_func_sptr gamma_func();

  //: Return the average match error given the induced gamma
  double induced_match_error() const { return induced_match_error_; }

  //: The change in time spanned by this arc
  int time_step() const;

  //: Smart pointer to the node where this arc originates
  bmrf_node_sptr from() { return bmrf_node_sptr(from_); }

  //: Smart pointer to the node where this arc ends
  bmrf_node_sptr to() { return bmrf_node_sptr(to_); }

  //: Compute the alpha range and intensity comparison
  // \note vertices must be set
  void time_init();

 private:
  bmrf_node* from_;
  bmrf_node* to_;

  double probability_;
  double min_alpha_, max_alpha_;

  double gamma_, inv_gamma_;
  double avg_intensity_error_;
  double induced_match_error_;

  bmrf_gamma_func_sptr gamma_func_;
};


//: Binary save bmrf_arc* to stream
void vsl_b_write(vsl_b_ostream &os, const bmrf_arc* a);

//: Binary load bmrf_arc* from stream.
void vsl_b_read(vsl_b_istream &is, bmrf_arc* &a);

//: Print an ASCII summary to the stream
void vsl_print_summary(vcl_ostream &os, const bmrf_arc* a);

#endif // bmrf_arc_h_
