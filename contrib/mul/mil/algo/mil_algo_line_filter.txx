// This is mul/mil/algo/mil_algo_line_filter.txx
#ifndef mil_algo_line_filter_txx_
#define mil_algo_line_filter_txx_
//: \file
//  \brief Find line-like structures in a 2D image
//  \author Tim Cootes

#include "mil_algo_line_filter.h"
#include <vcl_cassert.h>

//: Find line like structures in image (light lines on dark backgrounds)
//  On exit line_str contains line strength at each pixel,
//  line_dir contains value indicating direction [0,4]
//  0 = Undefined, 1 = horizontal, 2 = 45 degrees etc
//  This version looks for light lines on a dark background only.
template <class Type>
void mil_algo_line_filter<Type>::light_lines_3x3(mil_image_2d_of<unsigned char>& line_dir,
                         mil_image_2d_of<float>& line_str,
                         const mil_image_2d_of<Type>& image,
                         float edge_thresh)
{
  assert(image.n_planes()==1);
  int nx = image.nx();
  int ny = image.ny();
  int xstep = image.xstep();
  int ystep = image.ystep();

  line_dir.resize(nx,ny,1);
  line_str.resize(nx,ny,1);

  int d_xstep = line_dir.xstep();
  int d_ystep = line_dir.ystep();
  unsigned char* d_data  = line_dir.plane(0);
  int s_xstep = line_str.xstep();
  int s_ystep = line_str.ystep();
  float* s_data = line_str.plane(0);

  // Cannot calculate line strength in borders
  fill_row(d_data,d_xstep,nx,0);
  fill_row(d_data+(ny-1)*d_ystep,d_xstep,nx,0);
  fill_row(s_data,s_xstep,nx,0);
  fill_row(s_data+(ny-1)*s_ystep,s_xstep,nx,0);

  d_data += d_ystep;
  s_data += s_ystep;
  const Type* im_data = image.plane(0)+ystep+xstep;

  int nx1 = nx-1;
  int ny1 = ny-1;

  // Relative positions of points to be sampled
  int nxstep = -xstep;
  int nystep = -ystep;
  int xystep = xstep+ystep;
  int ynxstep = ystep-xstep;
  int xnystep = xstep-ystep;
  int nxnystep = -xstep-ystep;

  for (int y=1;y<ny1;++y)
  {
    unsigned char* d_row = d_data; d_row[0]=0; d_row+=d_xstep;
    float*    s_row = s_data; s_row[0]=0; s_row+=s_xstep;
    const Type* i_row = im_data;
    for (int x=1;x<nx1;++x)
    {
      // Evaluate the line strength at each orientation

      float f1 = float(i_row[nxstep])  +float(i_row[xstep]);
      float f2 = float(i_row[nxnystep])+float(i_row[xystep]);
      float f3 = float(i_row[nystep])  +float(i_row[ystep]);
      float f4 = float(i_row[ynxstep]) +float(i_row[xnystep]);

      // Look for highest value (ie bright line on dark background)
      unsigned char best_d = 1;
      float max_f = f1;
      if (f2>max_f) { best_d=2; max_f=f2;}
      if (f3>max_f) { best_d=3; max_f=f3;}
      if (f4>max_f) { best_d=4; max_f=f4;}

      float edge_s = 0.5f*max_f + (*i_row)/3.0f -(f1+f2+f3+f4)/6.0f;
      if (edge_s>edge_thresh)
      {
        *d_row = best_d;
        *s_row = edge_s;
      }
      else
      {
        *d_row=0; *s_row=0;
      }

      d_row+=d_xstep;
      s_row+=s_xstep;
      i_row+=xstep;
    }
    // Zero the last elements in the rows
    d_row[0]=0;
    s_row[0]=0;

    d_data += d_ystep;
    s_data += s_ystep;
    im_data += ystep;
  }
}

//: Find line like structures in image (dark lines on bright backgrounds)
//  On exit line_str contains line strength at each pixel,
//  line_dir contains value indicating direction [0,4]
//  0 = Undefined, 1 = horizontal, 2 = 45 degrees etc
template <class Type>
void mil_algo_line_filter<Type>::dark_lines_3x3(mil_image_2d_of<unsigned char>& line_dir,
                         mil_image_2d_of<float>& line_str,
                         const mil_image_2d_of<Type>& image,
                         float edge_thresh)
{
  assert(image.n_planes()==1);
  int nx = image.nx();
  int ny = image.ny();
  int xstep = image.xstep();
  int ystep = image.ystep();

  line_dir.resize(nx,ny,1);
  line_str.resize(nx,ny,1);

  int d_xstep = line_dir.xstep();
  int d_ystep = line_dir.ystep();
  unsigned char* d_data  = line_dir.plane(0);
  int s_xstep = line_str.xstep();
  int s_ystep = line_str.ystep();
  float* s_data = line_str.plane(0);

  // Cannot calculate line strength in borders
  fill_row(d_data,d_xstep,nx,0);
  fill_row(d_data+(ny-1)*d_ystep,d_xstep,nx,0);
  fill_row(s_data,s_xstep,nx,0);
  fill_row(s_data+(ny-1)*s_ystep,s_xstep,nx,0);

  d_data += d_ystep;
  s_data += s_ystep;
  const Type* im_data = image.plane(0)+ystep+xstep;

  int nx1 = nx-1;
  int ny1 = ny-1;

  // Relative positions of points to be sampled
  int nxstep = -xstep;
  int nystep = -ystep;
  int xystep = xstep+ystep;
  int ynxstep = ystep-xstep;
  int xnystep = xstep-ystep;
  int nxnystep = -xstep-ystep;

  for (int y=1;y<ny1;++y)
  {
    unsigned char* d_row = d_data; d_row[0]=0; d_row+=d_xstep;
    float*    s_row = s_data; s_row[0]=0; s_row+=s_xstep;
    const Type* i_row = im_data;
    for (int x=1;x<nx1;++x)
    {
      // Evaluate the line strength at each orientation

      float f1 = float(i_row[nxstep])  +float(i_row[xstep]);
      float f2 = float(i_row[nxnystep])+float(i_row[xystep]);
      float f3 = float(i_row[nystep])  +float(i_row[ystep]);
      float f4 = float(i_row[xnystep]) +float(i_row[ynxstep]);

      // Look for lowest value (ie dark line on light background)
      unsigned char best_d = 1;
      float min_f = f1;
      if (f2<min_f) { best_d=2; min_f=f2;}
      if (f3<min_f) { best_d=3; min_f=f3;}
      if (f4<min_f) { best_d=4; min_f=f4;}

      float edge_s = (f1+f2+f3+f4)/6.0f - 0.5f*min_f - (*i_row)/3.0f;
      if (edge_s>edge_thresh)
      {
        *d_row = best_d;
        *s_row = edge_s;
      }
      else
      {
        *d_row=0; *s_row=0;
      }

      d_row+=d_xstep;
      s_row+=s_xstep;
      i_row+=xstep;
    }
    // Zero the last elements in the rows
    d_row[0]=0;
    s_row[0]=0;

    d_data += d_ystep;
    s_data += s_ystep;
    im_data += ystep;
  }
}

//: Find line like structures in image (light lines on dark backgrounds)
//  On exit line_str contains line strength at each pixel,
//  line_dir contains value indicating direction [0,4]
//  0 = Undefined, 1 = horizontal, 2 = 45 degrees etc
//  This version looks for light lines on a dark background only.
template <class Type>
void mil_algo_line_filter<Type>::light_lines_5x5(mil_image_2d_of<unsigned char>& line_dir,
                         mil_image_2d_of<float>& line_str,
                         const mil_image_2d_of<Type>& image,
                         float edge_thresh)
{
  assert(image.n_planes()==1);
  int nx = image.nx();
  int ny = image.ny();
  int xstep = image.xstep();
  int ystep = image.ystep();

  line_dir.resize(nx,ny,1);
  line_str.resize(nx,ny,1);

  int d_xstep = line_dir.xstep();
  int d_ystep = line_dir.ystep();
  unsigned char* d_data  = line_dir.plane(0);
  int s_xstep = line_str.xstep();
  int s_ystep = line_str.ystep();
  float* s_data = line_str.plane(0);

  // Cannot calculate line strength in borders
  fill_row(d_data,d_xstep,nx,0);
  fill_row(d_data+d_ystep,d_xstep,nx,0);
  fill_row(d_data+(ny-1)*d_ystep,d_xstep,nx,0);
  fill_row(d_data+(ny-2)*d_ystep,d_xstep,nx,0);
  fill_row(s_data,s_xstep,nx,0);
  fill_row(s_data+s_ystep,s_xstep,nx,0);
  fill_row(s_data+(ny-1)*s_ystep,s_xstep,nx,0);
  fill_row(s_data+(ny-2)*s_ystep,s_xstep,nx,0);

  d_data += 2*d_ystep;
  s_data += 2*s_ystep;
  const Type* im_data = image.plane(0)+2*(ystep+xstep);

  int nx2 = nx-2;
  int ny2 = ny-2;

  // Relative positions of points to be sampled
  int i1a = -2*xstep;
  int i1b = -xstep;
  int i1c = xstep;
  int i1d = 2*xstep;

  int i2c = xstep+ystep;
  int i2a = -2*i2c;
  int i2b = -1*i2c;
  int i2d = 2*i2c;

  int i3a = -2*ystep;
  int i3b = -1*ystep;
  int i3c = ystep;
  int i3d = 2*ystep;

  int i4c = xstep-ystep;
  int i4a = -2*i4c;
  int i4b = -1*i4c;
  int i4d = 2*i4c;

  for (int y=2;y<ny2;++y)
  {
    unsigned char* d_row = d_data; d_row[0]=0; d_row+=d_xstep;d_row[0]=0; d_row+=d_xstep;
    float*    s_row = s_data; s_row[0]=0; s_row+=s_xstep;s_row[0]=0; s_row+=s_xstep;
    const Type* i_row = im_data;
    for (int x=2;x<nx2;++x)
    {
      // Evaluate the line strength at each orientation

      float f1 = float(i_row[i1a])+float(i_row[i1b])+float(i_row[i1c])+float(i_row[i1d]);
      float f2 = float(i_row[i2a])+float(i_row[i2b])+float(i_row[i2c])+float(i_row[i2d]);
      float f3 = float(i_row[i3a])+float(i_row[i3b])+float(i_row[i3c])+float(i_row[i3d]);
      float f4 = float(i_row[i4a])+float(i_row[i4b])+float(i_row[i4c])+float(i_row[i4d]);

      // Look for highest value (ie bright line on dark background)
      unsigned char best_d = 1;
      float max_f = f1;
      if (f2>max_f) { best_d=2; max_f=f2;}
      if (f3>max_f) { best_d=3; max_f=f3;}
      if (f4>max_f) { best_d=4; max_f=f4;}

        // Average on line - average off line
      float edge_s = (17.0f/60) * max_f + 0.2f*(*i_row) -(f1+f2+f3+f4)/12;
      if (edge_s>edge_thresh)
      {
        *d_row = best_d;
        *s_row = edge_s;
      }
      else
      {
        *d_row=0; *s_row=0;
      }

      d_row+=d_xstep;
      s_row+=s_xstep;
      i_row+=xstep;
    }
    // Zero the last elements in the rows
    d_row[0]=0; d_row[d_xstep]=0;
    s_row[0]=0; s_row[s_xstep]=0;

    d_data += d_ystep;
    s_data += s_ystep;
    im_data += ystep;
  }
}

//: Find line like structures in image (dark lines on light backgrounds)
template <class Type>
void mil_algo_line_filter<Type>::dark_lines_5x5(mil_image_2d_of<unsigned char>& line_dir,
                         mil_image_2d_of<float>& line_str,
                         const mil_image_2d_of<Type>& image,
                         float edge_thresh)
{
  assert(image.n_planes()==1);
  int nx = image.nx();
  int ny = image.ny();
  int xstep = image.xstep();
  int ystep = image.ystep();

  line_dir.resize(nx,ny,1);
  line_str.resize(nx,ny,1);

  int d_xstep = line_dir.xstep();
  int d_ystep = line_dir.ystep();
  unsigned char* d_data  = line_dir.plane(0);
  int s_xstep = line_str.xstep();
  int s_ystep = line_str.ystep();
  float* s_data = line_str.plane(0);

  // Cannot calculate line strength in borders
  fill_row(d_data,d_xstep,nx,0);
  fill_row(d_data+d_ystep,d_xstep,nx,0);
  fill_row(d_data+(ny-1)*d_ystep,d_xstep,nx,0);
  fill_row(d_data+(ny-2)*d_ystep,d_xstep,nx,0);
  fill_row(s_data,s_xstep,nx,0);
  fill_row(s_data+s_ystep,s_xstep,nx,0);
  fill_row(s_data+(ny-1)*s_ystep,s_xstep,nx,0);
  fill_row(s_data+(ny-2)*s_ystep,s_xstep,nx,0);

  d_data += 2*d_ystep;
  s_data += 2*s_ystep;
  const Type* im_data = image.plane(0)+2*(ystep+xstep);

  int nx2 = nx-2;
  int ny2 = ny-2;

  // Relative positions of points to be sampled
  int i1a = -2*xstep;
  int i1b = -xstep;
  int i1c = xstep;
  int i1d = 2*xstep;

  int i2c = xstep+ystep;
  int i2a = -2*i2c;
  int i2b = -1*i2c;
  int i2d = 2*i2c;

  int i3a = -2*ystep;
  int i3b = -1*ystep;
  int i3c = ystep;
  int i3d = 2*ystep;

  int i4c = xstep-ystep;
  int i4a = -2*i4c;
  int i4b = -1*i4c;
  int i4d = 2*i4c;

  for (int y=2;y<ny2;++y)
  {
    unsigned char* d_row = d_data; d_row[0]=0; d_row+=d_xstep;d_row[0]=0; d_row+=d_xstep;
    float*    s_row = s_data; s_row[0]=0; s_row+=s_xstep;s_row[0]=0; s_row+=s_xstep;
    const Type* i_row = im_data;
    for (int x=2;x<nx2;++x)
    {
      // Evaluate the line strength at each orientation

      float f1 = float(i_row[i1a])+float(i_row[i1b])+float(i_row[i1c])+float(i_row[i1d]);
      float f2 = float(i_row[i2a])+float(i_row[i2b])+float(i_row[i2c])+float(i_row[i2d]);
      float f3 = float(i_row[i3a])+float(i_row[i3b])+float(i_row[i3c])+float(i_row[i3d]);
      float f4 = float(i_row[i4a])+float(i_row[i4b])+float(i_row[i4c])+float(i_row[i4d]);

      // Look for highest value (ie bright line on dark background)
      unsigned char best_d = 1;
      float min_f = f1;
      if (f2<min_f) { best_d=2; min_f=f2;}
      if (f3<min_f) { best_d=3; min_f=f3;}
      if (f4<min_f) { best_d=4; min_f=f4;}

        // Average on line - average off line
      float edge_s = (f1+f2+f3+f4)/12 - (17.0f/60) * min_f - 0.2*(*i_row);
      if (edge_s>edge_thresh)
      {
        *d_row = best_d;
        *s_row = edge_s;
      }
      else
      {
        *d_row=0; *s_row=0;
      }

      d_row+=d_xstep;
      s_row+=s_xstep;
      i_row+=xstep;
    }
    // Zero the last elements in the rows
    d_row[0]=0; d_row[d_xstep]=0;
    s_row[0]=0; s_row[s_xstep]=0;

    d_data += d_ystep;
    s_data += s_ystep;
    im_data += ystep;
  }
}


#undef MIL_ALGO_LINE_FILTER_INSTANTIATE
#define MIL_ALGO_LINE_FILTER_INSTANTIATE(T) \
 template class mil_algo_line_filter<T >

#endif
