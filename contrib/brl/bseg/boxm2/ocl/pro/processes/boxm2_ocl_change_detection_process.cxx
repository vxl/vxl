// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_change_detection_process.cxx
//:
// \file
// \brief  A process for change detection
//
// \author Vishal Jain
// \date Mar 10, 2011

#include <fstream>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <bprb/bprb_func_process.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/ocl/algo/boxm2_ocl_change_detection.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boxm2/boxm2_util.h>
#include <vil/vil_image_view.h>
#include <vil/vil_new.h>
#include <vil/vil_crop.h>

// brdb stuff
#include <brdb/brdb_value.h>

// directory utility
#include <vul/vul_timer.h>
#include <vcl_where_root_dir.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

namespace boxm2_ocl_change_detection_process_globals
{
  constexpr unsigned n_inputs_ = 10;
  constexpr unsigned n_outputs_ = 2;
}

bool boxm2_ocl_change_detection_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_change_detection_process_globals;

  // process takes 9 inputs and two outputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "boxm2_opencl_cache_sptr";
  input_types_[3] = "vpgl_camera_double_sptr";
  input_types_[4] = "vil_image_view_base_sptr";
  input_types_[5] = "vil_image_view_base_sptr";
  input_types_[6] = "int";   // n for nxn ray casting
  input_types_[7] = "vcl_string"; // "raybelief" string for using raybelief
  input_types_[8] = "bool";       // true to use max mode probability
  input_types_[9] = "vcl_string";       // idenitifer

  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";  // prob of change image
  output_types_[1] = "vil_image_view_base_sptr";  // Red Green change image
  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);

  // default is 1x1, with no ray belief
  brdb_value_sptr nxn = new brdb_value_t<int>(1);
  brdb_value_sptr rayb = new brdb_value_t<std::string>(""); // use ray belief?
  brdb_value_sptr pmax = new brdb_value_t<bool>(false);    // use max-mode probability instead of mixture?
  brdb_value_sptr ident = new brdb_value_t<std::string>(""); // identifier
  pro.set_input(6, nxn);
  pro.set_input(7, rayb);
  pro.set_input(8, pmax);
  pro.set_input(9, ident);
  return good;
}

bool boxm2_ocl_change_detection_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_change_detection_process_globals;
  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  // get the inputs
  unsigned i = 0;
  bocl_device_sptr          device = pro.get_input<bocl_device_sptr>(i++);
  boxm2_scene_sptr          scene = pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_opencl_cache_sptr   opencl_cache = pro.get_input<boxm2_opencl_cache_sptr>(i++);
  vpgl_camera_double_sptr   cam = pro.get_input<vpgl_camera_double_sptr>(i++);
  vil_image_view_base_sptr  img = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr  exp_img = pro.get_input<vil_image_view_base_sptr>(i++);
  int                       n = pro.get_input<unsigned>(i++);                 // nxn
  std::string                norm_type = pro.get_input<std::string>(i++);
  bool                      pmax = pro.get_input<bool>(i++);
  std::string                identifier = pro.get_input<std::string>(i++);

  // img dims
  unsigned ni=img->ni();
  unsigned nj=img->nj();

  // allocate two output images
  auto*    change_img = new vil_image_view<float>(ni, nj);
  auto* rgb_change_img = new vil_image_view<vxl_byte>(ni,nj,4);

  // check to see which type of change detection to do, either two pass, or regular
  vul_timer t;
  if ( norm_type == "twopass" ) {
    boxm2_ocl_two_pass_change::change_detect( *change_img,
                                              device,
                                              scene,
                                              opencl_cache,
                                              cam,
                                              img,
                                              exp_img,
                                              n,
                                              norm_type,
                                              pmax );
  }
  else {
    bool ret = true;
    //TODO Factor this out to a utility function
    //make sure this image small enough (or else carve it into image pieces)
    constexpr std::size_t MAX_PIXELS = 16777216;
    if (ni*nj > MAX_PIXELS) {
      std::size_t sni = RoundUp(ni, 16);
      std::size_t snj = RoundUp(nj, 16);
      unsigned int numSegI = 1;
      unsigned int numSegJ = 1;
      while ( sni*snj*2 > MAX_PIXELS ) {
        sni /= 2;
        snj /= 2;
        ++numSegI;
        ++numSegJ;
      }
      sni = RoundUp(sni, 16);
      snj = RoundUp(snj, 16);
      vil_image_resource_sptr ir = vil_new_image_resource_of_view(*img);
      vil_image_resource_sptr er = vil_new_image_resource_of_view(*exp_img);

      //run update for each image make sure to input i/j
      for (unsigned int i=0; i<=numSegI; ++i) {
        for (unsigned int j=0; j<=numSegJ; ++j) {
          if(!ret) {
            std::cout << pro.name() << " failed" << std::endl;
            return false;
          }

          //make sure the view doesn't extend past the original image
          std::size_t startI = (std::size_t) i * sni;
          std::size_t startJ = (std::size_t) j * snj;
          std::size_t endI = std::min(startI + sni, (std::size_t) ni);
          std::size_t endJ = std::min(startJ + snj, (std::size_t) nj);
          if (endI <= startI || endJ <= startJ)
            break;
          std::cout<<"Getting patch: ("<<startI<<','<<startJ<<") -> ("<<endI<<','<<endJ<<')'<<std::endl;

          unsigned int chunkNI = endI-startI;
          unsigned int chunkNJ = endJ-startJ;
          vil_image_view_base_sptr img_view = ir->get_copy_view(startI, chunkNI, startJ, chunkNJ);
          vil_image_view_base_sptr exp_view = er->get_copy_view(startI, chunkNI, startJ, chunkNJ);
          vil_image_view<float> change_view = vil_crop(*change_img, startI, chunkNI, startJ, chunkNJ);
          vil_image_view<vxl_byte> rgb_change_view = vil_crop(*rgb_change_img, startI, chunkNI, startJ, chunkNJ);

          ret = boxm2_ocl_change_detection::change_detect( change_view, rgb_change_view,
              device, scene, opencl_cache, cam, img_view, exp_view,
              n, norm_type, pmax, identifier, startI, startJ );
        }
      }
    }
    else { //otherwise just run a normal update with one image
      ret = boxm2_ocl_change_detection::change_detect( *change_img, *rgb_change_img,
          device, scene, opencl_cache, cam, img, exp_img,
          n, norm_type, pmax, identifier );
    }

    if(!ret) return false;
  }
  std::cout<<"Total change time: "<<t.all()<<" ms"<<std::endl;

  // set outputs
  i=0;
  pro.set_output_val<vil_image_view_base_sptr>(i++, change_img);
  pro.set_output_val<vil_image_view_base_sptr>(i++, rgb_change_img);
  return true;
}
