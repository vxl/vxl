//-----------------------------------------------------------------------------
//:
// \file
// \author P.L. Bazin
//-----------------------------------------------------------------------------
#include <vdgl/vdgl_edgel_chain_sptr.h>
#include <bdgl/bdgl_curve_tracker_primitive.h>
#include <bdgl/bdgl_curve_region.h>



//-----------------------------------------------------------------------------
void bdgl_curve_tracker_primitive::init(int id, vdgl_edgel_chain_sptr curve)
{
	curve_ = curve; 
	id_ = id;
	bdgl_curve_region	reg(curve);
	region_ = reg; 
	num_ = id; 
	prev_num_ = -1; 
	next_num_ = -1; 

	return;
}

//-----------------------------------------------------------------------------
