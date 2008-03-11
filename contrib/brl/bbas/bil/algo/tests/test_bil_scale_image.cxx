#include <testlib/testlib_test.h>
#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include <bil/algo/bil_scale_image.h>
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>
#include <vil/vil_convert.h>


static void save_and_display(const vil_image_view<float> img, const vcl_string& name)
{
  vil_image_view<vxl_byte> save_img;
  vil_convert_stretch_range(img, save_img);
  vil_save(save_img, vcl_string(name+".png").c_str());
  vcl_cout << "<DartMeasurementFile name=\""<<name<<"\" type=\"image/png\"> "
           << name << ".png </DartMeasurementFile>" <<vcl_endl;
}


MAIN( test_bil_scale_image )
{
  START ("scale_image");

  float sigma = 64.0f;
  unsigned int img_size = 256;
  vil_image_view<float> gaussian(img_size,img_size);
  for (unsigned int i=0; i<img_size; ++i) {
    for (unsigned int j=0; j<img_size; ++j) {
      float x = (i - float(img_size/2));
      float y = (j - float(img_size/2));
      gaussian(i,j) = vcl_exp(-(x*x+y*y)/(2.0*sigma*sigma));
    }
  }

  save_and_display(gaussian,"original");

  bil_scale_image<float> gs_test(3,4,0.5);
  TEST("levels()",gs_test.levels(),3);
  TEST("octaves()",gs_test.octaves(),4);
  TEST("closest_scale(scale)",gs_test.closest_scale(2.01f),2.0f);

  int octaves = 5;
  int levels = 3;
  float k = vcl_pow(2.0,1.0/double(levels));
  bil_scale_image<float> gauss_scale(levels, octaves);

  bil_scale_image<float> dog_scale;
  gauss_scale.build_gaussian(gaussian, &dog_scale);

  save_and_display(gauss_scale(0,1),"gauss_2");
  save_and_display(dog_scale(0,1),"dog_2");

  float k2 = k*k;
  float sigma2 = sigma*sigma;

  bool good_approx = true;
  vcl_cout << "  scale \t actual \t expected \t error"<<vcl_endl;
  for (unsigned int oct=0; oct<gauss_scale.octaves(); ++oct) {
    for (unsigned int lvl=0; lvl<gauss_scale.levels(); ++lvl) {
    float x = img_size/2;
    float y = img_size/2;

    float scale = gauss_scale.scale(oct,lvl);
    const vil_image_view<float>& image = dog_scale(oct,lvl);
    float img_scale = gauss_scale.image_scale(oct);
    x /= img_scale;  y /= img_scale;
    float scale2 = scale*scale;

    // FIXME - explain what this should be
    float expected = -sigma2*scale2*(-1+k2)/(k2*scale2+sigma2)/(sigma2+scale2);
    float actual = image(int(x),int(y));

    vcl_cout <<"  "<< scale <<"     \t " << actual <<"\t "
             << expected <<"\t " << (actual-expected) <<"\t "<<vcl_endl;

    good_approx = good_approx && vcl_fabs(actual-expected) < 1e-3;
    }
  }

  TEST("Value Test",good_approx,true);


  SUMMARY();
}
