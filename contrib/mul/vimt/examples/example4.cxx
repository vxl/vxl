//:
// \file
// \brief Example of applying convolutions at arbitrary orientations
// \author Tim Cootes - Manchester
// Actually, we are going to use correlation, which is similar to convolution,
// but does not reverse the kernel.

#include <vxl_config.h> // for vxl_byte
#include <vcl_iostream.h>
#include <vimt/algo/vimt_find_peaks.h>
#include <vimt/algo/vimt_correlate_2d.h>
#include <vimt/vimt_resample_bilin.h>

int main()
{
  vcl_cout<<"Generate an image\n"
          <<"Sample a region from this at an angle (using vimt_resample_bilin)\n"
          <<"Correlate the sampled region with a small filter (using vimt_correlate_2d)\n"
          <<"Locate the peaks in the correlation results (using vimt_find_peaks)\n"
          <<"Because of the cunning way transformations are kept up to date,\n"
          <<"the resulting peak points give the position of the rotated correlation kernel\n"
          <<"in the original image\n";

  vimt_image_2d_of<vxl_byte> image0;
  vimt_image_2d_of<float> fit_image,sample_im;
  image0.image().set_size(10,10);
  image0.image().fill(10);
  image0.image()(3,7)=18;  // One peak

  vcl_cout<<"Original image:\n";
  image0.print_all(vcl_cout);

  // Create simple kernel to detect isolated peaks
  vil_image_view<double> kernel(3,3,1);
  kernel.fill(1.0);
  kernel(1,1)=8.0;
  vgl_point_2d<double> kernel_ref_pt(1,1);


  vimt_resample_bilin(image0,sample_im,vgl_point_2d<double>(3,0),
                      vgl_vector_2d<double>(0.7,0.7),vgl_vector_2d<double>(-0.7,0.7),8,8);

  vcl_cout<<"Result of resampling a region from an image:\n";
  sample_im.print_all(vcl_cout);

  vimt_correlate_2d(sample_im,fit_image,kernel,kernel_ref_pt,float());

  vcl_cout<<"Kernel response image:\n";
  fit_image.print_all(vcl_cout);

  vcl_vector<vgl_point_2d<double> > w_peaks;
  vimt_find_world_peaks_3x3(w_peaks,fit_image);
  for (unsigned i=0;i<w_peaks.size();++i) vcl_cout<<"Peak "<<i<<") "<<w_peaks[i]<<vcl_endl;

  return 0;
}
