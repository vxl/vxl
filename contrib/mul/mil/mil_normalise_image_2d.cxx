// This is mul/mil/mil_normalise_image_2d.cxx
#include "mil_normalise_image_2d.h"
//:
//  \file
//  \brief Functions to normalise 2d float plane images
//  \author Tim Cootes

#include <vcl_cmath.h>
#include <mil/mil_image_2d_of.h>
#include <vnl/vnl_vector.h>

//: creates a variance normalised version of an input image
void mil_var_norm_image_2d(mil_image_2d_of<float>& dest,
                           const mil_image_2d_of<float>& src)
{
  int n = src.n_planes();
  int nx = src.nx();
  int ny = src.ny();
  dest.resize(nx,ny,n);
  int sxstep=src.xstep();
  int systep=src.ystep();
  int dxstep=dest.xstep();
  int dystep=dest.ystep();

  vnl_vector<float> mean(n), std(n);

  // calculate the mean and variance of the src image
  for (int i=0;i<n;++i)
  {
    float sum=0, sqr_sum=0;
    const float* s_row = src.plane(i);

    for (int y=0;y<ny;++y)
    {
      const float* s = s_row;
      for (int x=0;x<nx;++x)
      {
        sum += *s;
        sqr_sum += (*s) * (*s);
        s+=sxstep;
      }

      s_row += systep;
    }

    // calc mean and variance for this plane
    int np=nx*ny;
    mean(i)= sum/np;
    std(i) = vcl_sqrt ( sqr_sum/np - mean(i)*mean(i) );
    //vcl_cout<<"std("<<i<<")= "<<std(i)<<'\n';
  }


  // calculate the normalised image
  for (int i=0;i<n;++i)
  {
    const float* s_row = src.plane(i);
    float* d_row = dest.plane(i);

    for (int y=0;y<ny;++y)
    {
      const float* s = s_row;
      float * d = d_row;

      for (int x=0;x<nx;++x)
      {
        if ( std(i)==0 )
          *d = 0;
        else
          *d = ( (*s)-mean(i) )*1.0f/std(i);
        s+=sxstep;
        d+=dxstep;
      }

      s_row += systep;
      d_row += dystep;
    }
  }
  dest.setWorld2im(src.world2im());

  //vcl_cout<<"dest=\n";
  //dest.print_messy_all(vcl_cout);
}

//: creates a mean normalised version of an input image
void mil_mean_norm_image_2d(mil_image_2d_of<float>& dest,
                            const mil_image_2d_of<float>& src)
{
  int n = src.n_planes();
  int nx = src.nx();
  int ny = src.ny();
  dest.resize(nx,ny,n);
  int sxstep=src.xstep();
  int systep=src.ystep();
  int dxstep=dest.xstep();
  int dystep=dest.ystep();

  vnl_vector<float> mean(n);

  // calculate the mean and variance of the src image
  for (int i=0;i<n;++i)
  {
    float sum=0;
    const float* s_row = src.plane(i);

    for (int y=0;y<ny;++y)
    {
      const float* s = s_row;
      for (int x=0;x<nx;++x)
      {
        sum += *s;
        s+=sxstep;
      }

      s_row += systep;
    }

    // calc mean and variance for this plane
    int np=nx*ny;
    mean(i)= sum/np;
  }


  // calculate the normalised image
  for (int i=0;i<n;++i)
  {
    const float* s_row = src.plane(i);
    float* d_row = dest.plane(i);

    for (int y=0;y<ny;++y)
    {
      const float* s = s_row;
      float * d = d_row;

      for (int x=0;x<nx;++x)
      {
        *d = (*s)-mean(i);
        s+=sxstep;
        d+=dxstep;
      }

      s_row += systep;
      d_row += dystep;
    }
  }
  dest.setWorld2im(src.world2im());
}
