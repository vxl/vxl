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
#include <vcl_vector.h>
#include <vcl_cassert.h>

const unsigned NI=256;
const unsigned NJ=256;
const unsigned NP=3;
const double dx = 0.7;
const double dy = 1.3;

template <class imT>
double method1(vil_image_view<imT>& image,
               const int* x, const int* y, int n_pts, int n_loops)
{
  vcl_time_t t0=vcl_clock();
  double sum=0.0;
  for (int n=0;n<n_loops;++n)
  {
    for (int i=0;i<n_pts;++i)
      sum+=(double) image(x[i],y[i]);
  }
  vcl_time_t t1=vcl_clock();
  vcl_cout << sum;
  return 1e9*(double(t1)-double(t0))/(double(n_pts)*n_loops*CLOCKS_PER_SEC);
}

template <class imT>
double method2(vil_image_view<imT>& image,
               const int* x, const int* y, int n_pts, int n_loops)
{
  vcl_time_t t0=vcl_clock();
  double sum=0.0;
  for (int n=0;n<n_loops;++n)
  {
    for (int i=0;i<n_pts;++i)
      sum+=(double) image.top_left_ptr()[x[i]*image.istep()+y[i]*image.jstep()];
  }
  vcl_time_t t1=vcl_clock();
  vcl_cout << sum;
  return 1e9*(double(t1)-double(t0))/(double(n_pts)*n_loops*CLOCKS_PER_SEC);
}


template <class imT>
double method3(vil_image_view<imT>& image,
               const int* x, const int* y, int n_pts, int n_loops)
{
  assert (image.istep() == 1);
  // Uses row[i] to simulate lookup type access used in original vil1 images
  assert(image.ni() == NI);
  imT* raster_ptrs[NJ];

  {
    unsigned nj=image.nj();
    for (unsigned j=0;j<nj;++j)
        (raster_ptrs)[j] = & image(0,j,0);
  }

  vcl_time_t t0=vcl_clock();
  double sum=0.0;
  for (int n=0;n<n_loops;++n)
  {
    for (int i=0;i<n_pts;++i)
      sum+=(double) raster_ptrs[y[i]][x[i]];
  }
  vcl_time_t t1=vcl_clock();
  vcl_cout << sum;
  return 1e9*(double(t1)-double(t0))/(double(n_pts)*n_loops*CLOCKS_PER_SEC);
}


template <class imT>
double method(int i, vil_image_view<imT>& image,
              const int* x, const int* y, int n_pts, int n_loops)
{
  double t;
  switch (i)
  {
    case 1 : t=method1(image,x,y,n_pts,n_loops); break;
    case 2 : t=method2(image,x,y,n_pts,n_loops); break;
    case 3 : t=method3(image,x,y,n_pts,n_loops); break;
    default: t=-1;
  }
  return t;
}

template <class imT>
void compute_stats(int i, vil_image_view<imT>& image,
                   const int* x, const int* y, int n_pts, int n_loops)
{
  mbl_stats_1d stats;
  vcl_cout << "\t\t\t\t";
  for (int j=0;j<10;++j) stats.obs(method(i,image,x,y,n_pts,n_loops));
  vcl_cout<<"\nMethod "<<i<<") Mean: "<<0.1*int(10*stats.mean()+5)
          <<"ns  +/-"<<0.1*int(5*(stats.max()-stats.min())+5)<<"ns\n";
}


// ================== Bilinear ========================

template<class T>
inline double bilin_interp1(double x, double y, const T* data, vcl_ptrdiff_t xstep, vcl_ptrdiff_t ystep)
{
  int p1x=int(x);
  double normx = x-p1x;
  int p1y=int(y);
  double normy = y-p1y;

  const T* pix1 = data + p1y*ystep + p1x*xstep;

  double i1 = pix1[0    ]+(pix1[      ystep]-pix1[0    ])*normy;
  double i2 = pix1[xstep]+(pix1[xstep+ystep]-pix1[xstep])*normy;

  return i1+(i2-i1)*normx;
}

template<class T>
inline double bilin_interp2(double x, double y, const T** data)
{
  int p1x=int(x);
  double normx = x-p1x;
  int p1y=int(y);
  double normy = y-p1y;

  const T* row1=data[p1y];
  const T* row2=data[p1y+1];

  double i1 = row1[p1x]+(row1[p1x+1]-row1[p1x])*normx;
  double i2 = row2[p1x]+(row2[p1x+1]-row2[p1x])*normx;

  return i1+(i2-i1)*normy;
}

template <class imT>
double bilin_method1(vil_image_view<imT>& image, int n_pts, int n_loops, double& sum)
{
  vcl_time_t t0=vcl_clock();
  sum=0.0;
  for (int n=0;n<n_loops;++n)
  {
     double x = 1.3,y=1.7;
     vcl_ptrdiff_t istep=image.istep(),jstep=image.jstep();
     imT* plane = image.top_left_ptr();
     for (int i=0;i<n_pts;++i,x+=dx,y+=dy)
       sum+=(double) bilin_interp1(x,y,plane,istep,jstep);
  }
  vcl_time_t t1=vcl_clock();
  vcl_cout << sum;
  return 1e9*(double(t1)-double(t0))/(double(n_pts)*n_loops*CLOCKS_PER_SEC);
}

template <class imT>
double bilin_method2(vil_image_view<imT>& image, int n_pts, int n_loops, double& sum)
{
  vcl_time_t t0=vcl_clock();
  sum=0.0;
  for (int n=0;n<n_loops;++n)
  {
    double x = 1.3,y=1.7;
    for (int i=0;i<n_pts;++i,x+=dx,y+=dy)
      sum+=(double) bilin_interp1(x,y,image.top_left_ptr(),image.istep(),image.jstep());
  }
  vcl_time_t t1=vcl_clock();
  vcl_cout << sum;
  return 1e9*(double(t1)-double(t0))/(double(n_pts)*n_loops*CLOCKS_PER_SEC);
}

template <class imT>
double bilin_method3(vil_image_view<imT>& image, int n_pts, int n_loops, double& sum)
{
  assert (image.istep() == 1);
  // Uses row[i] to simulate lookup type access used in original vil1 images
  assert(image.ni() == NI);
  const imT* raster_ptrs[NJ];

  {
    unsigned nj=image.nj();
    for (unsigned j=0;j<nj;++j)
      (raster_ptrs)[j] = & image(0,j,0);
  }

  vcl_time_t t0=vcl_clock();
  sum=0.0;
  for (int n=0;n<n_loops;++n)
  {
    double x = 1.3,y=1.7;
    for (int i=0;i<n_pts;++i,x+=dx,y+=dy)
      sum+=(double) bilin_interp2(x,y,raster_ptrs);
  }
  vcl_time_t t1=vcl_clock();
  vcl_cout << sum;
  return 1e9*(double(t1)-double(t0))/(double(n_pts)*n_loops*CLOCKS_PER_SEC);
}

template <class imT>
double bilin_method(int i, vil_image_view<imT>& image, int n_pts, int n_loops)
{
  double t,sum;
  switch (i)
  {
    case 1 : t=bilin_method1(image,n_pts,n_loops,sum); break;
    case 2 : t=bilin_method2(image,n_pts,n_loops,sum); break;
    case 3 : t=bilin_method3(image,n_pts,n_loops,sum); break;
    default: t=-1;
  }
  return t;
}

template <class imT>
void compute_bilin_stats(int i, vil_image_view<imT>& image,int n_pts, int n_loops)
{
  mbl_stats_1d stats;
  vcl_cout << "\t\t\t\t";
  for (int j=0;j<10;++j) stats.obs(bilin_method(i,image,n_pts,n_loops));
  vcl_cout<<"\nBilin. Method "<<i<<") Mean: "<<0.1*int(10*stats.mean()+5)
          <<"ns  +/-"<<0.1*int(5*(stats.max()-stats.min())+5)<<"ns\n";
}

int main(int argc, char** argv)
{
  vil_image_view<vxl_byte> byte_image(NI,NJ,1);  byte_image.fill(17);
  vil_image_view<float>    float_image(NI,NJ,1); float_image.fill(17);
  int n_loops = 10000;
  int n_pts = 1000;
  vcl_vector<int> x(n_pts),y(n_pts);
  for (int i=0;i<n_pts;++i)
  {
    x[i]=(i*7)%NI;
    y[i]=(i*13)%NJ;
  }

  n_pts = 100;
  n_loops = 10000;
  vcl_cout<<"Times to randomly access image (in nanosecs) [Range= 0.5(max-min)]\n"
          <<"Images of BYTE\n";
  for (int i=1;i<=3;++i)
  {
    compute_stats(i,byte_image,&x[0],&y[0],n_pts,n_loops);
  }
  vcl_cout<<"Images of FLOAT\n";
  for (int i=1;i<=3;++i)
  {
    compute_stats(i,float_image,&x[0],&y[0],n_pts,n_loops);
  }

  vcl_cout<<"Using Bilinear interpolation.\n"
          <<"Images of BYTE\n";
  for (int i=1;i<=3;++i)
  {
    compute_bilin_stats(i,byte_image,n_pts,n_loops);
  }
  vcl_cout<<"Images of FLOAT\n";
  for (int i=1;i<=3;++i)
  {
    compute_bilin_stats(i,float_image,n_pts,n_loops);
  }

  return 0;
}
