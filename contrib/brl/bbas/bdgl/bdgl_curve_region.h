#ifndef bdgl_curve_region_h_
#define bdgl_curve_region_h_

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
#include <vgl/vgl_point_2d.h>
#include <vdgl/vdgl_edgel_chain_sptr.h>



//-----------------------------------------------------------------------------
class bdgl_curve_region
{
public:

	vgl_point_2d<double> center_;
	double							 radius_;

	bdgl_curve_region(){}
	bdgl_curve_region(vdgl_edgel_chain_sptr curve){ init(curve); }
	~bdgl_curve_region(){}

	void init(vdgl_edgel_chain_sptr curve);

	vgl_point_2d<double> get_center(){ return center_ ;}
	double get_radius(){ return radius_ ;}
	double x(){ return center_.x() ;}
	double y(){ return center_.y() ;}
	double r(){ return radius_ ;}

};


#endif bdgl_curve_region_h_
