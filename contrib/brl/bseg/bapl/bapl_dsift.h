//this is /brl/bseg/bapl/bapl_dsift.h
#ifndef BAPL_DSIFT_H_
#define BAPL_DSIFT_H_
//:
// \file
// \brief Extract SIFT features at specific image locations. This is
// different than bapl_dense_sift in that it ignores scale space optimization,
// the descriptor is computed at the scale of the image. Additionally, the
// dominant gradient direction is not computed and an orientation can be specified
// before a descriptor is extracted.
// \author Brandon Mayer (b.mayer1@gmail.com)
// \date March 7, 2011
//
// \verbatim
//  Modifications
// \endverbatim

//Ignores Scale and Orientation computations and produces a sift descriptor at the scale of the image, 
//specified orientation and location.
#include<vbl/vbl_ref_count.h>

#include<vcl_cmath.h>

#include<vgl/vgl_point_2d.h>

#include<vil/vil_image_view.h>

#include<vnl/vnl_vector.h>

class bapl_dsift: public vbl_ref_count
{
public:
	bapl_dsift():grad_valid_(false){}
	bapl_dsift( vil_image_view<float> const& img );
	~bapl_dsift(){}

	bool set_img( vil_image_view<float> const& img );

	vcl_vector<float> dsift( unsigned const& key_x, unsigned const& key_y, float const& key_orient = 0.0f );

	vnl_vector<double> vnl_dsift( unsigned const& key_x, unsigned const& key_y, float const& key_orient = 0.0f );

protected:
	vil_image_view<float> grad_mag_;
	vil_image_view<float> grad_orient_;
	bool grad_valid_;

	static float gaussian( float const& x, float const& y ){return vcl_exp(-((x*x)+(y*y))/(128.0f));}
	
};//end bapl_dsift

#endif //BAPL_DSIFT_H_