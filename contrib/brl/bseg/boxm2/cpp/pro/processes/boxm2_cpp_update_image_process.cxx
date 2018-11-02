// This is brl/bseg/boxm2/cpp/pro/processes/boxm2_cpp_update_image_process.cxx
#include <iostream>
#include <fstream>
#include <sstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for updating the scene.
//
// \author Vishal Jain
// \date Mar 10, 2011

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/io/boxm2_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <vil/vil_image_view.h>
//brdb stuff
#include <brdb/brdb_value.h>
#include <boxm2/boxm2_util.h>
#include <boxm2/cpp/algo/boxm2_update_functions.h>

//directory utility
#include <vcl_where_root_dir.h>

namespace boxm2_cpp_update_image_process_globals
{
  constexpr unsigned n_inputs_ = 5;
  constexpr unsigned n_outputs_ = 0;
}

bool boxm2_cpp_update_image_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_cpp_update_image_process_globals;

  //process takes 4 inputs
  // 0) scene
  // 1) cache
  // 2) camera
  // 3) image
  // 4) illumination_bin_index
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "boxm2_cache_sptr";
  input_types_[2] = "vpgl_camera_double_sptr";
  input_types_[3] = "vil_image_view_base_sptr";
  input_types_[4] = "vcl_string";// if identifier is empty, then only one appearance model
  // process has 1 output:
  // output[0]: scene sptr
  std::vector<std::string>  output_types_(n_outputs_);
  bool good = pro.set_input_types(input_types_) &&
    pro.set_output_types(output_types_);
  // in case the 5th input is not set
  brdb_value_sptr idx = new brdb_value_t<std::string>("");
  pro.set_input(4, idx);
  return good;
}

bool boxm2_cpp_update_image_process(bprb_func_process& pro)
{
    using namespace boxm2_cpp_update_image_process_globals;

    if ( pro.n_inputs() < n_inputs_ ) {
        std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
        return false;
    }
    //get the inputs
    unsigned i = 0;
    boxm2_scene_sptr scene =pro.get_input<boxm2_scene_sptr>(i++);
    boxm2_cache_sptr cache= pro.get_input<boxm2_cache_sptr>(i++);
    vpgl_camera_double_sptr cam= pro.get_input<vpgl_camera_double_sptr>(i++);
    vil_image_view_base_sptr in_img=pro.get_input<vil_image_view_base_sptr>(i++);
    std::string identifier = pro.get_input<std::string>(i);

    vil_image_view_base_sptr float_image=boxm2_util::prepare_input_image(in_img);
    if (auto * input_image=dynamic_cast<vil_image_view<float> * > (float_image.ptr()))
    {
        bool foundDataType = false;
        bool foundNumObsType = false;

        std::string data_type;
        std::string num_obs_type;
        std::vector<std::string> apps = scene->appearances();
        int appTypeSize = 0; // just to avoid compiler warning about using potentially uninitialised value
        for (const auto & app : apps) {
            if ( app == boxm2_data_traits<BOXM2_MOG3_GREY>::prefix() )
            {
                data_type = app;
                foundDataType = true;
                appTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_MOG3_GREY>::prefix());
            }
            else if ( app == boxm2_data_traits<BOXM2_MOG3_GREY_16>::prefix() )
            {
                data_type = app;
                foundDataType = true;
                appTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_MOG3_GREY_16>::prefix());
            }
            else if ( app == boxm2_data_traits<BOXM2_NUM_OBS>::prefix() )
            {
                num_obs_type = app;
                foundNumObsType = true;
            }
            else if ( app == boxm2_data_traits<BOXM2_GAUSS_GREY>::prefix() )
            {
                data_type = app;
                foundDataType = true;
                appTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_GAUSS_GREY>::prefix());
            }
        }
        if (!foundDataType) {
            std::cout<<"BOXM2_OCL_UPDATE_IMAGE_PROCESS ERROR: scene doesn't have BOXM2_MOG3_GREY or BOXM2_MOG3_GREY_16 data type"<<std::endl;
            return false;
        }
        if (identifier.size() > 0) {
          data_type += "_" + identifier;
          if (foundNumObsType)
            num_obs_type += "_" + identifier;
        }

        std::cout<<"Update"<<std::endl;
        return boxm2_update_image(scene,
                                  data_type,appTypeSize,
                                  num_obs_type,
                                  cam,
                                  input_image,
                                  input_image->ni(),
                                  input_image->nj());
    }

    return false;
}
