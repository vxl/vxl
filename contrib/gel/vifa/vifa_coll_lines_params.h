// This is contrib/gel/vifa/vifa_coll_lines_params.h
#ifndef VIFA_COLL_LINES_PARAMS_H
#define VIFA_COLL_LINES_PARAMS_H

//-----------------------------------------------------------------------------
//:
// \file
// \brief Parameter mixin for collinearity tests.
//
// \author Roddy Collins, from DDB in TargetJr
//
// \date May 2001
//
// \verbatim
//  Modifications:
//   MPP Mar 2003, Ported to VXL
// \endverbatim
//-----------------------------------------------------------------------------

#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
#include <vul/vul_timestamp.h>
#include <gevd/gevd_param_mixin.h>


//: Container holding parameters for collinearizing lines
class vifa_coll_lines_params : public gevd_param_mixin,
							   public vul_timestamp,
							   public vbl_ref_count
{
public:
	//: Distance of midpoint of qualified edge segment to projected line.
	float _midpt_distance;

	//: Maximum angluar difference (in degrees).
	float _angle_tolerance;

	//: Minimum of supported to spanning length.
	float _discard_threshold;

	//: Endpoints within this distance are accepted no matter what the angular difference is.
	float _endpt_distance;

	//: Default constructor
	vifa_coll_lines_params(float midpt_distance = 1.0,
						   float angle_tolerance = 5.0,
						   float discard_threshold = 0.3,
						   float endpt_distance = 3.0
						  );

	//: Copy constructor
	vifa_coll_lines_params(const vifa_coll_lines_params& old_params);

	//: Output contents
	void	print_info(void);
};


typedef vbl_smart_ptr<vifa_coll_lines_params>	vifa_coll_lines_params_sptr;


#endif	// VIFA_COLL_LINES_PARAMS_H
