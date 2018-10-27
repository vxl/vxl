// This is brl/bseg/boxm2/cpp/pro/processes/boxm2_cpp_query_cell_process.cxx
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for querying cell data at a given 3-d location
//
// \author Andy Neff
// \date Oct 15 2015

#include <boxm2/io/boxm2_cache.h>
#include <boxm2/io/boxm2_stream_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/boxm2_util.h>
//directory utility
#include <vcl_where_root_dir.h>
#include <boct/boct_bit_tree.h>
#include <brad/brad_phongs_model_est.h>

#include <bpro/core/bbas_pro/bbas_1d_array_float.h>

//#define SEGLEN_FACTOR 93206.7555f   //Hack representation of int32.maxvalue/(ni*nj*block_length)

namespace boxm2_cpp_query_cell_process_globals
{
  constexpr unsigned n_inputs_ = 8;
  constexpr unsigned n_outputs_ = 1;
}

bool boxm2_cpp_query_cell_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_cpp_query_cell_process_globals;

  bool ok=false;

  //process takes 7 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "boxm2_cache_sptr";
  input_types_[2] = "int"; //blk_i
  input_types_[3] = "int"; //blk_j
  input_types_[4] = "int"; //blk_k
  input_types_[5] = "int"; //index
  input_types_[6] = "vcl_string"; //identifier string
  input_types_[7] = "vcl_string"; //type string
  ok = pro.set_input_types(input_types_);
  if (!ok) return ok;

  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0]="bbas_1d_array_float_sptr";
  ok = pro.set_output_types(output_types_);
  if (!ok) return ok;

  return true;

}

// template <boxm2_data_type data_type>
// bool get(boxm2_data_base* data_base, int index, int num_entries, vnl_vector <float>* return_data)
// {

//   boxm2_data<data_type>* data = new boxm2_data<data_type>(data_base->data_buffer(),
//                                                           data_base->buffer_length(),
//                                                           data_base->block_id());
//   boxm2_array_1d<boxm2_data_traits<data_type>::datatype> data_array=data->data();
//   boxm2_data_traits<data_type>::datatype model=data_array[index];
//   for(int x=0; x<num_entries; x++) //can I use boxm2_data_traits<data_type>.argument_type.SIZE()?
//     return_data.push_back(model[x]);

//   return true;
// }

bool  boxm2_cpp_query_cell_process(bprb_func_process& pro)
{
  using namespace boxm2_cpp_query_cell_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  boxm2_scene_sptr scene =pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_cache_sptr cache= pro.get_input<boxm2_cache_sptr>(i++);
  int blk_i =pro.get_input<int>(i++);
  int blk_j =pro.get_input<int>(i++);
  int blk_k =pro.get_input<int>(i++);
  int index =pro.get_input<int>(i++);
  std::string ident = pro.get_input<std::string>(i++);
  std::string ident_type = pro.get_input<std::string>(i++);

  // set arguments
  // vgl_point_3d<double> local;
  boxm2_block_id id(blk_i, blk_j, blk_k);
  // if (!scene->contains(vgl_point_3d<double>(x, y, z), id, local))
    // return false;

  // int index_x=(int)std::floor(local.x());
  // int index_y=(int)std::floor(local.y());
  // int index_z=(int)std::floor(local.z());
  // boxm2_block * blk=cache->get_block(scene, id);
  // boxm2_block_metadata mdata = scene->get_block_metadata_const(id);
  // vnl_vector_fixed<unsigned char,16> treebits=blk->trees()(index_x,index_y,index_z);
  // boct_bit_tree tree(treebits.data_block(),mdata.max_level_);
  // int bit_index=tree.traverse(local);
  // int index=tree.get_data_index(bit_index,false);

  boxm2_data_base* data_base = cache->get_data_base(scene, id,ident);

  std::vector<float> return_data;

  if (ident_type.compare("BOXM2_FLOAT16")==0)
  {

    boxm2_data<BOXM2_FLOAT16>* data = new boxm2_data<BOXM2_FLOAT16>(data_base->data_buffer(),
                                                                     data_base->buffer_length(),
                                                                     data_base->block_id());
    boxm2_array_1d<boxm2_data_traits<BOXM2_FLOAT16>::datatype> data_array=data->data();
    boxm2_data_traits<BOXM2_FLOAT16>::datatype model=data_array[index];
    for(int x=0; x<16; x++)
      return_data.push_back(model[x]);
  }
  else if (ident_type.compare("BOXM2_COVARIANCE")==0)
  {

    boxm2_data<BOXM2_COVARIANCE>* data = new boxm2_data<BOXM2_COVARIANCE>(data_base->data_buffer(),
                                                                     data_base->buffer_length(),
                                                                     data_base->block_id());
    boxm2_array_1d<boxm2_data_traits<BOXM2_COVARIANCE>::datatype> data_array=data->data();
    boxm2_data_traits<BOXM2_COVARIANCE>::datatype model=data_array[index];
    for(int x=0; x<9; x++)
      return_data.push_back(model[x]);
  }
  else if (ident_type.compare("BOXM2_POINT")==0)
  {

    boxm2_data<BOXM2_POINT>* data = new boxm2_data<BOXM2_POINT>(data_base->data_buffer(),
                                                                     data_base->buffer_length(),
                                                                     data_base->block_id());
    boxm2_array_1d<boxm2_data_traits<BOXM2_POINT>::datatype> data_array=data->data();
    boxm2_data_traits<BOXM2_POINT>::datatype model=data_array[index];
    for(int x=0; x<4; x++)
      return_data.push_back(model[x]);
  }
  else
    throw "Unknown type";
  // X(BOXM2_ALPHA, "alpha", float) \
  // X(BOXM2_GAMMA, "gamma", float) \
  // X(BOXM2_MOG3_GREY, "boxm2_mog3_grey", (vnl_vector_fixed<unsigned char, 8>)) \
  // X(BOXM2_MOG3_GREY_16, "boxm2_mog3_grey16", (vnl_vector_fixed<unsigned short, 8>)) \
  // X(BOXM2_MOG6_VIEW, "boxm2_mog6_view", (vnl_vector_fixed<float, 16>)) \
  // X(BOXM2_MOG6_VIEW_COMPACT, "boxm2_mog6_view_compact", (vnl_vector_fixed<unsigned char, 16>)) \
  // X(BOXM2_BATCH_HISTOGRAM, "boxm2_batch_histogram", (vnl_vector_fixed<float, 8>)) \
  // X(BOXM2_GAUSS_RGB, "boxm2_gauss_rgb", (vnl_vector_fixed<unsigned char, 8>)) \
  // X(BOXM2_GAUSS_RGB_VIEW, "boxm2_gauss_rgb_view", (vnl_vector_fixed<int, 16>)) \
  // X(BOXM2_GAUSS_RGB_VIEW_COMPACT, "boxm2_gauss_rgb_view_compact", (vnl_vector_fixed<int, 8>)) \
  // X(BOXM2_GAUSS_UV_VIEW, "boxm2_gauss_uv_view", (vnl_vector_fixed<int, 4>)) \
  // X(BOXM2_MOG2_RGB,"boxm2_mog2_rgb", (vnl_vector_fixed<unsigned char, 16>)) \
  // X(BOXM2_NUM_OBS, "boxm2_num_obs", (vnl_vector_fixed<unsigned short, 4>)) \
  // X(BOXM2_NUM_OBS_SINGLE, "boxm2_num_obs_single", unsigned short) \
  // X(BOXM2_NUM_OBS_SINGLE_INT, "boxm2_num_obs_single_int", unsigned) \
  // X(BOXM2_NUM_OBS_VIEW, "boxm2_num_obs_view", (vnl_vector_fixed<float, 8>)) \
  // X(BOXM2_NUM_OBS_VIEW_COMPACT, "boxm2_num_obs_view_compact", (vnl_vector_fixed<short, 8>)) \
  // X(BOXM2_LABEL_SHORT, "boxm2_label_short", short) \
  // X(BOXM2_AUX, "aux", (vnl_vector_fixed<float, 4>)) \
  // X(BOXM2_INTENSITY, "boxm2_intensity", float) \
  // X(BOXM2_AUX0, "aux0", float) \
  // X(BOXM2_AUX1, "aux1", float) \
  // X(BOXM2_AUX2, "aux2", float) \
  // X(BOXM2_AUX3, "aux3", float) \
  // X(BOXM2_AUX4, "aux4", float) \
  // X(BOXM2_FLOAT, "float", float) \
  // X(BOXM2_FLOAT8, "float8", (vnl_vector_fixed<float, 8>)) \
  // X(BOXM2_FLOAT16, "float16", (vnl_vector_fixed<float, 16>)) \
  // X(BOXM2_VIS_SPHERE, "boxm2_vis_sphere", (vnl_vector_fixed<float, 16>)) \
  // X(BOXM2_NORMAL, "boxm2_normal", (vnl_vector_fixed<float, 4>)) \
  // X(BOXM2_POINT, "boxm2_point", (vnl_vector_fixed<float, 4>)) \
  // X(BOXM2_VIS_SCORE, "boxm2_vis_score", float) \
  // X(BOXM2_GAUSS_GREY, "boxm2_gauss_grey", (vnl_vector_fixed<unsigned char, 2>)) \
  // X(BOXM2_NORMAL_ALBEDO_ARRAY, "boxm2_normal_albedo_array", boxm2_normal_albedo_array) \
  // X(BOXM2_COVARIANCE, "boxm2_covariance", (vnl_vector_fixed<float, 9>)) \
  // X(BOXM2_FEATURE_VECTOR, "boxm2_feature_vector", boxm2_feature_vector) \
  // X(BOXM2_PIXEL, "boxm2_pixel", unsigned char) \
  // X(BOXM2_EXPECTATION, "boxm2_expectation", float) \
  // X(BOXM2_DATA_INDEX, "boxm2_data_index", unsigned int) \
  // X(BOXM2_RAY_DIR, "boxm2_ray_dir", (vnl_vector_fixed<float, 4>)) \
  // X(BOXM2_CHAR8, "char8", (vnl_vector_fixed<unsigned char, 8>)) \
  // X(BOXM2_VEC3D, "boxm2_vec3d", (vnl_vector_fixed<float, 4>))


  bbas_1d_array_float_sptr return_data_sptr = new bbas_1d_array_float(return_data.size());
  for(int x=0; x<return_data.size(); x++)
    return_data_sptr->data_array[x] = return_data[x];
  pro.set_output_val<bbas_1d_array_float_sptr>(0, return_data_sptr);

  return true;
}
