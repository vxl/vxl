//:
// \file
// \brief Tool to test performance of different methods of accessing image data.
//  When run, tries a variety of different approaches and reports their timings.
//  Useful to try it on different platforms to see how different optimisers perform.
// \author Tim Cootes

#include <vcl_iostream.h>
#include <vxl_config.h> // for vxl_byte
#include <vil/vil_image_view.h>
#include <vcl_ctime.h>
#include <mbl/mbl_stats_1d.h>
#include <vil/algo/vil_gauss_reduce.h>
#include <mil/mil_image_2d_of.h>
#include <mil/mil_gaussian_pyramid_builder_2d_general.h>

double vil_method(unsigned n_loops, unsigned nx, unsigned ny)
{
  vil_image_view<vxl_byte> input(nx, ny);
  vil_image_view<vxl_byte> output, tempa, tempb;
  for (unsigned n=0;n<n_loops;++n)
  {
    for (unsigned j=0;j<input.nj();++j)
      for (unsigned i=0;i<input.ni();++i)
        input(i,j) = vxl_byte(i+j);
  }
  vil_gauss_reduce_params params(1.2);
  vcl_time_t t0=vcl_clock();
  vil_gauss_reduce_general(input, output, params);

  vcl_time_t t1=vcl_clock();
  return 1000000*(double(t1)-double(t0))/(n_loops*CLOCKS_PER_SEC);
}

double mil_method(unsigned n_loops, unsigned nx, unsigned ny)
{
  mil_image_2d_of<vxl_byte> input(nx, ny);
  mil_image_2d_of<vxl_byte> output, worka, workb;
  for (unsigned n=0;n<n_loops;++n)
  {
    for (int j=0;j<input.ny();++j)
      for (int i=0;i<input.nx();++i)
        input(i,j) = vxl_byte(i+j);
  }
  mil_gaussian_pyramid_builder_2d_general<vxl_byte> builder(1.2);

  worka.resize(nx,ny);
  workb.resize(nx,ny);
  output.resize((int) (nx/1.2+0.5), (int) (ny/1.2+0.5));


  vcl_time_t t0=vcl_clock();

  mil_gaussian_pyramid_builder_2d_general<vxl_byte>::gauss_reduce(
                      output.plane(0), output.ystep(),
                      input.plane(0),
                      input.nx(), input.ny(),
                      output.nx(), output.ny(),
                      input.ystep(),
                      worka.plane(0), workb.plane(0), worka.ystep(),
                      &builder);


  vcl_time_t t1=vcl_clock();
  return 1000000*(double(t1)-double(t0))/(n_loops*CLOCKS_PER_SEC);
}


int main(int argc, char** argv)
{
  unsigned n_loops = 20, nx = 320, ny = 240;
  vcl_cout<<"Times to do a gaussion subsample of a "<<nx<<" x "<<ny
          <<" image of 1 planes (in microsecs) [Range= 0.5(max-min)]\n";

  mbl_stats_1d stats;
  for (int j=0;j<10;++j) stats.obs(vil_method(n_loops, nx, ny));
  vcl_cout<<"Using operator():        Mean: "<<int(stats.mean()+0.5)
          <<"us  +/-"<<int(0.5*(stats.max()-stats.min())+0.5)<<"us\n";

  stats.clear();
  for (int j=0;j<10;++j) stats.obs(mil_method(n_loops, nx, ny));
  vcl_cout<<"Using explicit pointers: Mean: "<<int(stats.mean()+0.5)
          <<"us  +/-"<<int(0.5*(stats.max()-stats.min())+0.5)<<"us\n";

  return 0;
}
