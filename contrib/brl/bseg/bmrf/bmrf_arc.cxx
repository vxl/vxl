// This is brl/bseg/bmrf/bmrf_arc.cxx
#include "bmrf_arc.h"
//:
// \file

#include <vcl_algorithm.h>
#include <vsl/vsl_binary_io.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <bmrf/bmrf_node.h>


//: Constructor
bmrf_arc::bmrf_arc()
  : from_(NULL), to_(NULL), probability_(-1.0),
    min_alpha_(0.0), max_alpha_(0.0), avg_intensity_error_(0.0)
{
}


//: Constructor
bmrf_arc::bmrf_arc( const bmrf_node_sptr& f, const bmrf_node_sptr& t)
  : from_(f.ptr()), to_(t.ptr()), probability_(-1.0),
    min_alpha_(0.0), max_alpha_(0.0), avg_intensity_error_(0.0)
{
  if ( from_ && to_ ) {
    if (from_->frame_num() != to_->frame_num())
      time_init();
  }
}

//: Return the probability of this arc
double 
bmrf_arc::probability()
{
  if(probability_ < 0.0)
    from_->compute_probability();
  return probability_;
}


//: Binary save bmrf_arc to stream.
void
bmrf_arc::b_write( vsl_b_ostream& ) const
{
  // Nothing to write
}


//: Binary load bmrf_arc from stream.
void
bmrf_arc::b_read( vsl_b_istream& )
{
  // Nothing to read
}


//: Compute the alpha range and intensity comparison
void
bmrf_arc::time_init()
{
  if( from_->epi_seg()->n_pts() <= 0 || 
      to_->epi_seg()->n_pts() <= 0 )
    return;

  bmrf_epi_seg_sptr ep1 = from_->epi_seg();
  bmrf_epi_seg_sptr ep2 = to_->epi_seg();

  min_alpha_ = vcl_max(ep1->min_alpha(), ep2->min_alpha());
  max_alpha_ = vcl_min(ep1->max_alpha(), ep2->max_alpha());
  double alpha_range = max_alpha_ - min_alpha_;
  double d_alpha = vcl_min((ep1->max_alpha() - ep1->min_alpha())/ep1->n_pts() ,
                           (ep2->max_alpha() - ep2->min_alpha())/ep2->n_pts() );

  double l_error = 0.0, r_error = 0.0;
  for (double alpha = min_alpha_; alpha <= max_alpha_; alpha += d_alpha)
  {
    double dli = (ep1->left_int(alpha) - ep2->left_int(alpha));
    double dri = (ep1->right_int(alpha) - ep2->right_int(alpha));
    l_error += dli*dli;
    r_error += dri*dri;
  }
  avg_intensity_error_ = (l_error + r_error) * d_alpha / alpha_range;
}


//-----------------------------------------------------------------------------------------
// External functions
//-----------------------------------------------------------------------------------------



//: Binary save bmrf_arc \p a to stream.
void
vsl_b_write(vsl_b_ostream &os, const bmrf_arc* a)
{
  if (a==0) {
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else{
    vsl_b_write(os,true); // Indicate non-null pointer stored
    a->b_write(os);
  }
}


//: Binary load bmrf_arc \p a from stream.
void
vsl_b_read(vsl_b_istream &is, bmrf_arc* &a)
{
  delete a;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr) {
    a = new bmrf_arc();
    a->b_read(is);
  }
  else
    a = 0;
}


//: Print an ASCII summary of a bmrf_arc to the stream (NYI)
void
vsl_print_summary(vcl_ostream &os, const bmrf_arc* )
{
  os << "bmrf_arc{}";
}

