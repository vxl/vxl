//:
// \file
// \brief Example of applying convolutions at arbitrary orientations
// \author Tim Cootes - Manchester

#include <vxl_config.h> // for vxl_byte
#include <vcl_iostream.h>
#include <vimt/algo/vimt_find_peaks.h>
#include <vimt/algo/vimt_convolve_2d.h>
#include <vimt/vimt_resample_bilin.h>

int main(int argc, char** argv)
{
  vcl_cout<<"Generate an image"<<vcl_endl;
  vcl_cout<<"Sample a region from this at an angle (using vimt_resample_bilin)"<<vcl_endl;
  vcl_cout<<"Convolve the sampled region with a small filter (using vimt_convolve_2d)"<<vcl_endl;
  vcl_cout<<"Locate the peaks in the correllation results (using vimt_find_peaks)"<<vcl_endl;
  vcl_cout<<"Because of the cunning way transformations are kept up to date, "<<vcl_endl;
  vcl_cout<<"the resulting peak points give the position of the rotated convolution kernel"<<vcl_endl;
  vcl_cout<<"in the original image"<<vcl_endl;

  vimt_image_2d_of<vxl_byte> image0;
  vimt_image_2d_of<float> fit_image,sample_im;
  image0.image().resize(10,10);
  image0.image().fill(10);
  image0.image()(3,7)=18;  // One peak

  vcl_cout<<"Original image: "<<vcl_endl;
  image0.print_all(vcl_cout);

  // Create simple kernel to detect isolated peaks
  vil2_image_view<double> kernel(3,3,1);
  kernel.fill(1.0);
  kernel(1,1)=8.0;
  vgl_point_2d<double> kernel_ref_pt(1,1);


  vimt_resample_bilin(image0,sample_im,vgl_point_2d<double>(3,0),
                      vgl_vector_2d<double>(0.7,0.7),vgl_vector_2d<double>(-0.7,0.7),8,8);

  vcl_cout<<"Result of resampling a region from an image: "<<vcl_endl;
  sample_im.print_all(vcl_cout);

  vimt_convolve_2d(sample_im,fit_image,kernel,kernel_ref_pt,float());

  vcl_cout<<"Kernel response image: "<<vcl_endl;
  fit_image.print_all(vcl_cout);

  vcl_vector<vgl_point_2d<double> > w_peaks;
  vimt_find_world_peaks_3x3(w_peaks,fit_image);
  for (unsigned i=0;i<w_peaks.size();++i) vcl_cout<<"Peak "<<i<<") "<<w_peaks[i]<<vcl_endl;

  return 0;
}
