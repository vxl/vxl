// This is brl/bseg/boxm2/cpp/pro/processes/boxm2_cpp_update_using_quality_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for updating the scene using a quality map.
//
// \author Ozge C. Ozcanli
// \date Aug 04, 2011

#include <vcl_fstream.h>
#include <vcl_sstream.h>
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

namespace boxm2_cpp_update_using_quality_process_globals
{
  const unsigned n_inputs_ = 6;
  const unsigned n_outputs_ = 0;
}

bool boxm2_cpp_update_using_quality_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_cpp_update_using_quality_process_globals;

  //process takes 6 inputs
  // 0) scene
  // 1) cache
  // 2) camera
  // 3) image
  // 4) quality map, // float image with values in [0,1]
  // 5) illumination_bin_index
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "boxm2_cache_sptr";
  input_types_[2] = "vpgl_camera_double_sptr";
  input_types_[3] = "vil_image_view_base_sptr";
  input_types_[4] = "vil_image_view_base_sptr";
  input_types_[5] = "vcl_string";// if identifier is empty, then only one appearance model

  // process has 1 output:
  // output[0]: scene sptr
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  // in case the 6th input is not set:
  brdb_value_sptr idx = new brdb_value_t<vcl_string>("");
  return pro.set_input_types(input_types_) &&
         pro.set_output_types(output_types_) &&
         pro.set_input(5, idx);
}

bool boxm2_cpp_update_using_quality_process(bprb_func_process& pro)
{
    using namespace boxm2_cpp_update_using_quality_process_globals;

    if ( pro.n_inputs() < n_inputs_ ) {
        vcl_cout << pro.name() << ": The number of inputs should be " << n_inputs_<< vcl_endl;
        return false;
    }
    //get the inputs
    unsigned i = 0;
    boxm2_scene_sptr scene =pro.get_input<boxm2_scene_sptr>(i++);
    boxm2_cache_sptr cache= pro.get_input<boxm2_cache_sptr>(i++);
    vpgl_camera_double_sptr cam= pro.get_input<vpgl_camera_double_sptr>(i++);
    vil_image_view_base_sptr in_img=pro.get_input<vil_image_view_base_sptr>(i++);
    vil_image_view_base_sptr quality_img=pro.get_input<vil_image_view_base_sptr>(i++);
    vcl_string identifier = pro.get_input<vcl_string>(i);

    vil_image_view_base_sptr float_image=boxm2_util::prepare_input_image(in_img);
    vil_image_view<float> quality_map(quality_img);
    if (vil_image_view<float> * input_image=dynamic_cast<vil_image_view<float> * > (float_image.ptr()))
    {
        bool foundDataType = false;
        bool foundNumObsType = false;

        vcl_string data_type;
        vcl_string num_obs_type;
        vcl_vector<vcl_string> apps = scene->appearances();
        int appTypeSize = 0; // just to avoid compiler warning about using potentially uninitialised value
        for (unsigned int i=0; i<apps.size(); ++i) {
            if ( apps[i] == boxm2_data_traits<BOXM2_MOG3_GREY>::prefix() )
            {
                data_type = apps[i];
                foundDataType = true;
                appTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_MOG3_GREY>::prefix());
            }
            else if ( apps[i] == boxm2_data_traits<BOXM2_MOG3_GREY_16>::prefix() )
            {
                data_type = apps[i];
                foundDataType = true;
                appTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_MOG3_GREY_16>::prefix());
            }
            else if ( apps[i] == boxm2_data_traits<BOXM2_NUM_OBS>::prefix() )
            {
                num_obs_type = apps[i];
                foundNumObsType = true;
            }
            else if ( apps[i] == boxm2_data_traits<BOXM2_GAUSS_GREY>::prefix() )
            {
                data_type = apps[i];
                foundDataType = true;
                appTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_GAUSS_GREY>::prefix());
            }
        }
        if (!foundDataType) {
            vcl_cout<<"BOXM2_OCL_RENDER_PROCESS ERROR: scene doesn't have BOXM2_MOG3_GREY or BOXM2_MOG3_GREY_16 data type"<<vcl_endl;
            return false;
        }
        if (identifier.size() > 0) {
          data_type += "_" + identifier;
          if (foundNumObsType)
            num_obs_type += "_" + identifier;
        }

        vcl_cout<<"Update"<<vcl_endl;
        return boxm2_update_using_quality(scene,
                                          data_type,appTypeSize,
                                          num_obs_type,
                                          cam,
                                          input_image,
                                          quality_map,
                                          input_image->ni(),
                                          input_image->nj());
    }
    else
        return false;
}
