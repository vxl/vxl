// This is mul/vil2/algo/vil2_line_filter.txx
#ifndef vil2_line_filter_txx_
#define vil2_line_filter_txx_
//:
// \file
// \brief Find line-like structures in a 2D image
// \author Tim Cootes

#include "vil2_line_filter.h"
#include <vil2/vil2_fill.h>
#include <vcl_cassert.h>

//: Find line like structures in image (light lines on dark backgrounds)
//  On exit line_str contains line strength at each pixel,
//  line_dir contains value indicating direction [0,4]
//  0 = Undefined, 1 = horizontal, 2 = 45 degrees etc
//  This version looks for light lines on a dark background only.
template <class Type>
void vil2_line_filter<Type>::light_lines_3x3(vil2_image_view<vxl_byte>& line_dir,
                                             vil2_image_view<float>& line_str,
                                             vil2_image_view<Type>const& image,
                                             float edge_thresh)
{
  assert(image.nplanes()==1);
  unsigned ni = image.ni();
  unsigned nj = image.nj();
  vcl_ptrdiff_t istep = image.istep();
  vcl_ptrdiff_t jstep = image.jstep();

  line_dir.set_size(ni,nj,1);
  line_str.set_size(ni,nj,1);

  vcl_ptrdiff_t d_istep = line_dir.istep();
  vcl_ptrdiff_t d_jstep = line_dir.jstep();
  vxl_byte* d_data  = line_dir.top_left_ptr();
  vcl_ptrdiff_t s_istep = line_str.istep();
  vcl_ptrdiff_t s_jstep = line_str.jstep();
  float* s_data = line_str.top_left_ptr();

  // Cannot calculate line strength in borders
  vil2_fill_line(d_data,ni,d_istep,vxl_byte(0));
  vil2_fill_line(d_data+(nj-1)*d_jstep,ni,d_istep,vxl_byte(0));
  vil2_fill_line(s_data,ni,s_istep,0.0f);
  vil2_fill_line(s_data+(nj-1)*s_jstep,ni,s_istep,0.0f);

  d_data += d_jstep;
  s_data += s_jstep;
  const Type* im_data = image.top_left_ptr()+jstep+istep;

  int ni1 = ni-1;
  int nj1 = nj-1;

  // Relative positions of points to be sampled
  vcl_ptrdiff_t nistep = -istep;
  vcl_ptrdiff_t njstep = -jstep;
  vcl_ptrdiff_t xjstep = istep+jstep;
  vcl_ptrdiff_t ynistep = jstep-istep;
  vcl_ptrdiff_t xnjstep = istep-jstep;
  vcl_ptrdiff_t ninjstep = -istep-jstep;

  for (int y=1;y<nj1;++y)
  {
    vxl_byte* d_row = d_data; d_row[0]=0; d_row+=d_istep;
    float*    s_row = s_data; s_row[0]=0; s_row+=s_istep;
    const Type* i_row = im_data;
    for (int x=1;x<ni1;++x)
    {
      // Evaluate the line strength at each orientation

      float f1 = float(i_row[nistep])  +float(i_row[istep]);
      float f2 = float(i_row[ninjstep])+float(i_row[xjstep]);
      float f3 = float(i_row[njstep])  +float(i_row[jstep]);
      float f4 = float(i_row[ynistep]) +float(i_row[xnjstep]);

      // Look for highest value (ie bright line on dark background)
      vxl_byte best_d = 1;
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

      d_row+=d_istep;
      s_row+=s_istep;
      i_row+=istep;
    }
    // Zero the last elements in the rows
    d_row[0]=0;
    s_row[0]=0;

    d_data += d_jstep;
    s_data += s_jstep;
    im_data += jstep;
  }
}

//: Find line like structures in image (dark lines on bright backgrounds)
//  On exit line_str contains line strength at each pixel,
//  line_dir contains value indicating direction [0,4]
//  0 = Undefined, 1 = horizontal, 2 = 45 degrees etc
template <class Type>
void vil2_line_filter<Type>::dark_lines_3x3(vil2_image_view<vxl_byte>& line_dir,
                                            vil2_image_view<float>& line_str,
                                            vil2_image_view<Type>const& image,
                                            float edge_thresh)
{
  assert(image.nplanes()==1);
  unsigned ni = image.ni();
  unsigned nj = image.nj();
  vcl_ptrdiff_t istep = image.istep();
  vcl_ptrdiff_t jstep = image.jstep();

  line_dir.set_size(ni,nj,1);
  line_str.set_size(ni,nj,1);

  vcl_ptrdiff_t d_istep = line_dir.istep();
  vcl_ptrdiff_t d_jstep = line_dir.jstep();
  vxl_byte* d_data  = line_dir.top_left_ptr();
  vcl_ptrdiff_t s_istep = line_str.istep();
  vcl_ptrdiff_t s_jstep = line_str.jstep();
  float* s_data = line_str.top_left_ptr();

  // Cannot calculate line strength in borders
  vil2_fill_line(d_data,ni,d_istep,vxl_byte(0));
  vil2_fill_line(d_data+(nj-1)*d_jstep,ni,d_istep,vxl_byte(0));
  vil2_fill_line(s_data,ni,s_istep,0.0f);
  vil2_fill_line(s_data+(nj-1)*s_jstep,ni,s_istep,0.0f);

  d_data += d_jstep;
  s_data += s_jstep;
  const Type* im_data = image.top_left_ptr()+jstep+istep;

  int ni1 = ni-1;
  int nj1 = nj-1;

  // Relative positions of points to be sampled
  vcl_ptrdiff_t nistep = -istep;
  vcl_ptrdiff_t njstep = -jstep;
  vcl_ptrdiff_t xjstep = istep+jstep;
  vcl_ptrdiff_t ynistep = jstep-istep;
  vcl_ptrdiff_t xnjstep = istep-jstep;
  vcl_ptrdiff_t ninjstep = -istep-jstep;

  for (int y=1;y<nj1;++y)
  {
    vxl_byte* d_row = d_data; d_row[0]=0; d_row+=d_istep;
    float*    s_row = s_data; s_row[0]=0; s_row+=s_istep;
    const Type* i_row = im_data;
    for (int x=1;x<ni1;++x)
    {
      // Evaluate the line strength at each orientation

      float f1 = float(i_row[nistep])  +float(i_row[istep]);
      float f2 = float(i_row[ninjstep])+float(i_row[xjstep]);
      float f3 = float(i_row[njstep])  +float(i_row[jstep]);
      float f4 = float(i_row[xnjstep]) +float(i_row[ynistep]);

      // Look for lowest value (ie dark line on light background)
      vxl_byte best_d = 1;
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

      d_row+=d_istep;
      s_row+=s_istep;
      i_row+=istep;
    }
    // Zero the last elements in the rows
    d_row[0]=0;
    s_row[0]=0;

    d_data += d_jstep;
    s_data += s_jstep;
    im_data += jstep;
  }
}

//: Find line like structures in image (light lines on dark backgrounds)
//  On exit line_str contains line strength at each pixel,
//  line_dir contains value indicating direction [0,4]
//  0 = Undefined, 1 = horizontal, 2 = 45 degrees etc
//  This version looks for light lines on a dark background only.
template <class Type>
void vil2_line_filter<Type>::light_lines_5x5(vil2_image_view<vxl_byte>& line_dir,
                                             vil2_image_view<float>& line_str,
                                             vil2_image_view<Type>const& image,
                                             float edge_thresh)
{
  assert(image.nplanes()==1);
  unsigned ni = image.ni();
  unsigned nj = image.nj();
  vcl_ptrdiff_t istep = image.istep();
  vcl_ptrdiff_t jstep = image.jstep();

  line_dir.set_size(ni,nj,1);
  line_str.set_size(ni,nj,1);

  vcl_ptrdiff_t d_istep = line_dir.istep();
  vcl_ptrdiff_t d_jstep = line_dir.jstep();
  vxl_byte* d_data  = line_dir.top_left_ptr();
  vcl_ptrdiff_t s_istep = line_str.istep();
  vcl_ptrdiff_t s_jstep = line_str.jstep();
  float* s_data = line_str.top_left_ptr();

  // Cannot calculate line strength in borders
  vil2_fill_line(d_data,ni,d_istep,vxl_byte(0));
  vil2_fill_line(d_data+d_jstep,ni,d_istep,vxl_byte(0));
  vil2_fill_line(d_data+(nj-1)*d_jstep,ni,d_istep,vxl_byte(0));
  vil2_fill_line(d_data+(nj-2)*d_jstep,ni,d_istep,vxl_byte(0));
  vil2_fill_line(s_data,ni,s_istep,0.0f);
  vil2_fill_line(s_data+s_jstep,ni,s_istep,0.0f);
  vil2_fill_line(s_data+(nj-1)*s_jstep,ni,s_istep,0.0f);
  vil2_fill_line(s_data+(nj-2)*s_jstep,ni,s_istep,0.0f);

  d_data += 2*d_jstep;
  s_data += 2*s_jstep;
  const Type* im_data = image.top_left_ptr()+2*(jstep+istep);

  int ni2 = ni-2;
  int nj2 = nj-2;

  // Relative positions of points to be sampled
  vcl_ptrdiff_t i1a = -2*istep;
  vcl_ptrdiff_t i1b = -istep;
  vcl_ptrdiff_t i1c = istep;
  vcl_ptrdiff_t i1d = 2*istep;

  vcl_ptrdiff_t i2c = istep+jstep;
  vcl_ptrdiff_t i2a = -2*i2c;
  vcl_ptrdiff_t i2b = -1*i2c;
  vcl_ptrdiff_t i2d = 2*i2c;

  vcl_ptrdiff_t i3a = -2*jstep;
  vcl_ptrdiff_t i3b = -1*jstep;
  vcl_ptrdiff_t i3c = jstep;
  vcl_ptrdiff_t i3d = 2*jstep;

  vcl_ptrdiff_t i4c = istep-jstep;
  vcl_ptrdiff_t i4a = -2*i4c;
  vcl_ptrdiff_t i4b = -1*i4c;
  vcl_ptrdiff_t i4d = 2*i4c;

  for (int y=2;y<nj2;++y)
  {
    vxl_byte* d_row = d_data; d_row[0]=0; d_row+=d_istep;d_row[0]=0; d_row+=d_istep;
    float*    s_row = s_data; s_row[0]=0; s_row+=s_istep;s_row[0]=0; s_row+=s_istep;
    const Type* i_row = im_data;
    for (int x=2;x<ni2;++x)
    {
      // Evaluate the line strength at each orientation

      float f1 = float(i_row[i1a])+float(i_row[i1b])+float(i_row[i1c])+float(i_row[i1d]);
      float f2 = float(i_row[i2a])+float(i_row[i2b])+float(i_row[i2c])+float(i_row[i2d]);
      float f3 = float(i_row[i3a])+float(i_row[i3b])+float(i_row[i3c])+float(i_row[i3d]);
      float f4 = float(i_row[i4a])+float(i_row[i4b])+float(i_row[i4c])+float(i_row[i4d]);

      // Look for highest value (ie bright line on dark background)
      vxl_byte best_d = 1;
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

      d_row+=d_istep;
      s_row+=s_istep;
      i_row+=istep;
    }
    // Zero the last elements in the rows
    d_row[0]=0; d_row[d_istep]=0;
    s_row[0]=0; s_row[s_istep]=0;

    d_data += d_jstep;
    s_data += s_jstep;
    im_data += jstep;
  }
}

//: Find line like structures in image (dark lines on light backgrounds)
template <class Type>
void vil2_line_filter<Type>::dark_lines_5x5(vil2_image_view<vxl_byte>& line_dir,
                                            vil2_image_view<float>& line_str,
                                            vil2_image_view<Type>const& image,
                                            float edge_thresh)
{
  assert(image.nplanes()==1);
  unsigned ni = image.ni();
  unsigned nj = image.nj();
  vcl_ptrdiff_t istep = image.istep();
  vcl_ptrdiff_t jstep = image.jstep();

  line_dir.set_size(ni,nj,1);
  line_str.set_size(ni,nj,1);

  vcl_ptrdiff_t d_istep = line_dir.istep();
  vcl_ptrdiff_t d_jstep = line_dir.jstep();
  vxl_byte* d_data  = line_dir.top_left_ptr();
  vcl_ptrdiff_t s_istep = line_str.istep();
  vcl_ptrdiff_t s_jstep = line_str.jstep();
  float* s_data = line_str.top_left_ptr();

  // Cannot calculate line strength in borders
  vil2_fill_line(d_data,ni,d_istep,vxl_byte(0));
  vil2_fill_line(d_data+d_jstep,ni,d_istep,vxl_byte(0));
  vil2_fill_line(d_data+(nj-1)*d_jstep,ni,d_istep,vxl_byte(0));
  vil2_fill_line(d_data+(nj-2)*d_jstep,ni,d_istep,vxl_byte(0));
  vil2_fill_line(s_data,ni,s_istep,0.0f);
  vil2_fill_line(s_data+s_jstep,ni,s_istep,0.0f);
  vil2_fill_line(s_data+(nj-1)*s_jstep,ni,s_istep,0.0f);
  vil2_fill_line(s_data+(nj-2)*s_jstep,ni,s_istep,0.0f);

  d_data += 2*d_jstep;
  s_data += 2*s_jstep;
  const Type* im_data = image.top_left_ptr()+2*(jstep+istep);

  int ni2 = ni-2;
  int nj2 = nj-2;

  // Relative positions of points to be sampled
  vcl_ptrdiff_t i1a = -2*istep;
  vcl_ptrdiff_t i1b = -istep;
  vcl_ptrdiff_t i1c = istep;
  vcl_ptrdiff_t i1d = 2*istep;

  vcl_ptrdiff_t i2c = istep+jstep;
  vcl_ptrdiff_t i2a = -2*i2c;
  vcl_ptrdiff_t i2b = -1*i2c;
  vcl_ptrdiff_t i2d = 2*i2c;

  vcl_ptrdiff_t i3a = -2*jstep;
  vcl_ptrdiff_t i3b = -1*jstep;
  vcl_ptrdiff_t i3c = jstep;
  vcl_ptrdiff_t i3d = 2*jstep;

  vcl_ptrdiff_t i4c = istep-jstep;
  vcl_ptrdiff_t i4a = -2*i4c;
  vcl_ptrdiff_t i4b = -1*i4c;
  vcl_ptrdiff_t i4d = 2*i4c;

  for (int y=2;y<nj2;++y)
  {
    vxl_byte* d_row = d_data; d_row[0]=0; d_row+=d_istep;d_row[0]=0; d_row+=d_istep;
    float*    s_row = s_data; s_row[0]=0; s_row+=s_istep;s_row[0]=0; s_row+=s_istep;
    const Type* i_row = im_data;
    for (int x=2;x<ni2;++x)
    {
      // Evaluate the line strength at each orientation

      float f1 = float(i_row[i1a])+float(i_row[i1b])+float(i_row[i1c])+float(i_row[i1d]);
      float f2 = float(i_row[i2a])+float(i_row[i2b])+float(i_row[i2c])+float(i_row[i2d]);
      float f3 = float(i_row[i3a])+float(i_row[i3b])+float(i_row[i3c])+float(i_row[i3d]);
      float f4 = float(i_row[i4a])+float(i_row[i4b])+float(i_row[i4c])+float(i_row[i4d]);

      // Look for highest value (ie bright line on dark background)
      vxl_byte best_d = 1;
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

      d_row+=d_istep;
      s_row+=s_istep;
      i_row+=istep;
    }
    // Zero the last elements in the rows
    d_row[0]=0; d_row[d_istep]=0;
    s_row[0]=0; s_row[s_istep]=0;

    d_data += d_jstep;
    s_data += s_jstep;
    im_data += jstep;
  }
}


#undef VIL2_LINE_FILTER_INSTANTIATE
#define VIL2_LINE_FILTER_INSTANTIATE(T) \
 template class vil2_line_filter<T >

#endif
