//:
// \file
// \brief Tool to test performance of different methods of accessing image data
//   When run, tries a variety of different approaches and reports their timings.
//   Useful to try it on different platforms to how different optimisers perform.
// \author Tim Cootes

#include <iostream>
#include <ctime>
#include <cassert>
#include <vxl_config.h> // for vxl_byte
#include <vil/vil_image_view.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <mbl/mbl_stats_1d.h>
#include <vil/vil_rgb.h>

constexpr unsigned NI = 256;
constexpr unsigned NJ = 256;

template <class imT>
double method1(vil_image_view<imT>& image, int n_loops)
{
  std::time_t t0=std::clock();
  for (int n=0;n<n_loops;++n)
  {
    for (unsigned j=0;j<image.nj();++j)
      for (unsigned i=0;i<image.ni();++i)
        image(i,j) = imT(i+j);
  }
  std::time_t t1=std::clock();
  return 1000000*(double(t1)-double(t0))/(n_loops*CLOCKS_PER_SEC);
}

template <class imT>
double method2(vil_image_view<imT>& image, int n_loops)
{
  std::time_t t0=std::clock();
  for (int n=0;n<n_loops;++n)
  {
    unsigned ni=image.ni(),nj=image.nj();
    for (unsigned j=0;j<nj;++j)
      for (unsigned i=0;i<ni;++i)
        image(i,j) = imT(i+j);
  }
  std::time_t t1=std::clock();
  return 1000000*(double(t1)-double(t0))/(n_loops*CLOCKS_PER_SEC);
}

template <class imT>
double method3(vil_image_view<imT>& image, int n_loops)
{
  std::time_t t0=std::clock();
  for (int n=0;n<n_loops;++n)
  {
    imT* row = image.top_left_ptr();
    for (unsigned j=0;j<image.nj();++j,row += image.jstep())
    {
      imT* pixel = row;
      for (unsigned i=0;i<image.ni();++i,pixel+=image.istep())
        *pixel = imT(i+j);
    }
  }
  std::time_t t1=std::clock();
  return 1000000*(double(t1)-double(t0))/(n_loops*CLOCKS_PER_SEC);
}

template <class imT>
double method4(vil_image_view<imT>& image, int n_loops)
{
  std::time_t t0=std::clock();
  for (int n=0;n<n_loops;++n)
  {
   unsigned ni=image.ni(),nj=image.nj();
   std::ptrdiff_t istep=image.istep(),jstep=image.jstep();
    imT* row = image.top_left_ptr();
    for (unsigned j=0;j<nj;++j,row += jstep)
    {
      imT* pixel = row;
      for (unsigned i=0;i<ni;++i,pixel+=istep)
        *pixel = imT(i+j);
    }
  }
  std::time_t t1=std::clock();
  return 1000000*(double(t1)-double(t0))/(n_loops*CLOCKS_PER_SEC);
}

template <class imT>
double method5(vil_image_view<imT>& image, int n_loops)
{
  std::time_t t0=std::clock();
  for (int n=0;n<n_loops;++n)
  {
    unsigned ni=image.ni(),nj=image.nj();
    std::ptrdiff_t istep=image.istep(),jstep=image.jstep();
    imT* row = image.top_left_ptr();
    for (unsigned j=0;j<nj;++j,row += jstep)
    {
      imT* pixel = row+(ni-1)*istep;
      for (unsigned i=ni;i;--i,pixel-=istep)
        *pixel = imT(i-1+j);
    }
  }
  std::time_t t1=std::clock();
  return 1000000*(double(t1)-double(t0))/(n_loops*CLOCKS_PER_SEC);
}

template <class imT>
double method6(vil_image_view<imT>& image, int n_loops)
{
  assert(image.istep() == 1);
  // Uses row[i] to simulate lookup type access used in original vil1 images
  std::time_t t0=std::clock();
  for (int n=0;n<n_loops;++n)
  {
   unsigned ni=image.ni(),nj=image.nj();
   std::ptrdiff_t jstep=image.jstep();
    imT* row = image.top_left_ptr();
    for (unsigned j=0;j<nj;++j,row += jstep)
    {
      for (unsigned i=0;i<ni;++i)
        row[i] = imT(i+j);
    }
  }
  std::time_t t1=std::clock();
  return 1000000*(double(t1)-double(t0))/(n_loops*CLOCKS_PER_SEC);
}

template <class imT>
double method7(vil_image_view<imT>& image, int n_loops)
{
  assert(image.istep() == 1);
  // Uses row[i] to simulate lookup type access used in original vil1 images
  assert(image.ni() == NI);
  imT* raster_ptrs[NJ];

  {
    unsigned nj=image.nj();
    for (unsigned j=0;j<nj;++j)
      (raster_ptrs)[j] = & image(0,j);
  }

  std::time_t t0=std::clock();
  for (int n=0;n<n_loops;++n)
  {
    unsigned ni=image.ni(),nj=image.nj();
    for (unsigned j=0;j<nj;++j)
    {
      for (unsigned i=0;i<ni;++i)
        raster_ptrs[j][i] = imT(i+j);
    }
  }
  std::time_t t1=std::clock();
  return 1000000*(double(t1)-double(t0))/(n_loops*CLOCKS_PER_SEC);
}

template <class imT>
double method8(vil_image_view<imT>& image, int n_loops)
{
  assert(image.istep() == 1);

  std::time_t t0=std::clock();
  for (int n=0;n<n_loops;++n)
  {
    unsigned ni=image.ni(),nj=image.nj();
    std::ptrdiff_t istep=image.istep(),jstep=image.jstep();
    imT* row = image.top_left_ptr();
    for (unsigned j=0;j<nj;++j,row += jstep)
    {
      for (unsigned i=0;i<ni;++i)
        row[i*istep] = imT(i+j);
    }
  }
  std::time_t t1=std::clock();
  return 1000000*(double(t1)-double(t0))/(n_loops*CLOCKS_PER_SEC);
}

template <class imT>
double method(int i, vil_image_view<imT>& image, int n_loops)
{
  double t;
  switch (i)
  {
    case 1 : t=method1(image,n_loops); break;
    case 2 : t=method2(image,n_loops); break;
    case 3 : t=method3(image,n_loops); break;
    case 4 : t=method4(image,n_loops); break;
    case 5 : t=method5(image,n_loops); break;
    case 6 : t=method6(image,n_loops); break;
    case 7 : t=method7(image,n_loops); break;
    case 8 : t=method8(image,n_loops); break;
    default: t=-1;
  }
  return t;
}

template <class imT>
void compute_stats(int i, vil_image_view<imT>& image, int n_loops)
{
  mbl_stats_1d stats;
  for (int j=0;j<10;++j) stats.obs(method(i,image,n_loops));
  std::cout<<"Method "<<i<<") Mean: "<<int(stats.mean()+0.5)
          <<"us  +/-"<<int(0.5*(stats.max()-stats.min())+0.5)<<"us"<<std::endl;
}

int main(int argc, char** argv)
{
  vil_image_view<vxl_byte> byte_image(NI,NJ);
  vil_image_view<float>    float_image(NI,NJ);
  vil_image_view<vil_rgb<vxl_byte> > rgb_image(NI,NJ);
  int n_loops = 100;

  std::cout<<"Times to fill a "<<NI<<" x "<<NJ
          <<" image of 1 plane (in microsecs) [Range= 0.5(max-min)]"<<std::endl
          <<"Images of BYTE"<<std::endl;
  for (int i=1;i<=8;++i)
  {
    compute_stats(i,byte_image,n_loops);
  }
  std::cout<<"Images of FLOAT"<<std::endl;
  for (int i=1;i<=8;++i)
  {
    compute_stats(i,float_image,n_loops);
  }
  std::cout<<"Images of RGB<BYTE>"<<std::endl;
  for (int i=1;i<=8;++i)
  {
    compute_stats(i,rgb_image,n_loops);
  }
  return 0;
}
