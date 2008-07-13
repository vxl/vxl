#include "bvxm_pmap_hist_process.h"

#include <vil/vil_load.h>
#include <vil/vil_image_view_base.h>
#include <bprb/bprb_parameters.h>
#include <bsta/bsta_histogram.h>

bvxm_pmap_hist_process::bvxm_pmap_hist_process()
{
  // This process has 2 inputs:
  //input[0]: The voxel world
  //input[1]: The path for the output file
  input_data_.resize(2, brdb_value_sptr(0));
  input_types_.resize(2);

  int i=0;
  input_types_[i++] = "vcl_string";    // path to the prob. map image
  input_types_[i++] = "vcl_string";    // output path

  //output
  output_data_.resize(0,brdb_value_sptr(0));
  output_types_.resize(0);
}

bool bvxm_pmap_hist_process::execute()
{
  // Sanity check
  if (!this->verify_inputs())
    return false;

  // get the inputs:

  //voxel_world1
  brdb_value_t<vcl_string >* input0 =
    static_cast<brdb_value_t<vcl_string >* >(input_data_[0].ptr());
  vcl_string pmap = input0->value();

  //path for the output
  brdb_value_t<vcl_string >* input1 =
    static_cast<brdb_value_t<vcl_string >* >(input_data_[1].ptr());
  vcl_string path = input1->value();

  compute(pmap, path);

  return true;
}

bool bvxm_pmap_hist_process::compute(vcl_string pmap,
                                     vcl_string path)
{
  vil_image_view_base_sptr img = vil_load(pmap.c_str());
  bsta_histogram<double> hist(0.0, 20.0, 60);

  float p;
  for ( unsigned int ni = 0; ni < img->ni(); ++ni ){
    for ( unsigned int nj = 0; nj < img->nj(); ++nj ){
      if (img->pixel_format() == VIL_PIXEL_FORMAT_BYTE) {
        if (vil_image_view<unsigned char> *img_view = dynamic_cast<vil_image_view<unsigned char>*>(img.ptr()))
          p = (*img_view)(ni, nj);
      }
      else if (img->pixel_format() == VIL_PIXEL_FORMAT_FLOAT) {
        if (vil_image_view<float> *img_view = dynamic_cast<vil_image_view<float>*>(img.ptr()))
          p = (*img_view)(ni, nj);
      }

      hist.upcount(p, 1);
    }
  }

  vcl_ofstream f(path.c_str());
  hist.print(f);
  return true;
}
