//This is brl/bseg/bvxm/pro/processes/bvxm_pmap_hist_process.cxx
//:
// \file
// \brief A class for generating a histogram out of occupancy probability grid
//        -  Input:
//             - bvxm_voxel_world_sptr
//             - vcl_string             --path for the text file of the histogram
//
//        -  Output:
//             - none
//
// \author  Gamze D. Tunali
// \date    05/22/2008
// \verbatim
//  Modifications
//   Isabel Restrepo - 1/27/09 - converted process-class to functions which is the new design for processes.
// \endverbatim


#include <bprb/bprb_func_process.h>

#include <bvxm/bvxm_world_params.h>
#include <bvxm/bvxm_voxel_world.h>

#include <vil/vil_load.h>
#include <vil/vil_image_view_base.h>
#include <bsta/bsta_histogram.h>

//: globals
namespace bvxm_pmap_hist_process_globals
{
  const unsigned n_inputs_ = 2;
  const unsigned n_outputs_ = 0;
  
  //functions
  bool compute(vcl_string pmap,  vcl_string path);
}

//: set input and output types
bool bvxm_pmap_hist_process_init(bprb_func_process& pro)
{
  using namespace bvxm_pmap_hist_process_globals;
  
// This process has 2 inputs:
  //input[0]: The voxel world
  //input[1]: The path for the output file
  vcl_vector<vcl_string> input_types_(n_inputs_);
  int i=0;
  input_types_[i++] = "vcl_string";    // path to the prob. map image
  input_types_[i++] = "vcl_string";    // output path
  if(!pro.set_input_types(input_types_))
    return false;
  
  return true;

}

bool bvxm_pmap_hist_process(bprb_func_process& pro)
{
  using namespace bvxm_pmap_hist_process_globals;
  
  //check number of inputs
  if(pro.n_inputs()<n_inputs_)
  {
    vcl_cout << pro.name() << " The input number should be " << n_inputs_<< vcl_endl;
    return false; 
  }
  // get the inputs:

  //path to the prob. map image
  unsigned i = 0;
  vcl_string pmap = pro.get_input<vcl_string>(i++);
  //path for the output
  vcl_string path = pro.get_input<vcl_string>(i++);

  compute(pmap, path);

  return true;
}

bool bvxm_pmap_hist_process_globals::compute(vcl_string pmap,  vcl_string path)
{
  vil_image_view_base_sptr img = vil_load(pmap.c_str());
  bsta_histogram<double> hist(0.0, 20.0, 60);

  float p = -1.0f;
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
