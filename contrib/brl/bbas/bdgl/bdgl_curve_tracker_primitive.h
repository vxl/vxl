#ifndef bdgl_curve_tracker_primitive_h_
#define bdgl_curve_tracker_primitive_h_

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
#include <bdgl/bdgl_curve_region.h>



//-----------------------------------------------------------------------------
class bdgl_curve_tracker_primitive
{
 public:
  // Parameters

	int id_;

	vdgl_edgel_chain_sptr curve_;

	bdgl_curve_region	region_;

	int num_;
	int prev_num_;
	int next_num_;

  bdgl_curve_tracker_primitive(){}
	bdgl_curve_tracker_primitive(int id, vdgl_edgel_chain_sptr curve){ init(id, curve); }
	~bdgl_curve_tracker_primitive(){}

	void init(int id, vdgl_edgel_chain_sptr curve);

	int get_id(){ return id_; }
	void set_id(int id){ id_ = id; }

	vdgl_edgel_chain_sptr get_curve(){ return curve_; }
	void set_curve(vdgl_edgel_chain_sptr curve){ curve_=curve; }

	void set_region(bdgl_curve_region r){ region_ = r; }
	bdgl_curve_region region(){ return region_; }

	int get_num(){ return num_; }
	void set_num(int num){ num_=num; } 
	int get_prev_num(){ return prev_num_; }
	void set_prev_num(int num){ prev_num_=num; }
	int get_next_num(){ return next_num_; }
	void set_next_num(int num){ next_num_=num; }
};


#endif bdgl_curve_tracker_primitive_h_
