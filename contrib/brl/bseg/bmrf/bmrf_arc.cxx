// This is brl/bseg/bmrf/bmrf_arc.cxx
#include "bmrf_arc.h"
//:
// \file

#include <vsl/vsl_binary_io.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <bmrf/bmrf_node.h>
#include <bmrf/bmrf_epi_seg_compare.h>
#include <bmrf/bmrf_gamma_func.h>
#include <bmrf/bmrf_epi_transform.h>


//: Constructor
bmrf_arc::bmrf_arc()
  : from_(NULL), to_(NULL), probability_(-1.0),
    min_alpha_(0.0), max_alpha_(0.0),
    gamma_(0.0), inv_gamma_(0.0),
    avg_intensity_error_(0.0), induced_match_error_(0.0),
    gamma_func_(NULL)
{
}


//: Copy constructor
bmrf_arc::bmrf_arc(bmrf_arc const& a)
  : vbl_ref_count(), from_(a.from_), to_(a.to_), probability_(a.probability_),
    min_alpha_(a.min_alpha_), max_alpha_(a.max_alpha_),
    gamma_(a.gamma_), inv_gamma_(a.inv_gamma_),
    avg_intensity_error_(a.avg_intensity_error_), induced_match_error_(a.induced_match_error_),
    gamma_func_(NULL)
{
}


//: Constructor
bmrf_arc::bmrf_arc( const bmrf_node_sptr& f, const bmrf_node_sptr& t)
  : from_(f.ptr()), to_(t.ptr()), probability_(-1.0),
    min_alpha_(0.0), max_alpha_(0.0),
    gamma_(0.0), inv_gamma_(0.0),
    avg_intensity_error_(0.0), induced_match_error_(0.0),
    gamma_func_(NULL)
{
  if ( from_ && to_ ) {
    if (from_->frame_num() != to_->frame_num())
      time_init();
  }
}


//: Produce a new arc which is the reverse of this one efficiently
bmrf_arc_sptr
bmrf_arc::reverse() const
{
  bmrf_arc_sptr rev_arc = new bmrf_arc(*this);
  rev_arc->to_ = this->from_;
  rev_arc->from_ = this->to_;
  rev_arc->probability_ = -1.0;
  rev_arc->gamma_ = this->inv_gamma_;
  rev_arc->inv_gamma_ = this->gamma_;
  rev_arc->gamma_func_ = NULL;

  double t = double(rev_arc->time_step());

  bmrf_epi_seg_sptr ep1 = rev_arc->from_->epi_seg();
  bmrf_epi_seg_sptr ep2 = rev_arc->to_->epi_seg();
  bmrf_gamma_func_sptr gamma_inv = new bmrf_const_gamma_func(rev_arc->inv_gamma_);
  bmrf_epi_seg_sptr xform_seg = bmrf_epi_transform(ep2, gamma_inv, -t);
  rev_arc->induced_match_error_ = bmrf_match_error(ep1, xform_seg);

  return rev_arc;
}


//: Return the probability of this arc
double
bmrf_arc::probability()
{
  if (from_->probability_ < 0.0)
    from_->compute_probability();
  return probability_;
}


//: The change in time spanned by this arc
int
bmrf_arc::time_step() const
{
  if ( !to_ || !from_ )
    return 0;
  return to_->frame_num() - from_->frame_num();
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
  if ( from_->epi_seg()->n_pts() <= 0 ||
       to_->epi_seg()->n_pts() <= 0 )
    return;

  bmrf_epi_seg_sptr ep1 = from_->epi_seg();
  bmrf_epi_seg_sptr ep2 = to_->epi_seg();

  min_alpha_ = bmrf_min_alpha(ep1, ep2);
  max_alpha_ = bmrf_max_alpha(ep1, ep2);

  avg_intensity_error_ = bmrf_intensity_error(ep1, ep2);

  int t = time_step();
  double dist_ratio = bmrf_avg_distance_ratio(ep1, ep2);
  gamma_ = (1.0 - dist_ratio) / t;

  dist_ratio = bmrf_avg_distance_ratio(ep2, ep1);
  inv_gamma_ = (1.0 - dist_ratio) / -t;

  bmrf_gamma_func_sptr gamma_inv = new bmrf_const_gamma_func(inv_gamma_);
  bmrf_epi_seg_sptr xform_seg = bmrf_epi_transform(ep2, gamma_inv, double(-t));
  induced_match_error_ = bmrf_match_error(ep1, xform_seg);
}


//: Return the piecewise linear gamma function fit to the pair
bmrf_gamma_func_sptr 
bmrf_arc::gamma_func(){ 
  if(!gamma_func_)
    gamma_func_ = new bmrf_pwl_gamma_func( from_->epi_seg(), 
                                           to_->epi_seg(), 
                                           double(time_step()) );
  return gamma_func_; 
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

