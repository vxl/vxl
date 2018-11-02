// This is brl/bseg/boxm2/cpp/pro/processes/boxm2_cpp_render_z_images_process.cxx
#include <iostream>
#include <fstream>
#include <sstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for querying  cell data at a given 3-d location
//
// \author Vishal Jain
// \date Mar 10, 2011

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/io/boxm2_cache.h>
#include <boxm2/io/boxm2_stream_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/boxm2_util.h>
//brdb stuff
#include <brdb/brdb_value.h>
#include <bsta/bsta_joint_histogram.h>
//directory utility
#include <vcl_where_root_dir.h>
#include <boct/boct_bit_tree.h>
#include <brad/brad_phongs_model_est.h>
#include <vil/vil_save.h>

//#define SEGLEN_FACTOR 93206.7555f   //Hack representation of int32.maxvalue/(ni*nj*block_length)

namespace boxm2_cpp_render_z_images_process_globals
{
  constexpr unsigned n_inputs_ = 4;
  constexpr unsigned n_outputs_ = 0;
}

bool boxm2_cpp_render_z_images_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_cpp_render_z_images_process_globals;

  //process takes 4 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "boxm2_cache_sptr";
  input_types_[2] = "vcl_string"; // data identifier
  input_types_[3] = "vcl_string"; //outputdir
  //and no outputs
  std::vector<std::string>  output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool  boxm2_cpp_render_z_images_process(bprb_func_process& pro)
{
  using namespace boxm2_cpp_render_z_images_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  boxm2_scene_sptr scene =pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_cache_sptr cache =pro.get_input<boxm2_cache_sptr>(i++);
  std::string data_identifier= pro.get_input<std::string>(i++);
  std::string outdir = pro.get_input<std::string>(i++);

  vgl_box_3d<double> bbox=scene->bounding_box();
  std::vector<boxm2_block_id> vis_order = scene->get_block_ids();// (vpgl_perspective_camera<double>*) cam.ptr());
  std::vector<boxm2_block_id>::iterator id;
  double xint=0.0;
  double yint=0.0;
  double zint=0.0;
  for (id = vis_order.begin(); id != vis_order.end(); ++id)
  {
    boxm2_block_metadata mdata=scene->get_block_metadata(*id);
    unsigned long num_octree_cells = 1L << (mdata.max_level_-1);
    xint += mdata.sub_block_dim_.x()/num_octree_cells;
    yint += mdata.sub_block_dim_.y()/num_octree_cells;
    zint += mdata.sub_block_dim_.z()/num_octree_cells;
  }
  auto ni = (unsigned int)(bbox.width() /xint -1);
  auto nj = (unsigned int)(bbox.height()/yint -1);
  auto nz = (unsigned int)(bbox.depth() /zint -1);

  for (unsigned k = 0;k<nz;++k)
  {
    vil_image_view<float> img(ni,nj);
    double z = bbox.min_z()+zint*(k+0.5);
    for (unsigned i = 0; i<ni; ++i)
    {
      double x = bbox.min_x()+xint*(i+0.5);
      for (unsigned j = 0; j<nj; ++j)
      {
        double y = bbox.min_y()+yint*(j+0.5);
        vgl_point_3d<double> local;
        boxm2_block_id id;
        if (!scene->contains(vgl_point_3d<double>(x, y, z), id, local))
        {
          std::cout<<"does not contain"<<std::endl;
          continue;
        }
        int index_x=(int)std::floor(local.x());
        int index_y=(int)std::floor(local.y());
        int index_z=(int)std::floor(local.z());

        boxm2_block * blk=cache->get_block(scene,id);
        boxm2_block_metadata mdata = scene->get_block_metadata_const(id);

        vnl_vector_fixed<unsigned char,16> treebits=blk->trees()(index_x,index_y,index_z);
        boct_bit_tree tree(treebits.data_block(),mdata.max_level_);
        int bit_index=tree.traverse(local);
        int index=tree.get_data_index(bit_index,false);

        boxm2_data_base     *  float_base = cache->get_data_base(scene,id,data_identifier);
        auto * buffer = reinterpret_cast<float*>(float_base->data_buffer());
        img(i,j) = buffer[index];
      }
    }
    std::ostringstream ss;
    ss<<outdir<<'/'<<k<<".tiff";
    vil_save(img,ss.str().c_str());
  }

 return true;
}
