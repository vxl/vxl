// This is prip/vdtop/tests/vdtop_kernel_test.cxx

#include <vxl_config.h>

#include <vdtop/vdtop_8_lower_leveling_kernel.h>
#include <vdtop/vdtop_4_lower_homotopic_kernel.h>
#include <vdtop/vdtop_replace_quasi_8_minima.h>
#include <vdtop/vil_canny_deriche_grad_filter.h>

#include <vil/vil_convert.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>

int main(int argc, char* argv[])
{
  vil_image_view<vxl_byte> img,res,res2;
  img= vil_load(argc>1 ? argv[1] : "lena.org.pgm") ;
  vil_image_view<float> gradi ;

  double k=1.5;
  vil_canny_deriche_grad_filter(img,gradi,k) ;
  vil_convert_cast(gradi, res) ;
  vil_save(res,argc>2 ? argv[2] : "lena.grad.pgm");

  vdtop_8_lower_leveling_kernel(res) ;
  //vdtop_replace_quasi_8_minima(res,res2,(vxl_byte)255) ;
  //vil_convert_cast(res2, res) ;
  vil_save(res,argc>3 ? argv[3] : "lena.kernel.pgm");
}
