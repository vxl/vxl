#include <testlib/testlib_test.h>
#include <bapl/bapl_affine_transform.h>
#include <bapl/bapl_affine_roi.h>
#include <vil/vil_crop.h>
#include <vil/vil_save.h>



MAIN( test_roi )
{
  START ("affine ROI");

  // Generate an image
  //--------------------------------------------
  unsigned ni=256;
  unsigned nj=256;
  vil_image_view<vxl_byte> scene(ni,nj);
  for (unsigned j=0;j<nj;++j){
    for (unsigned i=0;i<ni;++i){
      scene(i,j) = vxl_byte(((i%8)<4)?0:255);
    }
  }

  bapl_affine_transform T;
  bapl_affine_roi roi(scene, T, 32, 32);

  vil_image_view<vxl_byte> region = roi.rectified_image();
  vil_image_view<vxl_byte> crop = vil_crop(scene,0,32,0,32);

  vil_save(region, "test_region.png");
  
  testlib_test_begin("identity transformed ROI");
  testlib_test_perform(vil_image_view_deep_equality(region, crop));


  SUMMARY();
}
