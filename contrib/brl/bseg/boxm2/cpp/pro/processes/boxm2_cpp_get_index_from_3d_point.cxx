// This is brl/bseg/boxm2/cpp/pro/processes/boxm2_cpp_get_index_from_3d_point_process.cxx
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for converting a 3d point into an octree blockid/index
//
// \author Andy Neff
// \date Oct 15 2015

#include <boxm2/io/boxm2_cache.h>
#include <boxm2/io/boxm2_stream_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/boxm2_util.h>
//brdb stuff
#include <boct/boct_bit_tree.h>


namespace boxm2_cpp_get_index_from_3d_point_process_globals
{
  constexpr unsigned n_inputs_ = 5;
  constexpr unsigned n_outputs_ = 4;
}

namespace boxm2_cpp_get_3d_point_from_index_process_globals
{
  constexpr unsigned n_inputs_ = 6;
  constexpr unsigned n_outputs_ = 7;
}

bool boxm2_cpp_get_index_from_3d_point_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_cpp_get_index_from_3d_point_process_globals;

  bool ok=false;

  //process takes 7 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "boxm2_cache_sptr";
  input_types_[2] = "float"; //x
  input_types_[3] = "float"; //y
  input_types_[4] = "float"; //z
  ok = pro.set_input_types(input_types_);
  if (!ok) return ok;

  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0]="int";
  output_types_[1]="int";
  output_types_[2]="int";
  output_types_[3]="int";
  ok = pro.set_output_types(output_types_);
  if (!ok) return ok;

  return true;
}

bool boxm2_cpp_get_3d_point_from_index_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_cpp_get_3d_point_from_index_process_globals;

  bool ok=false;

  //process takes 7 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "boxm2_cache_sptr";
  input_types_[2] = "int"; //i
  input_types_[3] = "int"; //j
  input_types_[4] = "int"; //k
  input_types_[5] = "int"; //index
  ok = pro.set_input_types(input_types_);
  if (!ok) return ok;

  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0]="float";
  output_types_[1]="float";
  output_types_[2]="float";
  output_types_[3]="float";
  output_types_[4]="float";
  output_types_[5]="float";
  output_types_[6]="int";
  ok = pro.set_output_types(output_types_);
  if (!ok) return ok;

  return true;
}

bool  boxm2_cpp_get_index_from_3d_point_process(bprb_func_process& pro)
{
  using namespace boxm2_cpp_get_index_from_3d_point_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  boxm2_scene_sptr scene =pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_cache_sptr cache= pro.get_input<boxm2_cache_sptr>(i++);
  auto x =pro.get_input<float>(i++);
  auto y =pro.get_input<float>(i++);
  auto z =pro.get_input<float>(i++);

  vgl_point_3d<double> local;
  boxm2_block_id id;
  if (!scene->contains(vgl_point_3d<double>(x, y, z), id, local))
    return false;

  int index_x=(int)std::floor(local.x());
  int index_y=(int)std::floor(local.y());
  int index_z=(int)std::floor(local.z());
  boxm2_block * blk=cache->get_block(scene, id);
  boxm2_block_metadata mdata = scene->get_block_metadata_const(id);
  vnl_vector_fixed<unsigned char,16> treebits=blk->trees()(index_x,index_y,index_z);
  boct_bit_tree tree(treebits.data_block(),mdata.max_level_);
  int bit_index=tree.traverse(local);
  int index=tree.get_data_index(bit_index,false);

  pro.set_output_val<int>(0, id.i());
  pro.set_output_val<int>(1, id.j());
  pro.set_output_val<int>(2, id.k());
  pro.set_output_val<int>(3, index);

  return true;
}

bool  boxm2_cpp_get_3d_point_from_index_process(bprb_func_process& pro)
{
  using namespace boxm2_cpp_get_3d_point_from_index_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  boxm2_scene_sptr scene =pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_cache_sptr cache= pro.get_input<boxm2_cache_sptr>(i++);
  int bi =pro.get_input<int>(i++);
  int bj =pro.get_input<int>(i++);
  int bk =pro.get_input<int>(i++);
  int index =pro.get_input<int>(i++);

  vgl_point_3d<double> local;
  boxm2_block_id id(bi, bj, bk);
  boxm2_block * blk = cache->get_block(scene,id);
  boxm2_block_metadata mdata = scene->get_block_metadata_const(id);
  vgl_box_3d<double> bbox = mdata.bbox();
  vgl_vector_3d<unsigned int> num_subblocks = mdata.sub_block_num_;
  vgl_point_3d<double> block_origin = mdata.local_origin_;

  // int ix_min = std::max(0,int(std::floor((bbox.min_x() - block_origin.x())/mdata.sub_block_dim_.x())));
  // int iy_min = std::max(0,int(std::floor((bbox.min_y() - block_origin.y())/mdata.sub_block_dim_.y())));
  // int iz_min = std::max(0,int(std::floor((bbox.min_z() - block_origin.z())/mdata.sub_block_dim_.z())));
  // int ix_max = std::min(int(num_subblocks.x()-1),int(std::floor((bbox.max_x() - block_origin.x())/mdata.sub_block_dim_.x())));
  // int iy_max = std::min(int(num_subblocks.y()-1),int(std::floor((bbox.max_y() - block_origin.y())/mdata.sub_block_dim_.y())));
  // int iz_max = std::min(int(num_subblocks.z()-1),int(std::floor((bbox.max_z() - block_origin.z())/mdata.sub_block_dim_.z())));

  const boxm2_array_3d<vnl_vector_fixed<unsigned char, 16> > &trees = blk->trees();

  vgl_point_3d<double> subblock_origin;

  for (int ix=0; ix<mdata.sub_block_num_.x(); ++ix)
  {
    for (int iy=0; iy<mdata.sub_block_num_.y(); ++iy)
    {
      for (int iz=0; iz<mdata.sub_block_num_.z(); ++iz)
      {
        vnl_vector_fixed<unsigned char, 16>  tree = trees(ix, iy, iz);

        boct_bit_tree bit_tree((unsigned char*) tree.data_block(), mdata.max_level_);

//if (bit_tree.num_cells()>1)
//  std::cout << bit_tree.num_cells() << " cells" << std::endl;

        if (bit_tree.num_cells() <= index)
        {
          index -= bit_tree.num_cells();
          //std::cout << bit_tree.num_cells() << " cells" << std::endl;
        }
        else
        {
          //std::cout << "subblock index: " << index << std::endl;

          std::vector<int> cell_indexes = bit_tree.get_cell_bits();

          int cell_index = cell_indexes[index];

          //std::cout << "bit index: " << cell_index << std::endl;

          //iterate through leaves of the tree
          //std::vector<int> leafBits = bit_tree.get_leaf_bits();
          subblock_origin = block_origin + vgl_vector_3d<double>(mdata.sub_block_dim_.x()*ix,
                                                                 mdata.sub_block_dim_.y()*iy,
                                                                 mdata.sub_block_dim_.z()*iz);

          vgl_point_3d<double> cell_center = bit_tree.cell_center(cell_index);
          cell_center.x() = cell_center.x() * mdata.sub_block_dim_.x() + subblock_origin.x();
          cell_center.y() = cell_center.y() * mdata.sub_block_dim_.y() + subblock_origin.y();
          cell_center.z() = cell_center.z() * mdata.sub_block_dim_.z() + subblock_origin.z();

          pro.set_output_val<float>(0, cell_center.x());
          pro.set_output_val<float>(1, cell_center.y());
          pro.set_output_val<float>(2, cell_center.z());
          pro.set_output_val<float>(3, bit_tree.cell_len(cell_index) * mdata.sub_block_dim_.x());
          pro.set_output_val<float>(4, bit_tree.cell_len(cell_index) * mdata.sub_block_dim_.y());
          pro.set_output_val<float>(5, bit_tree.cell_len(cell_index) * mdata.sub_block_dim_.z());
          pro.set_output_val<int>(6, bit_tree.is_leaf(cell_index));

          return true;
        }
      }
    }
  }

  return false;
}
