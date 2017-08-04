// This is brl/bseg/bstm/pro/processes/bstm_describe_scene_process.cxx
//:
// \file
// \brief  A process for descibing a Multi-BSTM scene. Returns data path,
// appearance model, and the resolution of inidiviual voxels (for the first
// block in the scene -- if the scene has blocks with different resolutions this
// will not be reflected)
//
// \author Raphael Kargon
// \date 04 Aug 2017

#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_vector.h>

#include <bprb/bprb_func_process.h>
#include <bstm/bstm_data_traits.h>
#include <bstm/bstm_util.h>
#include <bstm_multi/bstm_multi_typedefs.h>
#include <bstm_multi/space_time_scene.h>

namespace bstm_multi_describe_scene_process_globals {
const unsigned n_inputs_ = 1;
const unsigned n_outputs_ = 3;
}

bool bstm_multi_describe_scene_process_cons(bprb_func_process &pro) {
  using namespace bstm_multi_describe_scene_process_globals;

  // process takes 1 input
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "bstm_multi_scene_sptr";

  // process has 2 outputs:
  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0] = "vcl_string"; // path to model data
  output_types_[1] = "vcl_string"; // appearance model type
  output_types_[2] = "double";     // voxel x size of first block
  output_types_[3] = "double";     // voxel y size of first block
  output_types_[4] = "double";     // voxel z size of first block
  output_types_[5] = "double";     // voxel t size of first block
  return pro.set_input_types(input_types_) &&
         pro.set_output_types(output_types_);
}

bool bstm_multi_describe_scene_process(bprb_func_process &pro) {
  using namespace bstm_multi_describe_scene_process_globals;

  if (pro.n_inputs() < n_inputs_) {
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_
             << vcl_endl;
    return false;
  }
  // get the inputs
  bstm_multi_scene_sptr scene = pro.get_input<bstm_multi_scene_sptr>(0);
  if (!scene) {
    vcl_cout << " null scene in bstm_multi_describe_scene_process\n";
    return false;
  }
  // vcl_cout << *scene;

  // verifies that a scene has a valid appearance, spits out data type and
  // appearance type size
  // NOTE: some data traits are commented out because they don't work (yet) on
  // BSTM
  vcl_vector<vcl_string> valid_types;
  valid_types.push_back(bstm_data_traits<BSTM_MOG3_GREY>::prefix());
  // valid_types.push_back(bstm_data_traits<BSTM_MOG3_GREY_16>::prefix());
  valid_types.push_back(bstm_data_traits<BSTM_GAUSS_RGB>::prefix());
  valid_types.push_back(bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::prefix());
  valid_types.push_back(bstm_data_traits<BSTM_MOG6_VIEW>::prefix());
  valid_types.push_back(bstm_data_traits<BSTM_GAUSS_RGB_VIEW>::prefix());
  // valid_types.push_back(bstm_data_traits<BSTM_GAUSS_UV_VIEW>::prefix());
  vcl_string data_type;
  int appTypeSize;
  bstm_util::verify_appearance(
      scene->appearances(), valid_types, data_type, appTypeSize);
  vcl_cout << "DATA_TYPE:" << data_type << vcl_endl;

  vgl_vector_3d<double> voxel_sizes;
  double voxel_time_range = 0;

  // obtain the resolution of first block we run into
  const vcl_map<bstm_block_id, bstm_multi_block_metadata> &blks =
      scene->blocks();
  if (blks.begin() != blks.end()) {
    const bstm_multi_block_metadata &blk = blks.begin()->second;

    vgl_vector_3d<double> voxel_sizes =
        blk.bbox().max_point() - blk.bbox().min_point();
    double voxel_time_range = blk.bbox_t().first - blk.bbox_t().second;

    for (vcl_vector<space_time_enum>::const_iterator iter =
             blk.subdivisions_.begin();
         iter != blk.subdivisions_.end();
         ++iter) {
      space_time_enum ste = *iter;
      switch (ste) {
      case STE_SPACE:
        voxel_sizes /= 8.0;
      case STE_TIME:;
        voxel_time_range /= 32.0;
      }
    }
  } else {
    // if scene has no blocks, no point in getting resolution
    vcl_cout << "Scene has no blocks, resolution will be returned as zero."
             << vcl_endl;
  }

  // set model dir as output
  vcl_string dataPath = scene->data_path();
  int i = 0;
  pro.set_output_val<vcl_string>(i++, dataPath);
  pro.set_output_val<vcl_string>(i++, data_type);
  pro.set_output_val<double>(i++, voxel_sizes.x());
  pro.set_output_val<double>(i++, voxel_sizes.y());
  pro.set_output_val<double>(i++, voxel_sizes.z());
  pro.set_output_val<double>(i++, voxel_time_range);
  return true;
}
