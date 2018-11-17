#include <iostream>
#include <queue>
#include "bvxm_to_boxm2.h"
//:
// \file
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//executable args
#include <vul/vul_arg.h>

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_box_3d.h>

//boct files
#include <boct/boct_tree.h>
#include <boct/boct_bit_tree.h>

//boxm2 files
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_traits.h>
#include <boxm2/io/boxm2_sio_mgr.h>
#include <boxm2/io/boxm2_lru_cache.h>

// bvxm files
#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_world_params_parser.h>


// this method creates a regular boxm2 world, i.e. each subblock/octree is one voxel of the bvxm_world, octrees are not refined/sub-divided.
void create_regular_world_scene_xml(boxm2_scene& new_scene, const bvxm_world_params_sptr& params, const bvxm_voxel_world_sptr&  /*world*/, int max_level)
{
  std::cout << "boxm2 scene directory: " << new_scene.data_path() << std::endl;
  std::cout << "setting 1 block only. number of subblocks, dimx: " << params->num_voxels().x() << " dimy: " << params->num_voxels().y() << " dimz: " << params->num_voxels().z() << std::endl;

  vgl_point_3d<double> local_orig((double)params->corner().x(), (double)params->corner().y(), (double)params->corner().z());

  //boxm2_lru_cache::create(&new_scene);
  std::map<boxm2_block_id, boxm2_block_metadata> new_blocks;

  // create only 1 block for a regular world
  boxm2_block_id block_id(0,0,0);

  // make each subblock one voxel:
  vgl_vector_3d<double> sub_block_dim(params->voxel_length(),params->voxel_length(),params->voxel_length());

  vgl_vector_3d<unsigned> sub_block_num(params->num_voxels().x(),params->num_voxels().y(),params->num_voxels().z());

  boxm2_block_metadata data(block_id, local_orig,sub_block_dim,sub_block_num,1,max_level,650.0,params->min_occupancy_prob());
  boxm2_block new_block(data);
  new_blocks[block_id]=data;
  new_scene.set_blocks(new_blocks);
}

// this method creates a regular boxm2 world, i.e. each subblock/octree is one voxel of the bvxm_world, octrees are not refined/sub-divided.
void initialize_regular_world_scene(boxm2_scene_sptr new_scene, const boxm2_cache_sptr& cache, const bvxm_world_params_sptr&  /*params*/, const bvxm_voxel_world_sptr& world)
{

  typedef bvxm_voxel_traits<OCCUPANCY>::voxel_datatype ocp_datatype;
  bvxm_voxel_grid_base_sptr ocp_grid_base = world->get_grid<OCCUPANCY>(0,0);
  auto *ocp_grid  = static_cast<bvxm_voxel_grid<ocp_datatype>*>(ocp_grid_base.ptr());

  // iterate over each subblock (tree) and create alpha values according to surface occupancy probabilities read from corresponding voxels in bvxm world
  typedef vnl_vector_fixed<unsigned char, 16> uchar16;

  std::map<boxm2_block_id, boxm2_block_metadata> blocks = new_scene->blocks();
  std::map<boxm2_block_id, boxm2_block_metadata>::iterator blk_iter;
  for (blk_iter = blocks.begin(); blk_iter != blocks.end(); ++blk_iter)
  {
    boxm2_block_id id = blk_iter->first;
    boxm2_block_metadata data = blk_iter->second;

    // to initialize alphas
    //float empty_prob_alpha = -std::log(1.0f-data.p_init_) / data.sub_block_dim_.x();
    float empty_prob_alpha = data.p_init_;

    //get data from cache
    boxm2_data_base * alpha = cache->get_data_base(new_scene, id,boxm2_data_traits<BOXM2_ALPHA>::prefix());
    std::size_t alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());

    //3d array of trees
    const boxm2_array_3d<uchar16> trees = cache->get_block(new_scene, id)->trees();

    auto * alpha_data = (boxm2_data_traits<BOXM2_ALPHA>::datatype*) alpha->data_buffer();
    // check for invalid parameters
    if( alphaTypeSize == 0 ) //This should never happen, it will result in division by zero later
    {
      std::cerr << "ERROR: Division by 0 in " << __FILE__ << __LINE__ << std::endl;
      throw 0;
    }

    std::cout << " alpha array size: " << alpha->buffer_length() /alphaTypeSize << std::endl; std::cout.flush();

    for ( unsigned i = 0 ; i < (alpha->buffer_length() /alphaTypeSize) ; ++i)
    {
      alpha_data[i] = empty_prob_alpha;
    }
    int Nx = trees.get_row1_count();
    int Ny = trees.get_row2_count();
    int Nz = trees.get_row3_count();
    //iterate through each tree
    bvxm_voxel_grid<ocp_datatype>::const_iterator ocp_slab_it = ocp_grid->begin();  // starts from z = 0 which is the top most slice in bvxm world
    for (unsigned int z = 0; z < Nz; ++z, ++ocp_slab_it) {

      bvxm_voxel_slab<ocp_datatype> const &slab = *ocp_slab_it;

      for (unsigned int x = 0; x < Nx; ++x) {
        for (unsigned int y = 0; y < Ny; ++y) {

         // retrieve the corresponding prob value from bvxm world
         //float occ_prob = 1.0;
         float occ_prob = slab(x,y);

         //int offset = (Nz-1-z) + Nz*(y + x*Ny);  // for boxm2 worlds, z = 0 is the bottom slice
         //alpha_data[offset] = occ_prob;

         //load current block/tree
         uchar16 tree = trees(x, y, Nz-1-z);  // for boxm2 z = 0 is the bottom slice, so convert from bvxm z to boxm2 z
         boct_bit_tree bit_tree((unsigned char*) tree.data_block(), data.max_level_);

         //iterate through leaves of the tree
         std::vector<int> leafBits = bit_tree.get_leaf_bits(0,data.max_level_);  // OZGE: what should depth be here?
         std::vector<int>::iterator iter;
         for (iter = leafBits.begin(); iter != leafBits.end(); ++iter) {
           int currBitIndex = (*iter);
           int currIdx = bit_tree.get_data_index(currBitIndex); //data index

           int curr_depth = bit_tree.depth_at(currBitIndex);
           double side_len = 1.0 / (double) (1<<curr_depth);

           alpha_data[currIdx] = -std::log(1.0f-occ_prob) / side_len;
         }
        }
      }
    }
  }
}

int main(int argc, char** argv)
{
  std::cout << "Converting bvxm scene to boxm2 Scene" << std::endl;
  vul_arg<std::string> scene_path("-bvxm_scene", "bvxm scene filename", "");
  vul_arg<std::string> out_dir("-out", "output world directory for boxm2 scene", "");
  vul_arg<bool> only_create_xml("-only_scene_xml", "if present creates boxm2 scene xml file by converting bvxm scene file and exists", false);
  vul_arg_parse(argc, argv);

  // parse bvxm scene
  bvxm_world_param_parser parser(scene_path());
  bvxm_world_params_sptr params = parser.create_bvxm_world_params();

  bvxm_voxel_world_sptr world = new bvxm_voxel_world;
  world->set_params(params);

  // create a boxm2 scene
  boxm2_scene new_scene;
  new_scene.set_version(2);
  new_scene.set_data_path(out_dir());
  vgl_point_3d<double> local_orig((double)params->corner().x(), (double)params->corner().y(), (double)params->corner().z());
  new_scene.set_local_origin(local_orig);
  new_scene.set_lvcs(*(params->lvcs()));
  new_scene.set_xml_path(out_dir()+"/scene.xml");
  std::vector<std::string> apps;
  apps.emplace_back("boxm2_mog3_grey");
  apps.emplace_back("boxm2_num_obs");
  new_scene.set_appearances(apps);

  int max_level = 1;
  //create_regular_world(new_scene, params, world);
  create_regular_world_scene_xml(new_scene, params, world, max_level);
  new_scene.save_scene();

  if (!only_create_xml()) {

    // now load the scene and create a cpp cache
    boxm2_scene_sptr scene = new boxm2_scene(out_dir()+"/scene.xml");
    boxm2_lru_cache::create(scene);

    boxm2_cache_sptr cache = boxm2_cache::instance();

    initialize_regular_world_scene(scene, cache, params, world);
    cache->write_to_disk();
  }
}
