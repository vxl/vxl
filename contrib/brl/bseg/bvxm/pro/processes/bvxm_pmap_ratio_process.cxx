//This is brl/bseg/bvxm/pro/processes/bvxm_pmap_ratio_process.cxx
#include "bvxm_pmap_ratio_process.h"
//:
// \file
#include <bprb/bprb_func_process.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_new.h>
#include <vil/vil_image_view_base.h>
#include <vnl/vnl_math.h>
#include <vcl_cassert.h>

//: set input and output types
bool bvxm_pmap_ratio_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_pmap_ratio_process_globals;

  // This process has 3 inputs:
  vcl_vector<vcl_string> input_types_(n_inputs_);
  int i=0;
  input_types_[i++] = "vcl_string";    // path to the prob. map image of LIDAR
  input_types_[i++] = "vcl_string";    // path to the prob. map image of NON_LIDAR
  input_types_[i++] = "vcl_string";    // output path

  return pro.set_input_types(input_types_);
}

//: generates a ratio histogram out of occupancy probability grid
bool bvxm_pmap_ratio_process(bprb_func_process& pro)
{
  using namespace bvxm_pmap_ratio_process_globals;

  if (pro.n_inputs()<n_inputs_)
  {
    vcl_cout << pro.name() << " The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }

  // get the inputs:
  unsigned i = 0;
  vcl_string pmap1 = pro.get_input<vcl_string>(i++);
  vcl_string pmap2 =  pro.get_input<vcl_string>(i++);
  vcl_string path =  pro.get_input<vcl_string>(i++);

  compute(pmap1, pmap2, path);

  return true;
}

bool bvxm_pmap_ratio_process_globals::compute(vcl_string pmap1,vcl_string pmap2, vcl_string path)
{
  vil_image_view_base_sptr lidar_img = vil_load(pmap1.c_str());
  vil_image_view_base_sptr nonlidar_img = vil_load(pmap2.c_str());
  vil_image_resource_sptr ratio_img = vil_new_image_resource(lidar_img->ni(), lidar_img->nj(), 1, VIL_PIXEL_FORMAT_FLOAT);
  assert((lidar_img->ni()==nonlidar_img->ni()) &&
      (lidar_img->nj()==nonlidar_img->nj()));
  float pmax=0.0f;
  for ( unsigned int ni = 0; ni < lidar_img->ni(); ni++ ) {
    for ( unsigned int nj = 0; nj < lidar_img->nj(); nj++ ) {
      if (lidar_img->pixel_format() == VIL_PIXEL_FORMAT_BYTE) {
        if (vil_image_view<unsigned char> *img_view = dynamic_cast<vil_image_view<unsigned char>*>(lidar_img.ptr()))
      {
        float p1 = (*img_view)(ni, nj);
        vil_image_view<unsigned char> *img_view2 = dynamic_cast<vil_image_view<unsigned char>*>(nonlidar_img.ptr());
        float p = 0.0f, p2 = (*img_view2)(ni, nj);
        if ((p1>0) && (p2>0))
          p=p1/p2;
        vil_image_view<float> v = *(ratio_img->get_view());
        v(ni,nj) = p;
      }
      else if (lidar_img->pixel_format() == VIL_PIXEL_FORMAT_FLOAT)
        if (vil_image_view<float> *img_view = dynamic_cast<vil_image_view<float>*>(lidar_img.ptr())) {
        float p1 = (*img_view)(ni, nj);
        vil_image_view<float> *img_view2 = dynamic_cast<vil_image_view<float>*>(nonlidar_img.ptr());
        float p=0.0f, p2 = (*img_view2)(ni, nj);

        if ((p1>0) && (p2>0) && vnl_math::isfinite(p1) && vnl_math::isfinite(p2))
          p=vcl_log10(p1/p2);
        if (p>pmax)
          pmax=p;
        vil_image_view<float> v = *(ratio_img->get_view());
        v(ni,nj) = p;
        }
      }
    }
  }
  vcl_cout << "Pmax=" << pmax << vcl_endl;
  vil_save_image_resource(ratio_img, path.c_str());
  return true;
}
