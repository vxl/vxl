#ifndef bdgl_curve_matcher_h_
#define bdgl_curve_matcher_h_

//-----------------------------------------------------------------------------
//:
// \file
// \author P.L. Bazin
// \brief matching of extracted vdgl_edgel_chains from images
//
// \verbatim
// Initial version February 12, 2003
// \endverbatim
//
//-----------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vdgl/vdgl_edgel_chain_sptr.h>
#include <bdgl/bdgl_curve_tracker_primitive.h>

class bdgl_curve_matcher_params
{
 public:
  // Parameters

  bool debug_;               //!< Carry out debug processing
  bool verbose_;             //!< Print detailed output
	double image_scale_;
	double grad_scale_;
	double angle_scale_;

  bdgl_curve_matcher_params(){}

  bdgl_curve_matcher_params(bool verbose, bool debug){verbose_=verbose; debug_=debug; }
  bdgl_curve_matcher_params(double is, double gs, double as){
		image_scale_=is; grad_scale_=gs; angle_scale_=as; }

	~bdgl_curve_matcher_params(){}

};


class bdgl_curve_matcher
{
 public:
	// Input : a curve from the current image, the primitive to match
	vdgl_edgel_chain_sptr							 image_curve_;
	bdgl_curve_tracker_primitive	 primitive_curve_;

	// Output : the matching values
	double	 matching_score_;					// confidence on matching
	int      matching_flag_;					// encode particular events

	// Params
	bdgl_curve_matcher_params params_;

  bdgl_curve_matcher(){}
  bdgl_curve_matcher(bdgl_curve_tracker_primitive	primitive, vdgl_edgel_chain_sptr curve){
		init(primitive, curve); }
  bdgl_curve_matcher(bdgl_curve_matcher_params params){ params_ = params; }
	~bdgl_curve_matcher(){}

  void init(bdgl_curve_tracker_primitive	primitive, vdgl_edgel_chain_sptr curve){
		primitive_curve_ = primitive; image_curve_ = curve; }

	bool ismatched(){ return (matching_flag_==1); }
	bool islost(){ return (matching_flag_==2); }

	double score(){ return matching_score_; }

	double image_scale(){ return params_.image_scale_; }
	double grad_scale(){ return params_.grad_scale_; }
	double angle_scale(){ return params_.angle_scale_; }

	void match();

};

#endif bdgl_curve_matcher_h_
