#ifndef bdgl_curve_tracker_h_
#define bdgl_curve_tracker_h_

//-----------------------------------------------------------------------------
//:
// \file
// \author P.L. Bazin
// \brief tracking of extracted vdgl_edgel_chains on sequences
//
// \verbatim
// Initial version February 12, 2003
// \endverbatim
//
//-----------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vdgl/vdgl_edgel_chain_sptr.h>
#include <vdgl/vdgl_digital_curve_sptr.h>
#include <bdgl/bdgl_curve_region.h>
#include <bdgl/bdgl_curve_matcher.h>
#include <bdgl/bdgl_curve_tracker_primitive.h>


//-----------------------------------------------------------------------------
class bdgl_curve_tracker_params
{
 public:
  // Parameters

  bool debug_;               //!< Carry out debug processing
  bool verbose_;             //!< Print detailed output

  double match_thres_;         // threshold on the allowed matching quality

  bdgl_curve_matcher_params match_params_;

  bdgl_curve_tracker_params(){}

  bdgl_curve_tracker_params(bool verbose, bool debug){verbose_=verbose; debug_=debug; }
  bdgl_curve_tracker_params(double th){ match_thres_ = th; }

  ~bdgl_curve_tracker_params(){}
};


//-----------------------------------------------------------------------------
class bdgl_curve_tracker
{
 public:
  // Input : for each image, for each
  vcl_vector< vcl_vector< vdgl_edgel_chain_sptr > > input_curve_;

  // Output
  vcl_vector< vcl_vector< bdgl_curve_tracker_primitive > > output_curve_;

  // Params
  bdgl_curve_tracker_params params_;

  bdgl_curve_tracker(){}
  bdgl_curve_tracker(bdgl_curve_tracker_params p){ params_ = p; }
  ~bdgl_curve_tracker(){}
  vcl_vector< vcl_vector< vdgl_edgel_chain_sptr > > get_input(){ return input_curve_; }
  void set_input(vcl_vector< vcl_vector< vdgl_edgel_chain_sptr > > curve){ input_curve_=curve; }

  vcl_vector< vcl_vector< bdgl_curve_tracker_primitive > > get_output(){ return output_curve_; }
  void set_output(vcl_vector< vcl_vector< bdgl_curve_tracker_primitive > > curve){ output_curve_=curve; }

	vdgl_edgel_chain_sptr get_output_curve_at( int frame, int num )
		{ return output_curve_[frame][num].get_curve(); }
	int get_output_id_at( int frame, int num )
		{ return output_curve_[frame][num].get_id(); }
	int get_output_size_at( int frame )
		{ return output_curve_[frame].size(); }

  void track();
	void track_frame(int frame);
};

//-----------------------------------------------------------------------------

#endif // bdgl_curve_tracker_h_
