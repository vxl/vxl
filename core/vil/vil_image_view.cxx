//: \file
//  \brief Specialisations of is_a() function
//  \author Ian Scott

#include <vil2/vil2_image_view.h>
#include <vil/vil_byte.h>
#include <vil/vil_rgb.h>

// Specialise the is_a() for vil_byte
template<> vcl_string vil2_image_view<vil_byte>::is_a() const
{  return vcl_string("vil2_image_view<vil_byte>"); }

// Specialise the is_a() for int
template<> vcl_string vil2_image_view<int>::is_a() const
{  return vcl_string("vil2_image_view<int>"); }

// Specialise the is_a() for float
template<> vcl_string vil2_image_view<float>::is_a() const
{  return vcl_string("vil2_image_view<float>"); }

// Specialise the is_a() for char
template<> vcl_string vil2_image_view<char>::is_a() const
{  return vcl_string("vil2_image_view<char>"); }

// Specialise the is_a() for short
template<> vcl_string vil2_image_view<short>::is_a() const
{  return vcl_string("vil2_image_view<short>"); }

// Specialise the is_a() for vil_rgb<vil_byte>
template<> vcl_string vil2_image_view<vil_rgb<vil_byte> >::is_a() const
{  return vcl_string("vil2_image_view<vil_rgb<vil_byte>>"); }

// Specialise the print_all() template for rgb<uchar>
template<>
void vil2_image_view<vil_rgb<vil_byte> >::print_all(vcl_ostream& os) const
{
  print(os);
  os<<"  xstep: "<<xstep_<<" ystep: "<<ystep_<<" planestep: "<<planestep_<<vcl_endl;

  for (int i=0;i<nplanes();++i)
  {
    if (nplanes()>1) os<<"Plane "<<i<<":"<<vcl_endl;
    const vil_rgb<vil_byte>* im_data = top_left_ + i*planestep_;
    for (int y=0;y<ny_;++y)
    {
      for (int x=0;x<nx_;++x)
      {
        int r = int(im_data[ystep_*y+x*xstep_].r);
        if (r<10)  os<<"0";
        if (r<100) os<<"0";
        os<<r<<"-";
        int g = int(im_data[ystep_*y+x*xstep_].g);
        if (g<10)  os<<"0";
        if (g<100) os<<"0";
        os<<g<<"-";
        int b = int(im_data[ystep_*y+x*xstep_].b);
        if (b<10)  os<<"0";
        if (b<100) os<<"0";
        os<<b<<" ";
      }
      os<<vcl_endl;
    }
  }
}



