#include "bvxm_pmap_ratio_process.h"

#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_new.h>
#include <vil/vil_image_view_base.h>
#include <vnl/vnl_math.h>
#include <bprb/bprb_parameters.h>
#include <vcl_cassert.h>

bvxm_pmap_ratio_process::bvxm_pmap_ratio_process()
{
  // This process has 2 inputs:
  //input[0]: The voxel world
  //input[1]: The path for the output file
  input_data_.resize(3, brdb_value_sptr(0));
  input_types_.resize(3);

  int i=0;
  input_types_[i++] = "vcl_string";    // path to the prob. map image of LIDAR
  input_types_[i++] = "vcl_string";    // path to the prob. map image of NON_LIDAR
  input_types_[i++] = "vcl_string";    // output path


  //output
  output_data_.resize(0,brdb_value_sptr(0));
  output_types_.resize(0);
}

bool bvxm_pmap_ratio_process::execute()
{
  // Sanity check
  if (!this->verify_inputs())
    return false;

  // get the inputs:

  //voxel_world1
  brdb_value_t<vcl_string >* input0 =
    static_cast<brdb_value_t<vcl_string >* >(input_data_[0].ptr());
  vcl_string pmap1 = input0->value();

  //voxel_world2
  brdb_value_t<vcl_string >* input1 =
    static_cast<brdb_value_t<vcl_string >* >(input_data_[1].ptr());
  vcl_string pmap2 = input1->value();

  //path
  brdb_value_t<vcl_string >* input2 =
    static_cast<brdb_value_t<vcl_string >* >(input_data_[2].ptr());
  vcl_string path = input2->value();

  compute(pmap1, pmap2, path);

  return true;
}

bool bvxm_pmap_ratio_process::compute(vcl_string pmap1,
                                      vcl_string pmap2,
                                      vcl_string path)
{
  vil_image_view_base_sptr lidar_img = vil_load(pmap1.c_str());
  vil_image_view_base_sptr nonlidar_img = vil_load(pmap2.c_str());
  vil_image_resource_sptr ratio_img = vil_new_image_resource(lidar_img->ni(), lidar_img->nj(), 1, VIL_PIXEL_FORMAT_FLOAT);
  assert((lidar_img->ni()==nonlidar_img->ni()) &&
         (lidar_img->nj()==nonlidar_img->nj()));
  float pmax=0.0f;
  for ( unsigned int ni = 0; ni < lidar_img->ni(); ni++ ){
    for ( unsigned int nj = 0; nj < lidar_img->nj(); nj++ ){
      if (lidar_img->pixel_format() == VIL_PIXEL_FORMAT_BYTE) {
        if (vil_image_view<unsigned char> *img_view = dynamic_cast<vil_image_view<unsigned char>*>(lidar_img.ptr()))
          float p1 = (*img_view)(ni, nj);
          vil_image_view<unsigned char> *img_view2 = dynamic_cast<vil_image_view<unsigned char>*>(nonlidar_img.ptr());
          float p = 0.0f, p2 = (*img_view2)(ni, nj);
          if ((p1>0) && (p2>0))
            p=p1/p2;
          vil_image_view<float> v = *(ratio_img->get_view());
          v(ni,nj) = p;
      }
      else if (lidar_img->pixel_format() == VIL_PIXEL_FORMAT_FLOAT) {
        if (vil_image_view<float> *img_view = dynamic_cast<vil_image_view<float>*>(lidar_img.ptr()))
          float p1 = (*img_view)(ni, nj);
          vil_image_view<float> *img_view2 = dynamic_cast<vil_image_view<float>*>(nonlidar_img.ptr());
          float p=0.0f, p2 = (*img_view2)(ni, nj);

          if ((p1>0) && (p2>0) && vnl_math_isfinite(p1) && vnl_math_isfinite(p2))
            p=vcl_log10(p1/p2);
          if (p>pmax)
            pmax=p;
          vil_image_view<float> v = *(ratio_img->get_view());
          v(ni,nj) = p;
      }
    }
  }
  vcl_cout << "Pmax=" << pmax << vcl_endl;
  vil_save_image_resource(ratio_img, path.c_str());
  return true;
}
