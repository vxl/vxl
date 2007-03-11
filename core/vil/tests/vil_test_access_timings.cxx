//:
// \file
// \brief Tool to test performance of different methods of accessing image data
//        When run, tries a variety of different approaches and reports their timings.
//        Useful to try it on different platforms to how different optimisers perform.
// \author Tim Cootes

#include <vcl_iostream.h>
#include <vxl_config.h> // for vxl_byte
#include <vil/vil_image_view.h>
#include <vcl_ctime.h>
#include <mbl/mbl_stats_1d.h>
#include <vcl_cassert.h>

const unsigned NI=256;
const unsigned NJ=256;
const unsigned NP=3;

template <class imT>
double method1(vil_image_view<imT>& image, int n_loops)
{
  vcl_time_t t0=vcl_clock();
  for (int n=0;n<n_loops;++n)
  {
    for (unsigned p=0;p<image.nplanes();++p)
      for (unsigned j=0;j<image.nj();++j)
        for (unsigned i=0;i<image.ni();++i)
          image(i,j,p) = imT(i+j+3*p);
  }
  vcl_time_t t1=vcl_clock();
  return 1000000*(double(t1)-double(t0))/(n_loops*CLOCKS_PER_SEC);
}

template <class imT>
double method2(vil_image_view<imT>& image, int n_loops)
{
  vcl_time_t t0=vcl_clock();
  for (int n=0;n<n_loops;++n)
  {
    unsigned ni=image.ni(),nj=image.nj(),np=image.nplanes();
    for (unsigned p=0;p<np;++p)
      for (unsigned j=0;j<nj;++j)
        for (unsigned i=0;i<ni;++i)
          image(i,j,p) = imT(i+j+3*p);
  }
  vcl_time_t t1=vcl_clock();
  return 1000000*(double(t1)-double(t0))/(n_loops*CLOCKS_PER_SEC);
}

template <class imT>
double method3(vil_image_view<imT>& image, int n_loops)
{
  vcl_time_t t0=vcl_clock();
  for (int n=0;n<n_loops;++n)
  {
    imT* plane = image.top_left_ptr();
    for (unsigned p=0;p<image.nplanes();++p,plane += image.planestep())
    {
      imT* row = plane;
      for (unsigned j=0;j<image.nj();++j,row += image.jstep())
      {
        imT* pixel = row;
        for (unsigned i=0;i<image.ni();++i,pixel+=image.istep())
          *pixel = imT(i+j+3*p);
      }
    }
  }
  vcl_time_t t1=vcl_clock();
  return 1000000*(double(t1)-double(t0))/(n_loops*CLOCKS_PER_SEC);
}

template <class imT>
double method4(vil_image_view<imT>& image, int n_loops)
{
  vcl_time_t t0=vcl_clock();
  for (int n=0;n<n_loops;++n)
  {
   unsigned ni=image.ni(),nj=image.nj(),np=image.nplanes();
   vcl_ptrdiff_t istep=image.istep(),jstep=image.jstep(),pstep=image.planestep();
   imT* plane = image.top_left_ptr();
   for (unsigned p=0;p<np;++p,plane += pstep)
   {
    imT* row = plane;
    for (unsigned j=0;j<nj;++j,row += jstep)
    {
      imT* pixel = row;
      for (unsigned i=0;i<ni;++i,pixel+=istep)
        *pixel = imT(i+j+3*p);
    }
   }
  }
  vcl_time_t t1=vcl_clock();
  return 1000000*(double(t1)-double(t0))/(n_loops*CLOCKS_PER_SEC);
}

template <class imT>
double method5(vil_image_view<imT>& image, int n_loops)
{
  vcl_time_t t0=vcl_clock();
  for (int n=0;n<n_loops;++n)
  {
    unsigned ni=image.ni(),nj=image.nj(),np=image.nplanes();
    vcl_ptrdiff_t istep=image.istep(),jstep=image.jstep(),pstep=image.planestep();
    imT* plane = image.top_left_ptr();
    for (unsigned p=0;p<np;++p,plane += pstep)
    {
      imT* row = plane;
      for (unsigned j=0;j<nj;++j,row += jstep)
      {
        imT* pixel = row+(ni-1)*istep;
        for (unsigned i=ni;i;--i,pixel-=istep)
          *pixel = imT(i-1+j+3*p);
      }
    }
  }
  vcl_time_t t1=vcl_clock();
  return 1000000*(double(t1)-double(t0))/(n_loops*CLOCKS_PER_SEC);
}

template <class imT>
double method6(vil_image_view<imT>& image, int n_loops)
{
  assert(image.istep() == 1);
  // Uses row[i] to simulate lookup type access used in original vil1 images
  vcl_time_t t0=vcl_clock();
  for (int n=0;n<n_loops;++n)
  {
   unsigned ni=image.ni(),nj=image.nj(),np=image.nplanes();
   vcl_ptrdiff_t istep=image.istep(),jstep=image.jstep(),pstep=image.planestep();
   imT* plane = image.top_left_ptr();
   for (unsigned p=0;p<np;++p,plane += pstep)
   {
    imT* row = plane;
    for (unsigned j=0;j<nj;++j,row += jstep)
    {
      for (unsigned i=0;i<ni;++i)
        row[i*istep] = imT(i+j+3*p);
    }
   }
  }
  vcl_time_t t1=vcl_clock();
  return 1000000*(double(t1)-double(t0))/(n_loops*CLOCKS_PER_SEC);
}

template <class imT>
double method7(vil_image_view<imT>& image, int n_loops)
{
  assert(image.istep() == 1);
  // Uses row[i] to simulate lookup type access used in original vil1 images
  assert(image.nplanes() == NP && image.ni() == NI);
  imT* raster_ptrs[NP][NJ];

  {
    unsigned nj=image.nj();
    for (unsigned p=0;p<NP;++p)
      for (unsigned j=0;j<nj;++j)
        raster_ptrs[p][j] = & image(0,j,p);
  }

  vcl_time_t t0=vcl_clock();
  for (int n=0;n<n_loops;++n)
  {
   unsigned nj=image.nj();
   for (unsigned p=0;p<NP;++p)
   {
    for (unsigned j=0;j<nj;++j)
    {
      for (unsigned i=0;i<NI;++i)
        raster_ptrs[p][j][i] = imT(i+j+3*p);
    }
   }
  }
  vcl_time_t t1=vcl_clock();
  return 1000000*(double(t1)-double(t0))/(n_loops*CLOCKS_PER_SEC);
}

template <class imT>
double method8(vil_image_view<imT>& image, int n_loops)
{
  assert(image.istep() == 1);

  vcl_time_t t0=vcl_clock();
  for (int n=0;n<n_loops;++n)
  {
   unsigned ni=image.ni(),nj=image.nj(),np=image.nplanes();
   vcl_ptrdiff_t istep=image.istep(),jstep=image.jstep(),pstep=image.planestep();
   imT* plane = image.top_left_ptr();
   for (unsigned p=0;p<np;++p,plane += pstep)
   {
    imT* row = plane;
    for (unsigned j=0;j<nj;++j,row += jstep)
    {
      for (unsigned i=0;i<ni;++i)
        row[i*istep] = imT(i+j+3*p);
    }
   }
  }
  vcl_time_t t1=vcl_clock();
  return 1000000*(double(t1)-double(t0))/(n_loops*CLOCKS_PER_SEC);
}

template <class imT>
double method(int i, vil_image_view<imT>& image, int n_loops)
{
  switch (i)
  {
    case 1 : return method1(image,n_loops);
    case 2 : return method2(image,n_loops);
    case 3 : return method3(image,n_loops);
    case 4 : return method4(image,n_loops);
    case 5 : return method5(image,n_loops);
    case 6 : return method6(image,n_loops);
    case 7 : return method7(image,n_loops);
    case 8 : return method8(image,n_loops);
    default: return -1.0;
  }
}

template <class imT>
void compute_stats(int i, vil_image_view<imT>& image, int n_loops)
{
  mbl_stats_1d stats;
  for (int j=0;j<10;++j) stats.obs(method(i,image,n_loops));
  vcl_cout<<"Method "<<i<<") Mean: "<<int(stats.mean()+0.5)
          <<"us  +/-"<<int(0.5*(stats.max()-stats.min())+0.5)<<"us\n";
}

int main(int argc, char** argv)
{
  vil_image_view<vxl_byte> byte_image(NI,NJ,NP);
  vil_image_view<float>    float_image(NI,NJ,NP);
  int n_loops = 100;

  vcl_cout<<"Times to fill a "<<NI<<" x "<<NJ
          <<" image of "<<NP<<" planes (in microsecs) [Range= 0.5(max-min)]\n"
          <<"Images of BYTE\n";
  for (int i=1;i<=8;++i)
  {
    compute_stats(i,byte_image,n_loops);
  }
  vcl_cout<<"Images of FLOAT\n";
  for (int i=1;i<=8;++i)
  {
    compute_stats(i,float_image,n_loops);
  }
  return 0;
}
