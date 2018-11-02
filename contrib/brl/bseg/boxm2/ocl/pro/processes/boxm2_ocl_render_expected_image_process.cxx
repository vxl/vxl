// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_render_expected_image_process.cxx
#include <fstream>
#include <iostream>
#include <algorithm>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for rendering the scene.
//
// \author Vishal Jain
// \date Mar 10, 2011

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <vil/vil_image_view.h>
#include <vil/vil_crop.h>
//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <boxm2/ocl/algo/boxm2_ocl_render_expected_image.h>
#include <vul/vul_timer.h>


namespace boxm2_ocl_render_expected_image_process_globals
{
  constexpr unsigned n_inputs_ = 9;
  constexpr unsigned n_outputs_ = 2;
}

bool boxm2_ocl_render_expected_image_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_render_expected_image_process_globals;

  //process takes 1 input
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "boxm2_opencl_cache_sptr";
  input_types_[3] = "vpgl_camera_double_sptr";
  input_types_[4] = "unsigned";
  input_types_[5] = "unsigned";
  input_types_[6] = "vcl_string";
  input_types_[7] = "float"; // near factor ( maximum # of pixels should map to the finest voxel )
  input_types_[8] = "float"; // far factor ( minimum # of pixels should map to the finest voxel )

  // process has 2 outputs:
  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";
  output_types_[1] = "vil_image_view_base_sptr";

  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
  // in case the 7th input is not set
  brdb_value_sptr idx = new brdb_value_t<std::string>("");
  brdb_value_sptr tnearfactor = new brdb_value_t<float>(1e6f);
  brdb_value_sptr tfarfactor = new brdb_value_t<float>(1e6f);

  pro.set_input(6, idx);
  pro.set_input(7, tnearfactor);
  pro.set_input(8, tfarfactor);
  return good;
}

bool boxm2_ocl_render_expected_image_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_render_expected_image_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  bocl_device_sptr device= pro.get_input<bocl_device_sptr>(i++);
  boxm2_scene_sptr scene =pro.get_input<boxm2_scene_sptr>(i++);

  boxm2_opencl_cache_sptr opencl_cache= pro.get_input<boxm2_opencl_cache_sptr>(i++);
  vpgl_camera_double_sptr cam= pro.get_input<vpgl_camera_double_sptr>(i++);
  auto ni=pro.get_input<unsigned>(i++);
  auto nj=pro.get_input<unsigned>(i++);
  std::string ident = pro.get_input<std::string>(i++);
  auto   nearfactor = pro.get_input<float>(i++);
  auto   farfactor = pro.get_input<float>(i++);


  vil_image_view<float> exp_img(ni, nj, 1);
  vil_image_view<float> vis_img(ni, nj, 1);

  vul_timer t;
  t.mark();
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
        vil_image_view<float> exp_view = vil_crop(exp_img, startI, chunkNI, startJ, chunkNJ);
        vil_image_view<float> vis_view = vil_crop(vis_img, startI, chunkNI, startJ, chunkNJ);

        //run update
        ret = boxm2_ocl_render_expected_image::render(exp_view, vis_view, scene, device, opencl_cache, cam, ident,
          chunkNI, chunkNJ, nearfactor, farfactor, startI, startJ);
      }
    }
  }
  else { //otherwise just run a normal update with one image
    ret = boxm2_ocl_render_expected_image::render(exp_img, vis_img, scene, device, opencl_cache, cam, ident,
      ni, nj, nearfactor, farfactor);
  }
  std::cout<<"Total time taken is "<<t.all()<<std::endl;

  if(!ret) return false;

  i=0;
  // store scene smart pointer
  pro.set_output_val<vil_image_view_base_sptr>(i++, new vil_image_view<float>(exp_img));
  pro.set_output_val<vil_image_view_base_sptr>(i++, new vil_image_view<float>(vis_img));
  return true;
}
