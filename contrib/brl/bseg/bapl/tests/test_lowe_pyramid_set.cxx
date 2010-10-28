#include <testlib/testlib_test.h>
#include <vcl_cmath.h>
#include <bapl/bapl_lowe_pyramid_set.h>
#include <vil/vil_image_view.h>
#include <vil/vil_new.h>
#include <vil/vil_save.h>
#include <vil/vil_convert.h>


MAIN( test_lowe_pyramid_set )
{
  START ("Lowe Pyramid Set");

  float sigma = 8.0f;
  vil_image_view<float> gaussian(512,512);
  for(int i=0; i<512; ++i){
    for(int j=0; j<512; ++j){
      float x = (float)(i - 256);
      float y = (float)(j - 256);
      gaussian(i,j) = (float)vcl_exp(-(x*x+y*y)/(2.0*sigma*sigma));
    }
  }

  vil_image_view<vxl_byte> save_img;
  vil_convert_stretch_range(gaussian, save_img);
  vil_save(save_img, "gaussian_test.png");

  vil_image_resource_sptr gaussian_sptr = vil_new_image_resource_of_view(save_img);
  int octaves = 7;
  int levels = 3;
  float k = (float)vcl_pow(2.0,1.0/double(levels));
  bapl_lowe_pyramid_set pyramids(gaussian_sptr, levels, octaves);

  float k2 = k*k;
  float sigma2 = sigma*sigma;

  bool good_approx = true;
  vcl_cout << "  scale \t actual \t expected \t error"<<vcl_endl;
  for(int i=0; i<octaves*levels; ++i){
    float x = 256;
    float y = 256;
    float ps = vcl_pow(2.0f,(float)(i/levels)-1);
    float scale = ps * (vcl_pow(2.0f,(i%levels)/float(levels)));
    
    float actual_scale;
    vil_image_view<float> image = pyramids.dog_at(scale,&actual_scale);
    x /= actual_scale;  y /= actual_scale;
    float scale2 = scale*scale;

    float expected = -sigma2*scale2*(-1+k2)/(k2*scale2+sigma2)/(sigma2+scale2);
    float actual = image(int(x),int(y));

    vcl_cout <<"  "<< scale <<"     \t " << actual <<"\t "
             << expected <<"\t " << (actual-expected) <<"\t "<<vcl_endl;

    good_approx = good_approx && vcl_fabs(actual-expected) < 1e-2;
  }

  TEST("Pyramid Test",good_approx,true);


  SUMMARY();
}
