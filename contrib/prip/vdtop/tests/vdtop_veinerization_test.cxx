// This is prip/vdtop/tests/vdtop_veinerization_test.cxx
#include <vxl_config.h>

#include <vmap/vmap_2_tmap.h>
#include <vdtop/vil_canny_deriche_grad_filter.h>
#include <vdtop/vdtop_well_composed_lower_leveling_kernel.h>
#include <vdtop/vdtop_set_4_veinerization_structure.h>

#include <vil/vil_convert.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_clamp.h>

typedef vmap_2_tmap<> my_map_type ;

int main(int argc, char* argv[])
{
  my_map_type map ;
  vil_image_view<vxl_byte> img,res,res2;
  img= vil_load(argc>1 ? argv[1] : "lena.org.pgm") ;

  vil_image_view<float> gradi ;

  float k=1.0;
  vil_canny_deriche_grad_filter(img,gradi,k) ;
  vil_clamp_below(gradi,5.0f,0.0f) ;
  vil_convert_cast(gradi, res) ;
  vil_save(res,argc>2 ? argv[2] : "lena.grad.pgm");

  vdtop_well_composed_lower_leveling_kernel(res) ;
  vil_save(res,argc>3 ? argv[3] : "lena.kernel.pgm");

  vdtop_set_4_veinerization_structure(map,res) ;

  // removes pendant darts
  my_map_type::contraction_kernel kc(map) ;
  kc.initialise();
  kc.add_1_cycles() ;
  kc.finalise() ;
  map.contraction(kc) ;

  res2.set_size(img.ni(),img.nj()) ;
  res2.fill(0) ;
  for (my_map_type::vertex_iterator v=map.begin_vertex(); v!=map.end_vertex(); ++v)
  {
    res2.top_left_ptr()[map.position(*v)]=255 ;
  }
  vil_save(res2,argc>4 ? argv[4] : "res.pgm") ;
}
