//:
// \file
// \brief Tool to test performance of different methods of accessing image data
//        When run, tries a variety of different approaches and reports their timings.
//        Useful to try it on different platforms to how different optimisers perform.
// \author Tim Cootes

#include <vcl_iostream.h>
#include <vxl_config.h> // for vxl_byte
#include <vil2/vil2_image_view.h>
#include <vcl_ctime.h>
#include <mbl/mbl_stats_1d.h>

double method1(vil2_image_view<vxl_byte>& image, int n_loops)
{
  vcl_time_t t0=vcl_clock();
  for (int n=0;n<n_loops;++n)
  {
    for (unsigned p=0;p<image.nplanes();++p)
      for (unsigned j=0;j<image.nj();++j)
        for (unsigned i=0;i<image.ni();++i)
          image(i,j,p) = vxl_byte(i+j+3*p);
  }
  vcl_time_t t1=vcl_clock();
  return 1000000*(double(t1)-double(t0))/(n_loops*CLOCKS_PER_SEC);
}

double method2(vil2_image_view<vxl_byte>& image, int n_loops)
{
  vcl_time_t t0=vcl_clock();
  for (int n=0;n<n_loops;++n)
  {
    unsigned ni=image.ni(),nj=image.nj(),np=image.nplanes();
    for (unsigned p=0;p<np;++p)
      for (unsigned j=0;j<nj;++j)
        for (unsigned i=0;i<ni;++i)
          image(i,j,p) = vxl_byte(i+j+3*p);
  }
  vcl_time_t t1=vcl_clock();
  return 1000000*(double(t1)-double(t0))/(n_loops*CLOCKS_PER_SEC);
}

double method3(vil2_image_view<vxl_byte>& image, int n_loops)
{
  vcl_time_t t0=vcl_clock();
  for (int n=0;n<n_loops;++n)
  {
    vxl_byte* plane = image.top_left_ptr();
    for (unsigned p=0;p<image.nplanes();++p,plane += image.planestep())
     {
      vxl_byte* row = plane;
      for (unsigned j=0;j<image.nj();++j,row += image.jstep())
      {
        vxl_byte* pixel = row;
        for (unsigned i=0;i<image.ni();++i,pixel+=image.istep())
          *pixel = vxl_byte(i+j+3*p);
      }
    }
  }
  vcl_time_t t1=vcl_clock();
  return 1000000*(double(t1)-double(t0))/(n_loops*CLOCKS_PER_SEC);
}

double method4(vil2_image_view<vxl_byte>& image, int n_loops)
{
  vcl_time_t t0=vcl_clock();
  for (int n=0;n<n_loops;++n)
  {
   unsigned ni=image.ni(),nj=image.nj(),np=image.nplanes();
   int istep=image.istep(),jstep=image.jstep(),pstep=image.planestep();
   vxl_byte* plane = image.top_left_ptr();
   for (unsigned p=0;p<np;++p,plane += pstep)
   {
    vxl_byte* row = plane;
    for (unsigned j=0;j<nj;++j,row += jstep)
    {
      vxl_byte* pixel = row;
      for (unsigned i=0;i<ni;++i,pixel+=istep)
        *pixel = vxl_byte(i+j+3*p);
    }
   }
  }
  vcl_time_t t1=vcl_clock();
  return 1000000*(double(t1)-double(t0))/(n_loops*CLOCKS_PER_SEC);
}

double method5(vil2_image_view<vxl_byte>& image, int n_loops)
{
  vcl_time_t t0=vcl_clock();
  for (int n=0;n<n_loops;++n)
  {
    unsigned ni=image.ni(),nj=image.nj(),np=image.nplanes();
    int istep=image.istep(),jstep=image.jstep(),pstep=image.planestep();
    vxl_byte* plane = image.top_left_ptr();
   for (unsigned p=0;p<np;++p,plane += pstep)
   {
    vxl_byte* row = plane;
    for (unsigned j=0;j<nj;++j,row += jstep)
    {
      vxl_byte* pixel = row+(ni-1)*istep;
      for (unsigned i=ni;i;--i,pixel-=istep)
        *pixel = vxl_byte(i-1+j+3*p);
    }
   }
  }
  vcl_time_t t1=vcl_clock();
  return 1000000*(double(t1)-double(t0))/(n_loops*CLOCKS_PER_SEC);
}

double method(int i, vil2_image_view<vxl_byte>& image, int n_loops)
{
  double t;
  switch (i)
  {
    case 1 : t=method1(image,n_loops); break;
    case 2 : t=method2(image,n_loops); break;
    case 3 : t=method3(image,n_loops); break;
    case 4 : t=method4(image,n_loops); break;
    case 5 : t=method5(image,n_loops); break;
	default: t=-1;
  }
  return t;
}

void compute_stats(int i, vil2_image_view<vxl_byte>& image, int n_loops)
{
  mbl_stats_1d stats;
  for (int j=0;j<10;++j) stats.obs(method(i,image,n_loops));
  vcl_cout<<"Method "<<i<<") Mean: "<<int(stats.mean()+0.5)
          <<"us  +/-"<<int(0.5*(stats.max()-stats.min())+0.5)<<"us"<<vcl_endl;
}

int main(int argc, char** argv)
{
  vil2_image_view<vxl_byte> image(256,256,3);

  vcl_cout<<"Times to fill a 256 x 256 image of 3 planes (in microsecs) [Range= 0.5(max-min)]"<<vcl_endl;
  int n_loops = 100;
  for (int i=1;i<=5;++i)
  {
    compute_stats(i,image,n_loops);
  }
  return 0;
}
