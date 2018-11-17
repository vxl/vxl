//This is brl/bseg/bvxm/pro/processes/bvxm_pmap_hist_process.cxx
#include "bvxm_pmap_hist_process.h"
//:
// \file
#include <bprb/bprb_func_process.h>

#include <bvxm/bvxm_world_params.h>
#include <bvxm/bvxm_voxel_world.h>

#include <vil/vil_load.h>
#include <vil/vil_image_view_base.h>
#include <bsta/bsta_histogram.h>

//: set input and output types
bool bvxm_pmap_hist_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_pmap_hist_process_globals;

// This process has 2 inputs:
  //input[0]: The voxel world
  //input[1]: The path for the output file
  std::vector<std::string> input_types_(n_inputs_);
  int i=0;
  input_types_[i++] = "vcl_string";    // path to the prob. map image
  input_types_[i++] = "vcl_string";    // output path
  return pro.set_input_types(input_types_);
}

bool bvxm_pmap_hist_process(bprb_func_process& pro)
{
  using namespace bvxm_pmap_hist_process_globals;

  //check number of inputs
  if (pro.n_inputs()<n_inputs_)
  {
    std::cout << pro.name() << " The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  // get the inputs:

  //path to the prob. map image
  unsigned i = 0;
  std::string pmap = pro.get_input<std::string>(i++);
  //path for the output
  std::string path = pro.get_input<std::string>(i++);

  compute(pmap, path);

  return true;
}

bool bvxm_pmap_hist_process_globals::compute(const std::string& pmap,  const std::string& path)
{
  vil_image_view_base_sptr img = vil_load(pmap.c_str());
  bsta_histogram<double> hist(0.0, 20.0, 60);

  float p = -1.0f;
  for ( unsigned int ni = 0; ni < img->ni(); ++ni ){
    for ( unsigned int nj = 0; nj < img->nj(); ++nj ){
      if (img->pixel_format() == VIL_PIXEL_FORMAT_BYTE) {
        if (auto *img_view = dynamic_cast<vil_image_view<unsigned char>*>(img.ptr()))
          p = (*img_view)(ni, nj);
      }
      else if (img->pixel_format() == VIL_PIXEL_FORMAT_FLOAT) {
        if (auto *img_view = dynamic_cast<vil_image_view<float>*>(img.ptr()))
          p = (*img_view)(ni, nj);
      }

      hist.upcount(p, 1);
    }
  }

  std::ofstream f(path.c_str());
  hist.print(f);
  return true;
}
