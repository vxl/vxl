//this is /brl/bseg/bapl/bapl_dsift.cxx
#include "bapl_dsift.h"

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/algo/vil_orientations.h>
#include <vnl/vnl_math.h>

bapl_dsift::bapl_dsift( vil_image_view<float> const& img )
: grad_valid_(true)
{
  vil_orientations_from_sobel( img, this->grad_orient_, this->grad_mag_ );
}//end bapl_dsift::bapl_dsift

bapl_dsift::bapl_dsift( vil_image_view<vxl_byte> const& img )
: grad_valid_(true)
{
  vil_orientations_from_sobel( img, this->grad_orient_, this->grad_mag_ );
}//end bapl_dsift::bapl_dsift

bool bapl_dsift::set_img( vil_image_view<vxl_byte> const& img )
{
  vil_orientations_from_sobel( img, this->grad_orient_, this->grad_mag_ );
  this->grad_valid_ = true;
  return this->grad_valid_;
}

bool bapl_dsift::set_img( vil_image_view<float> const& img )
{
  vil_orientations_from_sobel( img, this->grad_orient_, this->grad_mag_ );
  this->grad_valid_ = true;
  return this->grad_valid_;
}

std::vector<float> bapl_dsift::dsift( unsigned const& key_x, unsigned const& key_y, float const& key_orient )
{
  assert(this->grad_valid_);
  std::vector<float> histogram(128,0.0f);

  for (int hi=0; hi<4; ++hi)
  {
    for (int hj=0; hj<4; ++hj)
    {
      for (int i=4*hi; i<4*(hi+1); ++i)
      {
        for (int j=4*hj; j<4*(hj+1); ++j)
        {
          double x = ( (i-7.5)*std::cos(key_orient)
                      -(j-7.5)*std::sin(key_orient));
          double y = ( (i-7.5)*std::sin(key_orient)
                      +(j-7.5)*std::cos(key_orient));

          for (int c=0; c<4; ++c)
          {
            int xc = int(x+key_x) + c/2;
            int yc = int(y+key_y) + c%2;

            if ( xc>=0 && xc<int(this->grad_orient_.ni()) &&
                 yc>=0 && yc<int(this->grad_orient_.nj()) )
            {
              float interp_x = 1.0f - std::fabs( key_x + float(x-xc) );
              float interp_y = 1.0f - std::fabs( key_y + float(y-yc) );

              float weight = this->grad_mag_(xc,yc) * interp_x * interp_y * bapl_dsift::gaussian(float(xc-key_x), float(yc-key_y));

              auto orient = float(vnl_math::angle_0_to_2pi(this->grad_orient_(xc,yc)-key_orient+vnl_math::pi));

              int bin = ((int(orient*15/float(vnl_math::twopi))+1)/2)%8;
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
          double x = ( (i-7.5)*std::cos(key_orient)
                      -(j-7.5)*std::sin(key_orient));
          double y = ( (i-7.5)*std::sin(key_orient)
                      +(j-7.5)*std::cos(key_orient));

          for (int c=0; c<4; ++c)
          {
            int xc = int(x+key_x) + c/2;
            int yc = int(y+key_y) + c%2;

            //std::cout << "(xc,yc) = " << xc << ", " << yc << ';' << std::endl;

            if ( xc>=0 && xc<int(this->grad_orient_.ni()) &&
                 yc>=0 && yc<int(this->grad_orient_.nj()) )
            {
              float interp_x = 1.0f - std::fabs( key_x + float(x-xc) );
              float interp_y = 1.0f - std::fabs( key_y + float(y-yc) );

              //int diff_x = xc-int(key_x);
              //int diff_y = yc-int(key_y);

              float gw = bapl_dsift::gaussian((xc-float(key_x)), (yc-float(key_y)));

              float weight = this->grad_mag_(xc,yc) * interp_x * interp_y * gw;
              auto orient = float(vnl_math::angle_0_to_2pi(this->grad_orient_(xc,yc)-key_orient+vnl_math::pi));

              int bin = ((int(orient*15/float(vnl_math::twopi))+1)/2)%8;
              histogram[hi*32+hj*8+bin] += weight;
            }//end boundary check
          }//end c
        }//end j
      }//end i
    }//end hj
  }//end hi

  return histogram;
}//end dsift

void bapl_dsift::b_write(vsl_b_ostream& os) const
{
  constexpr short version_no = 1;
  vsl_b_write(os, version_no);
  vsl_b_write(os, this->grad_valid_);

  if (this->grad_valid_)
  {
    vsl_b_write(os, grad_mag_);
    vsl_b_write(os, grad_orient_);
  }
}//end bapl_dsift::b_write

void bapl_dsift::b_read(vsl_b_istream& is)
{
  if (!is) return;
  short v;
  vsl_b_read(is,v);
  switch (v)
  {
   case 1:
    vsl_b_read(is,this->grad_valid_);
    if (this->grad_valid_)
    {
      vsl_b_read(is,this->grad_mag_);
      vsl_b_read(is,this->grad_orient_);
    }
    break;
   default:
    std::cerr << "----I/O ERROR: bapl_dsift::b_read ----\n"
             << "   UNKNOWN VERSION NUMBER " << v << '\n';
    is.is().clear(std::ios::badbit); //set an unrecoverable IO error on stream
    return;
  }//end switch
}//end bapl_dsift::b_read
