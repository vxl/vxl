//:
// \file
// \brief Tool to compare standard algorithms/functions in vil1 and vil
// \author Ian Scott
// Will compare the following operations:
// Deep copy, fill, 1d convolve, 2d convolve, correlation, force to rgb, force to float, pnm read/write
// using (where appropriate) byte, float and rgb byte pixels.
// These timings will be used to optimise vil.

#include <vcl_iostream.h>
#include <vxl_config.h> // for imT
#include <vil/vil_image_view.h>
#include <vil/vil_copy.h>
#include <vil/vil_convert.h>
#include <vil/algo/vil_convolve_1d.h>
#include <vcl_ctime.h>
#include <mbl/mbl_stats_1d.h>
#include <vil1/vil1_memory_image_of.h>
#include <vil1/vil1_rgb.h>
#include <vil/vil_rgb.h>
#include <vil1/vil1_copy.h>
#include <vil1/vil1_convolve.h>
#include <vil1/vil1_image_as.h>

const unsigned NI=256;
const unsigned NJ=256;

template <class T>
double fill(vil_image_view<T>& image, int n_loops, bool print)
{
  if (print)
    vcl_cout << "Time to fill a " << image.ni() << '*' << image.nj()
             << " vil image of " << vil_pixel_format_of(T()) << vcl_endl;
  vcl_time_t t0=vcl_clock();
  for (int n=0;n<n_loops;++n)
    image.fill(T(n));
  vcl_time_t t1=vcl_clock();
  return 1000000*(double(t1)-double(t0))/(n_loops*CLOCKS_PER_SEC);
}

template <class T>
double fill(vil1_memory_image_of<T>& image, int n_loops, bool print)
{
  if (print)
    vcl_cout << "Time to fill a " << image.width() << '*' << image.height()
             << " vil1 image of " << vil_pixel_format_of(T()) << vcl_endl;
  vcl_time_t t0=vcl_clock();
  for (int n=0;n<n_loops;++n)
    image.fill(T(n));
  vcl_time_t t1=vcl_clock();
  return 1000000*(double(t1)-double(t0))/(n_loops*CLOCKS_PER_SEC);
}

template <class T>
double copy(vil_image_view<T>& image, int n_loops, bool print)
{
  vil_image_view<T> dest;

  if (print)
    vcl_cout << "Time to copy a " << image.ni() << '*' << image.nj()
             << " vil image of " << vil_pixel_format_of(T()) << vcl_endl;
  vcl_time_t t0=vcl_clock();
  for (int n=0;n<n_loops;++n)
    dest = vil_copy_deep(image);
  vcl_time_t t1=vcl_clock();
  return 1000000*(double(t1)-double(t0))/(n_loops*CLOCKS_PER_SEC);
}

template <class T>
double copy(vil1_memory_image_of<T>& image, int n_loops, bool print)
{
  vil1_memory_image_of<T> dest;
  if (print)
    vcl_cout << "Time to copy a " << image.width() << '*' << image.height()
             << " vil1 image of " << vil_pixel_format_of(T()) << vcl_endl;
  vcl_time_t t0=vcl_clock();
  for (int n=0;n<n_loops;++n)
    dest=vil1_copy(image);
  vcl_time_t t1=vcl_clock();
  return 1000000*(double(t1)-double(t0))/(n_loops*CLOCKS_PER_SEC);
}
template <class T>
double force_to_rgb_byte(vil_image_view<T>& image, int n_loops, bool print)
{
  vil_image_view<vil_rgb<vxl_byte> > dest;

  if (print)
    vcl_cout << "Time to convert to rgb a " << image.ni() << '*' << image.nj()
             << " vil image of " << vil_pixel_format_of(T()) << vcl_endl;
  vcl_time_t t0=vcl_clock();
  for (int n=0;n<n_loops;++n)
    vil_convert_cast(image, dest);
  vcl_time_t t1=vcl_clock();
  return 1000000*(double(t1)-double(t0))/(n_loops*CLOCKS_PER_SEC);
}

template <class T>
double force_to_rgb_byte(vil1_memory_image_of<T>& image, int n_loops, bool print)
{
  vil1_memory_image_of<vil1_rgb<vxl_byte> > dest;
  if (print)
    vcl_cout << "Time to convert to rgb a " << image.width() << '*' << image.height()
             << " vil1 image of " << vil_pixel_format_of(T()) << vcl_endl;
  vcl_time_t t0=vcl_clock();
  for (int n=0;n<n_loops;++n)
    dest= vil1_image_as_rgb_byte(image);
  vcl_time_t t1=vcl_clock();
  return 1000000*(double(t1)-double(t0))/(n_loops*CLOCKS_PER_SEC);
}

double conv1d(vil_image_view<vxl_byte>& image, int n_loops, bool print)
{
  vil_image_view<float> dest;

  if (print)
    vcl_cout << "Time to 1d convolve a " << image.ni() << '*' << image.nj()
             << "vil image of vxl_byte\n";
  float kernel[] = {-1.0f, 0.5f, 1.0f, 0.5f, -1.0f};
  vcl_time_t t0=vcl_clock();
  for (int n=0;n<n_loops;++n)
    vil_convolve_1d(image, dest, kernel+2, -2, 2, float(), vil_convolve_trim, vil_convolve_constant_extend);
  vcl_time_t t1=vcl_clock();
  return 1000000*(double(t1)-double(t0))/(n_loops*CLOCKS_PER_SEC);
}

double conv1d(vil_image_view<float>& image, int n_loops, bool print)
{
  vil_image_view<float> dest;

  if (print)
    vcl_cout << "Time to 1d convolve a " << image.ni() << '*' << image.nj()
             << "vil image of float\n";
  float kernel[] = {-1.0f, 0.5f, 1.0f, 0.5f, -1.0f};
  vcl_time_t t0=vcl_clock();
  for (int n=0;n<n_loops;++n)
    vil_convolve_1d(image, dest, kernel+2, -2, 2, float(), vil_convolve_trim, vil_convolve_constant_extend);
  vcl_time_t t1=vcl_clock();
  return 1000000*(double(t1)-double(t0))/(n_loops*CLOCKS_PER_SEC);
}

double conv1d(vil_image_view<vil_rgb<vxl_byte> >& image, int n_loops, bool print)
{
  return 0.0;
}

double conv1d(vil1_memory_image_of<vxl_byte>& image, int n_loops, bool print)
{
  vil1_memory_image_of<float> dest;
  if (print)
    vcl_cout << "Time to 1d convolve a " << image.width() << '*' << image.height()
             << " vil1 image of vxl_byte\n";
  float kernel[] = {-1.0f, 0.5f, 1.0f, 0.5f, -1.0f};
  vil1_convolve_signal_1d<float const> K(kernel, 0, 2, 4);
  vcl_time_t t0=vcl_clock();
  for (int n=0;n<n_loops;++n)
  {
    dest.resize(image.width(), image.height());
    vil1_convolve_1d_x(K,
                       vil1_convolve_signal_2d<vxl_byte const>(image.row_array(), 0, 0, image.width(), 0, 0, image.height()),
                       (float *)0,
                       vil1_convolve_signal_2d<float>(dest.row_array(), 0, 0, image.width(), 0, 0, image.height()),
                       vil1_convolve_trim,  vil1_convolve_constant_extend);
  }
  vcl_time_t t1=vcl_clock();
  return 1000000*(double(t1)-double(t0))/(n_loops*CLOCKS_PER_SEC);
}

double conv1d(vil1_memory_image_of<float>& image, int n_loops, bool print)
{
  vil1_memory_image_of<float> dest;
  if (print)
    vcl_cout << "Time to 1d convolve a " << image.width() << '*' << image.height()
             << " vil1 image of float\n";
  float kernel[] = {-1.0f, 0.5f, 1.0f, 0.5f, -1.0f};
  vil1_convolve_signal_1d<float const> K(kernel, 0, 2, 4);
  vcl_time_t t0=vcl_clock();
  for (int n=0;n<n_loops;++n)
  {
    dest.resize(image.width(), image.height());
    vil1_convolve_1d_x(K,
                       vil1_convolve_signal_2d<float const>(image.row_array(), 0, 0, image.width(), 0, 0, image.height()),
                       (float *)0,
                       vil1_convolve_signal_2d<float>(dest.row_array(), 0, 0, image.width(), 0, 0, image.height()),
                       vil1_convolve_trim,  vil1_convolve_constant_extend);
  }
  vcl_time_t t1=vcl_clock();
  return 1000000*(double(t1)-double(t0))/(n_loops*CLOCKS_PER_SEC);
}

double conv1d(vil1_memory_image_of<vil1_rgb<vxl_byte> >& image, int n_loops, bool print)
{
  return 0;
}

template <class imT>
double method(int i, imT& image, int n_loops, bool print)
{
  double t;
  switch (i)
  {
    case 1 : t=fill(image,n_loops, print); break;
    case 2 : t=copy(image,n_loops, print); break;
    case 3 : t=conv1d(image,n_loops, print); break;
    case 4 : t=force_to_rgb_byte(image,n_loops, print); break;
    default: t=-1;
  }
  return t;
}

template <class imT>
void compute_stats(int i, imT& image, int n_loops)
{
  mbl_stats_1d stats;
  for (int j=0;j<10;++j) stats.obs(method(i,image,n_loops,j==0));
  vcl_cout<<"  Mean: "<<int(stats.mean()+0.5)
          <<"us  +/-"<<int(0.5*(stats.max()-stats.min())+0.5)<<"us\n";
}

int main(int argc, char** argv)
{
  vil1_memory_image_of<vxl_byte> byte_1image(NI,NJ);
  vil1_memory_image_of<float>    float_1image(NI,NJ);
  vil1_memory_image_of<vil1_rgb<vxl_byte> >    rgb_1image(NI,NJ);
  vil_image_view<vxl_byte> byte_2image(NI,NJ);
  vil_image_view<float>    float_2image(NI,NJ);
  vil_image_view<vil_rgb<vxl_byte> >    rgb_2image(NI,NJ);
  vcl_cout<<"Times to process a "<<NI<<" x "<<NJ
          <<" image (in microsecs) [Range= 0.5(max-min)]\n";

  // Estimate time so we can set n_loops appropriately
  double t = fill(byte_1image,5000,false);

  int n_loops = int(200000/t); // Number required for about half a sec running time
  vcl_cout << "Testing using n_loops = " << n_loops << vcl_endl << vcl_endl
           << "Float alignment ";
  if ((((unsigned long)(void*)float_2image.top_left_ptr()) % 4ul)>0)
    vcl_cout << "not on 4-byte boundary\n";
  else
    vcl_cout << "on 4-byte boundary\n";

  compute_stats(1,byte_1image,n_loops);
  compute_stats(1,byte_2image,n_loops);
  vcl_cout<<"------------------------------\n";
  compute_stats(1,float_1image,n_loops);
  compute_stats(1,float_2image,n_loops);
  vcl_cout<<"------------------------------\n";
  compute_stats(1,rgb_1image,n_loops);
  compute_stats(1,rgb_2image,n_loops);
  vcl_cout<<"------------------------------\n\n\n";

  compute_stats(2,byte_1image,n_loops);
  compute_stats(2,byte_2image,n_loops);
  vcl_cout<<"------------------------------\n";
  compute_stats(2,float_1image,1+n_loops/7);
  compute_stats(2,float_2image,1+n_loops/3);
  vcl_cout<<"------------------------------\n";
  compute_stats(2,rgb_1image,1+n_loops/4);
  compute_stats(2,rgb_2image,1+n_loops/2);
  vcl_cout<<"------------------------------\n\n\n";

  compute_stats(3,byte_1image,1+n_loops/20);
  compute_stats(3,byte_2image,1+n_loops/8);
  vcl_cout<<"------------------------------\n";
  compute_stats(3,float_1image,1+n_loops/20);
  compute_stats(3,float_2image,1+n_loops/9);
  vcl_cout<<"------------------------------\n\n\n";

  compute_stats(4,byte_1image,1+n_loops/2);
  compute_stats(4,byte_2image,1+n_loops);
  vcl_cout<<"------------------------------\n";
  compute_stats(4,float_1image,1+n_loops/33);
  compute_stats(4,float_2image,1+n_loops/12);
  vcl_cout<<"------------------------------\n";
  compute_stats(4,rgb_1image,1+n_loops/2);
  vcl_cout << "The next one is very fast because vil realises that you are trying\n"
           << "to view an image as its existing type and so does a shallow copy\n";
  compute_stats(4,rgb_2image,1+n_loops);
  vcl_cout<<"------------------------------\n";

  return 0;
}
