// This is mul/vimy/tests/test_find_peaks.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vxl_config.h> // for vxl_byte
#include <vimt/algo/vimt_find_peaks.h>
#include <vimt/algo/vimt_convolve_2d.h>
#include <vimt/vimt_resample_bilin.h>

void test_convolve_2d_byte()
{
  vimt_image_2d_of<vxl_byte> image0;
  vimt_image_2d_of<float> fit_image,sample_im;
  image0.image().set_size(10,10);
  image0.image().fill(10);
  image0.image()(3,7)=18;  // One peak

  vil2_image_view<double> kernel(3,3,1);
  kernel.fill(1.0);
  kernel(1,1)=8.0;
  vgl_point_2d<double> kernel_ref_pt(1,1);

  vimt_convolve_2d(image0,fit_image,kernel,kernel_ref_pt,float());

  TEST("Fits width",fit_image.image().ni(),8);
  TEST("Fits height",fit_image.image().nj(),8);

  vcl_vector<vgl_point_2d<double> > w_peaks;
  vimt_find_world_peaks_3x3(w_peaks,fit_image);

  TEST("Number of peaks",w_peaks.size(),1);
  TEST_NEAR("Peak 0",(w_peaks[0]-vgl_point_2d<double>(3,7)).sqr_length(),0,1e-12);

  vcl_cout<<"Sample a diagonal image and look for peaks in that."<<vcl_endl;

  vimt_resample_bilin(image0,sample_im,vgl_point_2d<double>(3,0),
                      vgl_vector_2d<double>(0.7,0.7),vgl_vector_2d<double>(-0.7,0.7),8,8);

//  sample_im.print_all(vcl_cout);

  vimt_convolve_2d(sample_im,fit_image,kernel,kernel_ref_pt,float());

//  fit_image.print_all(vcl_cout);

  TEST("Fits width",fit_image.image().ni(),6);
  TEST("Fits height",fit_image.image().nj(),6);

  vcl_vector<vgl_point_2d<double> > w_peaks2;
  vimt_find_world_peaks_3x3(w_peaks2,fit_image);
  TEST("Number of peaks",w_peaks2.size(),1);
  if (w_peaks2.size()>0)
  {
    TEST_NEAR("Peak 0",(w_peaks2[0]-vgl_point_2d<double>(3,7)).sqr_length(),0,1e-12);
  }
}

MAIN( test_convolve_2d )
{
  START( "vimt_convolve_2d" );

  test_convolve_2d_byte();

  SUMMARY();
}
