//:
// \file
// \brief Tool to compare standard algorithms/functions in vil and vil2
// \author Ian Scott
// Will compare the following operations
// Deep copy, fill, 1d convolve, 2d convolve, correlation, force to rgb, force to float, pnm read/write
// Using (where appropriate) byte, float and rgb byte pixels.
// These timings will be used to optimise vil2.


#include <vcl_iostream.h>
#include <vxl_config.h> // for imT
#include <vil2/vil2_image_view.h>
#include <vcl_ctime.h>
#include <mbl/mbl_stats_1d.h>
#include <vil/vil_memory_image_of.h>
#include <vil/vil_rgb.h>

const unsigned NI=256;
const unsigned NJ=256;

template <class T>
unsigned width(const vil2_image_view<T> & im){return im.ni();}
template <class T>
unsigned width(const vil_memory_image_of<T> & im){return im.width();}

template <class T>
unsigned height(const vil2_image_view<T> & im){return im.nj();}
template <class T>
unsigned height(const vil_memory_image_of<T> & im){return im.height();}

template <class imT>
double method1(imT& image, int n_loops)
{
  typedef typename imT::pixel_type PT;
  vcl_time_t t0=vcl_clock();
  for (int n=0;n<n_loops;++n)
  {
    for (unsigned j=0;j<height(image);++j)
      for (unsigned i=0;i<width(image);++i)
        image(i,j) = PT(i+j);
  }
  vcl_time_t t1=vcl_clock();
  return 1000000*(double(t1)-double(t0))/(n_loops*CLOCKS_PER_SEC);
}

template <class imT>
double method2(imT& image, int n_loops)
{
  typedef typename imT::pixel_type PT;
  vcl_time_t t0=vcl_clock();
  for (int n=0;n<n_loops;++n)
  {
    unsigned ni=width(image),nj=height(image);
    for (unsigned j=0;j<nj;++j)
      for (unsigned i=0;i<ni;++i)
        image(i,j) = PT(i+j);
  }
  vcl_time_t t1=vcl_clock();
  return 1000000*(double(t1)-double(t0))/(n_loops*CLOCKS_PER_SEC);
}

template <class imT>
double method(int i, imT& image, int n_loops)
{
  double t;
  switch (i)
  {
    case 1 : t=method1(image,n_loops); break;
    case 2 : t=method2(image,n_loops); break;
    default: t=-1;
  }
  return t;
}

template <class imT>
void compute_stats(int i, imT& image, int n_loops)
{
  mbl_stats_1d stats;
  for (int j=0;j<10;++j) stats.obs(method(i,image,n_loops));
  vcl_cout<<"Method "<<i<<") Mean: "<<int(stats.mean()+0.5)
          <<"us  +/-"<<int(0.5*(stats.max()-stats.min())+0.5)<<"us"<<vcl_endl;
}

int main(int argc, char** argv)
{
  vil_memory_image_of<vxl_byte> byte_1image(NI,NJ);
  vil_memory_image_of<float>    float_1image(NI,NJ);
  vil_memory_image_of<vil_rgb<vxl_byte> >    rgb_1image(NI,NJ);
  vil2_image_view<vxl_byte> byte_2image(NI,NJ);
  vil2_image_view<float>    float_2image(NI,NJ);
  vil2_image_view<vil_rgb<vxl_byte> >    rgb_2image(NI,NJ);
  int n_loops = 100;
  vcl_cout<<"Times to fill a "<<NI<<" x "<<NJ
          <<" image of 1 plane (in microsecs) [Range= 0.5(max-min)]"<<vcl_endl;
  vcl_cout<<"vil_memory_image_of Images of BYTE"<<vcl_endl;
  for (int i=1; i<=2; ++i)
    compute_stats(i,byte_1image,n_loops);
  vcl_cout<<"vil2_image_view Images of BYTE"<<vcl_endl;
  for (int i=1; i<=2; ++i)
    compute_stats(i,byte_2image,n_loops);
  vcl_cout<<"vil_memory_image_of Images of FLOAT"<<vcl_endl;
  for (int i=1; i<=2; ++i)
    compute_stats(i,float_1image,n_loops);
  vcl_cout<<"vil2_image_view Images of FLOAT"<<vcl_endl;
  for (int i=1; i<=2; ++i)
    compute_stats(i,float_2image,n_loops);
  vcl_cout<<"vil_memory_image_of Images of RGB<BYTE>"<<vcl_endl;
  for (int i=1; i<=2; ++i)
    compute_stats(i,rgb_1image,n_loops);
  vcl_cout<<"vil2_image_view Images of RGB<BYTE>"<<vcl_endl;
  for (int i=1; i<=2; ++i)
    compute_stats(i,rgb_2image,n_loops);
  return 0;
}
