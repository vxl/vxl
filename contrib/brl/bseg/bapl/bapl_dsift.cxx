//this is /brl/bseg/bapl/bapl_dsift.cxx
#include"bapl_dsift.h"

#include<vcl_cassert.h>

#include<vil/algo/vil_orientations.h>

#include<vnl/vnl_math.h>

//inline float gaussian( float const& x, float const& y )
//{
//	return vcl_exp(-((x*x)+(y*y))/(128.0f));
//}

bapl_dsift::bapl_dsift( vil_image_view<float> const& img ):grad_valid_(true)
{
	vil_orientations_from_sobel( img, this->grad_orient_, this->grad_mag_ );
}//end bapl_dsift::bapl_dsift

bool bapl_dsift::set_img( vil_image_view<float> const& img )
{
	vil_orientations_from_sobel( img, this->grad_orient_, this->grad_mag_ );
	this->grad_valid_ = true;
	return this->grad_valid_;
}

//float bapl_dsift::gaussian( float const& x, float const& y )
//{
//	return vcl_exp(-((x*x)+(y*y))/(128.0f));
//}

vcl_vector<float> bapl_dsift::dsift( unsigned const& key_x, unsigned const& key_y, float const& key_orient )
{
	vcl_vector<float> histogram(128,0.0f);

	assert(this->grad_valid_);

	for (int hi=0; hi<4; ++hi)
	{
		for (int hj=0; hj<4; ++hj)
		{
			for (int i=4*hi; i<4*(hi+1); ++i) 
			{
				for (int j=4*hj; j<4*(hj+1); ++j)
				{
					double x = ( (i-7.5)*vcl_cos(key_orient)
						-(j-7.5)*vcl_sin(key_orient));
					double y = ( (i-7.5)*vcl_sin(key_orient)
						+(j-7.5)*vcl_cos(key_orient));

					for (int c=0; c<4; ++c)
					{
						int xc = int(x+key_x) + c/2;
						int yc = int(y+key_y) + c%2;

						if ( xc>=0 && xc<int(this->grad_orient_.ni()) &&
							yc>=0 && yc<int(this->grad_orient_.nj()) )
						{
							float interp_x = 1.0f - vcl_fabs( x+key_x - float(xc) );
							float interp_y = 1.0f - vcl_fabs( y+key_y - float(yc) );

							float weight = this->grad_mag_(xc,yc) * interp_x * interp_y * bapl_dsift::gaussian((xc-key_x), (yc-key_y));

							float orient = this->grad_orient_(xc,yc)-key_orient+vnl_math::pi;

							while (orient > float(2*vnl_math::pi)) orient -= float(2*vnl_math::pi);
							while (orient < 0.0f)                  orient += float(2*vnl_math::pi);

							int bin = ((int(orient*15/float(2*vnl_math::pi))+1)/2)%8;
							histogram[hi*32+hj*8+bin] += weight;

						}//end boundary check
					}//end c
				}//end j
			}//end i
		}//end hj
	}//end hi
	
	return histogram;
}

vnl_vector<double> bapl_dsift::vnl_dsift( unsigned const& key_x, unsigned const& key_y, float const& key_orient )
{
	vnl_vector<double> histogram(128,0.0f);

	assert(this->grad_valid_);

	for (int hi=0; hi<4; ++hi)
	{
		for (int hj=0; hj<4; ++hj)
		{
			for (int i=4*hi; i<4*(hi+1); ++i) 
			{
				for (int j=4*hj; j<4*(hj+1); ++j)
				{
					double x = ( (i-7.5)*vcl_cos(key_orient)
						-(j-7.5)*vcl_sin(key_orient));
					double y = ( (i-7.5)*vcl_sin(key_orient)
						+(j-7.5)*vcl_cos(key_orient));

					for (int c=0; c<4; ++c)
					{
						int xc = int(x+key_x) + c/2;
						int yc = int(y+key_y) + c%2;

						//vcl_cout << "(xc,yc) = " << xc << ", " << yc << ";" << vcl_endl;

						if ( xc>=0 && xc<int(this->grad_orient_.ni()) &&
							yc>=0 && yc<int(this->grad_orient_.nj()) )
						{
							float interp_x = 1.0f - vcl_fabs( x+key_x - float(xc) );
							float interp_y = 1.0f - vcl_fabs( y+key_y - float(yc) );

							//int diff_x = xc-int(key_x);
							//int diff_y = yc-int(key_y);

							float gw = bapl_dsift::gaussian((xc-float(key_x)), (yc-float(key_y)));

							float weight = this->grad_mag_(xc,yc) * interp_x * interp_y * bapl_dsift::gaussian((xc-float(key_x)), (yc-float(key_y)));

							float orient = this->grad_orient_(xc,yc)-key_orient+vnl_math::pi;

							while (orient > float(2*vnl_math::pi)) orient -= float(2*vnl_math::pi);
							while (orient < 0.0f)                  orient += float(2*vnl_math::pi);

							int bin = ((int(orient*15/float(2*vnl_math::pi))+1)/2)%8;
							histogram[hi*32+hj*8+bin] += weight;

						}//end boundary check
					}//end c
				}//end j
			}//end i
		}//end hj
	}//end hi
	
	return histogram;
}//end dsift