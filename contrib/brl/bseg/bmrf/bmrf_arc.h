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

//: Directed arc from one node to another
class bmrf_arc : public vbl_ref_count
{
 public:
  friend class bmrf_node;

  //: Constructor
  bmrf_arc();
  //: Constructor
  bmrf_arc( const bmrf_node_sptr& f, const bmrf_node_sptr& t);
  //: Destructor
  ~bmrf_arc() {}

  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  void b_read(vsl_b_istream &is);

  double probability() { return probability_; }

  double avg_intensity_error() { return avg_intensity_error_; }

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
  double avg_intensity_error_;
};


//: Binary save bmrf_arc* to stream
void vsl_b_write(vsl_b_ostream &os, const bmrf_arc* a);

//: Binary load bmrf_arc* from stream.
void vsl_b_read(vsl_b_istream &is, bmrf_arc* &a);

//: Print an ASCII summary to the stream
void vsl_print_summary(vcl_ostream &os, const bmrf_arc* a);

#endif // bmrf_arc_h_
