//:
// \file
// \brief Tool to test performance of different methods of accessing image data
//        When run, tries a variety of different approaches and reports their timings.
//        Useful to try it on different platforms to how different optimisers perform.
// \author Tim Cootes

#include <ctime>
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vxl_config.h> // for vxl_byte
#include <vil/vil_image_view.h>
#include <mbl/mbl_stats_1d.h>

constexpr unsigned NI = 256;
constexpr unsigned NJ = 256;
constexpr unsigned NP = 3;

template <class imT>
double method1(const vil_image_view<imT>& src_image,
               vil_image_view<imT>& dest_image, int n_loops)
{
  dest_image.set_size(src_image.ni(),src_image.nj(),src_image.nplanes());
  std::time_t t0=std::clock();
  for (int n=0;n<n_loops;++n)
  {
    for (unsigned p=0;p<src_image.nplanes();++p)
      for (unsigned j=0;j<src_image.nj();++j)
        for (unsigned i=0;i<src_image.ni();++i)
          dest_image(i,j,p) = src_image(i,j,p)+1;
  }
  std::time_t t1=std::clock();
  return 1000000*(double(t1)-double(t0))/(n_loops*CLOCKS_PER_SEC);
}

template <class imT>
double method2(const vil_image_view<imT>& src_image,
               vil_image_view<imT>& dest_image, int n_loops)
{
  dest_image.set_size(src_image.ni(),src_image.nj(),src_image.nplanes());
  std::time_t t0=std::clock();
  for (int n=0;n<n_loops;++n)
  {
    unsigned ni=src_image.ni(),nj=src_image.nj(),np=src_image.nplanes();
    for (unsigned p=0;p<np;++p)
      for (unsigned j=0;j<nj;++j)
        for (unsigned i=0;i<ni;++i)
          dest_image(i,j,p) = src_image(i,j,p)+1;
  }
  std::time_t t1=std::clock();
  return 1000000*(double(t1)-double(t0))/(n_loops*CLOCKS_PER_SEC);
}


template <class imT>
double method3(const vil_image_view<imT>& src_image,
               vil_image_view<imT>& dest_image, int n_loops)
{
  std::time_t t0=std::clock();
  for (int n=0;n<n_loops;++n)
  {
   unsigned ni=src_image.ni(),nj=src_image.nj(),np=src_image.nplanes();
   std::ptrdiff_t sistep=src_image.istep(),sjstep=src_image.jstep(),spstep=src_image.planestep();
   std::ptrdiff_t distep=dest_image.istep(),djstep=dest_image.jstep(),dpstep=dest_image.planestep();
   const imT* splane = src_image.top_left_ptr();
   imT* dplane = dest_image.top_left_ptr();
   for (unsigned p=0;p<np;++p,splane += spstep, dplane+=dpstep)
   {
    const imT* srow = splane;
    imT* drow = dplane;
    for (unsigned j=0;j<nj;++j,srow += sjstep,drow+=djstep)
    {
      const imT* spixel = srow;
      imT* dpixel = drow;
      for (unsigned i=0;i<ni;++i,spixel+=sistep,dpixel+=distep)
        *dpixel = *spixel + 1;;
    }
   }
  }
  std::time_t t1=std::clock();
  return 1000000*(double(t1)-double(t0))/(n_loops*CLOCKS_PER_SEC);
}

template <class imT>
double method4(const vil_image_view<imT>& src_image,
               vil_image_view<imT>& dest_image, int n_loops)
{
  std::time_t t0=std::clock();
  for (int n=0;n<n_loops;++n)
  {
   unsigned ni=src_image.ni(),nj=src_image.nj(),np=src_image.nplanes();
   std::ptrdiff_t sistep=src_image.istep(),sjstep=src_image.jstep(),spstep=src_image.planestep();
   std::ptrdiff_t distep=dest_image.istep(),djstep=dest_image.jstep(),dpstep=dest_image.planestep();
   const imT* splane = src_image.top_left_ptr();
   imT* dplane = dest_image.top_left_ptr();
   for (unsigned p=0;p<np;++p,splane += spstep, dplane+=dpstep)
   {
    const imT* srow = splane;
    imT* drow = dplane;
    for (unsigned j=0;j<nj;++j,srow += sjstep,drow+=djstep)
    {
      for (unsigned i=0;i<ni;++i) drow[i*distep] = srow[i*sistep] + 1;;
    }
   }
  }
  std::time_t t1=std::clock();
  return 1000000*(double(t1)-double(t0))/(n_loops*CLOCKS_PER_SEC);
}

template <class imT>
double method5(const vil_image_view<imT>& src_image,
               vil_image_view<imT>& dest_image, int n_loops)
{
  // Assumes that istep==1 for both!
  std::time_t t0=std::clock();
  for (int n=0;n<n_loops;++n)
  {
   unsigned ni=src_image.ni(),nj=src_image.nj(),np=src_image.nplanes();
   std::ptrdiff_t sjstep=src_image.jstep(),spstep=src_image.planestep();
   std::ptrdiff_t djstep=dest_image.jstep(),dpstep=dest_image.planestep();
   const imT* splane = src_image.top_left_ptr();
   imT* dplane = dest_image.top_left_ptr();
   for (unsigned p=0;p<np;++p,splane += spstep, dplane+=dpstep)
   {
    const imT* srow = splane;
    imT* drow = dplane;
    for (unsigned j=0;j<nj;++j,srow += sjstep,drow+=djstep)
    {
      for (unsigned i=0;i<ni;++i) drow[i] = srow[i] + 1;;
    }
   }
  }
  std::time_t t1=std::clock();
  return 1000000*(double(t1)-double(t0))/(n_loops*CLOCKS_PER_SEC);
}

template <class imT>
double method6(const vil_image_view<imT>& src_image,
               vil_image_view<imT>& dest_image, int n_loops)
{
  // Assumes that istep==1 for both!
  std::time_t t0=std::clock();
  for (int n=0;n<n_loops;++n)
  {
   unsigned ni=src_image.ni(),nj=src_image.nj(),np=src_image.nplanes();
   std::ptrdiff_t sistep=src_image.istep(),sjstep=src_image.jstep(),spstep=src_image.planestep();
   std::ptrdiff_t distep=dest_image.istep(),djstep=dest_image.jstep(),dpstep=dest_image.planestep();
   const imT* splane = src_image.top_left_ptr();
   imT* dplane = dest_image.top_left_ptr();
   for (unsigned p=0;p<np;++p,splane += spstep, dplane+=dpstep)
   {
    const imT* srow = splane -sistep;  // going to index i=n..1
    imT* drow = dplane-distep;
    for (unsigned j=0;j<nj;++j,srow += sjstep,drow+=djstep)
    {
      for (unsigned i=ni;i;--i) drow[i] = srow[i] + 1;  // Test i against zero
    }
   }
  }
  std::time_t t1=std::clock();
  return 1000000*(double(t1)-double(t0))/(n_loops*CLOCKS_PER_SEC);
}

template <class imT>
double method7(const vil_image_view<imT>& src_image,
               vil_image_view<imT>& dest_image, int n_loops)
{
  assert (src_image.istep() == 1);
  // Uses row[i] to simulate lookup type access used in original vil1 images
  assert(src_image.nplanes() == NP && src_image.ni() == NI);
  const imT* src_raster_ptrs[NP][NJ];
  imT* dest_raster_ptrs[NP][NJ];

  {
    unsigned nj=src_image.nj(),np=src_image.nplanes();
    for (unsigned p=0;p<np;++p)
      for (unsigned j=0;j<nj;++j)
      {
        (src_raster_ptrs)[p][j] = & src_image(0,j,p);
        (dest_raster_ptrs)[p][j] = & dest_image(0,j,p);
      }
  }

  std::time_t t0=std::clock();
  for (int n=0;n<n_loops;++n)
  {
   unsigned ni=src_image.ni(),nj=src_image.nj(),np=src_image.nplanes();
   for (unsigned p=0;p<np;++p)
   {
    for (unsigned j=0;j<nj;++j)
    {
      for (unsigned i=0;i<ni;++i)
        dest_raster_ptrs[p][j][i] = src_raster_ptrs[p][j][i]+1;
    }
   }
  }
  std::time_t t1=std::clock();
  return 1000000*(double(t1)-double(t0))/(n_loops*CLOCKS_PER_SEC);
}


template <class imT>
double method(int i, const vil_image_view<imT>& src_image,
              vil_image_view<imT>& dest_image, int n_loops)
{
  double t;
  switch (i)
  {
    case 1 : t=method1(src_image,dest_image,n_loops); break;
    case 2 : t=method2(src_image,dest_image,n_loops); break;
    case 3 : t=method3(src_image,dest_image,n_loops); break;
    case 4 : t=method4(src_image,dest_image,n_loops); break;
    case 5 : t=method5(src_image,dest_image,n_loops); break;
    case 6 : t=method6(src_image,dest_image,n_loops); break;
    case 7 : t=method7(src_image,dest_image,n_loops); break;
    default: t=-1;
  }
  return t;
}

template <class imT>
void compute_stats(int i, const vil_image_view<imT>& src_image,
                   vil_image_view<imT>& dest_image, int n_loops)
{
  mbl_stats_1d stats;
  for (int j=0;j<10;++j) stats.obs(method(i,src_image,dest_image,n_loops));
  std::cout<<"Method "<<i<<") Mean: "<<int(stats.mean()+0.5)
          <<"us  +/-"<<int(0.5*(stats.max()-stats.min())+0.5)<<"us\n";
}

int main(int argc, char** argv)
{
  vil_image_view<vxl_byte> src_byte_image(NI,NJ,NP),dest_byte_image(NI,NJ,NP);
  vil_image_view<float>    src_float_image(NI,NJ,NP),dest_float_image(NI,NJ,NP);
  int n_loops = 100;

  std::cout<<"Times to copy and increment a "<<NI<<" x "<<NJ
          <<" image of "<<NP<<" planes (in microsecs) [Range= 0.5(max-min)]\n\n";

  std::cout<<"Images of BYTE\n";
  for (int i=1;i<=7;++i)
  {
    compute_stats(i,src_byte_image,dest_byte_image,n_loops);
  }
  std::cout<<"Images of FLOAT\n";
  for (int i=1;i<=7;++i)
  {
    compute_stats(i,src_float_image,dest_float_image,n_loops);
  }
  return 0;
}
