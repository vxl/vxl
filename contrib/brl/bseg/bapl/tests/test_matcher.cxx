#include <testlib/testlib_test.h>
#include <bapl/bapl_affine_transform.h>
#include <bapl/bapl_affine_roi.h>
#include <bapl/bapl_mi_matcher.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>



MAIN( test_matcher )
{
  START ("matcher");

  // Generate an image
  //--------------------------------------------
/*
  unsigned ni=256;
  unsigned nj=256;
  vil_image_view<vxl_byte> scene(ni,nj);
  for (unsigned j=0;j<nj;++j){
    for (unsigned i=0;i<ni;++i){
      scene(i,j) = vxl_byte(((i%8)<4)?0:255);
    }
  }
*/
  vil_image_view<vxl_byte> scene = vil_load("C:/Live/Aq1/6/right/00000.tiff");
  vil_image_view<vxl_byte> scene2 = vil_load("C:/Live/Aq1/6/right/00008.tiff");
  bapl_affine_transform T(1, 0, 0, 1, 100, 230);
  bapl_affine_roi roi(scene, T, 32, 32);
  vil_save(roi.rectified_image(),"region.png");
  bapl_mi_matcher_params params(25, 2.0, 10.0, 3.0, 2.0 );
  bapl_mi_matcher matcher(scene2, roi, params); 

  matcher.generate();
  matcher.generate();
  matcher.generate();
  matcher.generate();

  bapl_affine_roi roi2(scene2, matcher.best_xform(), 32, 32);
  vil_save(roi2.rectified_image(), "match.png");
  
  //testlib_test_begin("identity transformed ROI");
  //testlib_test_perform(vil_image_view_deep_equality(region, crop));


  SUMMARY();
}
