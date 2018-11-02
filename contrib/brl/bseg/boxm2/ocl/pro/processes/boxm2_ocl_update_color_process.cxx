// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_update_color_process.cxx
#include <iostream>
#include <algorithm>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for updating a color model
//
// \author Vishal Jain
// \date Mar 25, 2011

#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/ocl/algo/boxm2_ocl_update_color.h>
#include <bocl/bocl_device.h>
#include <brdb/brdb_value.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

namespace boxm2_ocl_update_color_process_globals
{
  constexpr unsigned int n_inputs_ = 8;
  constexpr unsigned int n_outputs_ = 0;
}

bool boxm2_ocl_update_color_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_update_color_process_globals;

  //process takes 8 inputs (of which the three last ones are optional):
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "boxm2_opencl_cache_sptr";
  input_types_[3] = "vpgl_camera_double_sptr";      //input camera
  input_types_[4] = "vil_image_view_base_sptr";     //input image
  input_types_[5] = "vcl_string";
  input_types_[6] = "vcl_string";
  input_types_[7] = "bool";                         //do_update_alpha/don't update alpha

  // process has no outputs
  std::vector<std::string>  output_types_(n_outputs_);
  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);

  //set defaults inputs
  brdb_value_sptr idx = new brdb_value_t<std::string>("");
  brdb_value_sptr updateAlpha = new brdb_value_t<bool>(true);
  pro.set_input(5, idx);
  pro.set_input(6, idx);
  pro.set_input(7, updateAlpha);
  return good;
}

bool boxm2_ocl_update_color_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_update_color_process_globals;
  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  //get the inputs: device, scene, opencl_cache, cam, img
  unsigned int argIdx = 0;
  bocl_device_sptr          device = pro.get_input<bocl_device_sptr>(argIdx++);
  boxm2_scene_sptr          scene = pro.get_input<boxm2_scene_sptr>(argIdx++);
  boxm2_opencl_cache_sptr   opencl_cache = pro.get_input<boxm2_opencl_cache_sptr>(argIdx++);
  vpgl_camera_double_sptr   cam = pro.get_input<vpgl_camera_double_sptr>(argIdx++);
  vil_image_view_base_sptr  img = pro.get_input<vil_image_view_base_sptr>(argIdx++);
  std::string in_identifier = pro.get_input<std::string>(argIdx++);
  std::string mask_filename = pro.get_input<std::string>(argIdx++);
  bool       updateAlpha = pro.get_input<bool>(argIdx++);

  //make sure this image small enough (or else carve it into image pieces)
  constexpr std::size_t MAX_PIXELS = 16777216;
  if (img->ni()*img->nj() > MAX_PIXELS) {
    std::size_t sni = RoundUp(img->ni(), 16);
    std::size_t snj = RoundUp(img->nj(), 16);
    unsigned int numSegI = 1;
    unsigned int numSegJ = 1;
    while ( sni*snj*4 > MAX_PIXELS ) {
      sni /= 2;
      snj /= 2;
      ++numSegI;
      ++numSegJ;
    }
    sni = RoundUp(sni, 16);
    snj = RoundUp(snj, 16);
    vil_image_resource_sptr ir = vil_new_image_resource_of_view(*img);

    //run update for each image make sure to input i/j
    for (unsigned int i=0; i<=numSegI; ++i) {
      for (unsigned int j=0; j<=numSegJ; ++j) {
        //make sure the view doesn't extend past the original image
        std::size_t startI = (std::size_t) i * sni;
        std::size_t startJ = (std::size_t) j * snj;
        std::size_t endI = std::min(startI + sni, (std::size_t) img->ni());
        std::size_t endJ = std::min(startJ + snj, (std::size_t) img->nj());
        if (endI <= startI || endJ <= startJ)
          break;
        std::cout<<"Getting patch: ("<<startI<<','<<startJ<<") -> ("<<endI<<','<<endJ<<')'<<std::endl;
        vil_image_view_base_sptr view = ir->get_copy_view(startI, endI-startI, startJ, endJ-startJ);

        //run update
        boxm2_ocl_update_color::update_color(scene, device, opencl_cache,
                                             cam, view, in_identifier,
                                             mask_filename, updateAlpha,
                                             startI, startJ);
      }
    }
    return true;
  }
  else // otherwise just call ocl update
    return boxm2_ocl_update_color::update_color(scene,device,opencl_cache,
                                                cam,img,in_identifier,
                                                mask_filename,updateAlpha);
}
