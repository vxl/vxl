// This is brl/bseg/boxm2/cpp/pro/processes/boxm2_cpp_query_cell_brdf_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for querying  cell data at a given 3-d location
//
// \author Vishal Jain
// \date Mar 10, 2011

#include <vcl_fstream.h>
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

//#define SEGLEN_FACTOR 93206.7555f   //Hack representation of int32.maxvalue/(ni*nj*block_length)

namespace boxm2_cpp_query_cell_brdf_process_globals
{
  const unsigned n_inputs_ =  6;
  const unsigned n_outputs_ = 0;

  typedef boxm2_data_traits<BOXM2_FLOAT8>::datatype float8_datatype;
  typedef boxm2_data_traits<BOXM2_AUX0>::datatype aux0_datatype;
  typedef boxm2_data_traits<BOXM2_AUX1>::datatype aux1_datatype;
  typedef boxm2_data_traits<BOXM2_AUX2>::datatype aux2_datatype;
  typedef boxm2_data_traits<BOXM2_AUX3>::datatype aux3_datatype;
}

bool boxm2_cpp_query_cell_brdf_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_cpp_query_cell_brdf_process_globals;

  //process takes 1 input
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "boxm2_cache_sptr";
  input_types_[2] = "float"; //x
  input_types_[3] = "float"; //y
  input_types_[4] = "float"; //z
  input_types_[5] = "vcl_string"; //identifier string

  vcl_vector<vcl_string>  output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool  boxm2_cpp_query_cell_brdf_process(bprb_func_process& pro)
{
  using namespace boxm2_cpp_query_cell_brdf_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  boxm2_scene_sptr scene =pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_cache_sptr cache= pro.get_input<boxm2_cache_sptr>(i++);
  float x =pro.get_input<float>(i++);
  float y =pro.get_input<float>(i++);
  float z =pro.get_input<float>(i++);
  vcl_string ident = pro.get_input<vcl_string>(i++);
  // set arguments
  vgl_point_3d<double> local;
  boxm2_block_id id;
  if (!scene->contains(vgl_point_3d<double>(x, y, z), id, local))
    return false;

  int index_x=(int)vcl_floor(local.x());
  int index_y=(int)vcl_floor(local.y());
  int index_z=(int)vcl_floor(local.z());
  boxm2_block * blk=cache->get_block(id);
  boxm2_block_metadata mdata = scene->get_block_metadata_const(id);
  vnl_vector_fixed<unsigned char,16> treebits=blk->trees()(index_x,index_y,index_z);
  boct_bit_tree tree(treebits.data_block(),mdata.max_level_);
  int bit_index=tree.traverse(local);
  int index=tree.get_data_index(bit_index,false);
  boxm2_data_base          *  phongs_base  = cache->get_data_base(id,boxm2_data_traits<BOXM2_FLOAT8>::prefix(ident));
  boxm2_data<BOXM2_FLOAT8> *  phongs_data  = new boxm2_data<BOXM2_FLOAT8>(phongs_base->data_buffer(),
                                                                          phongs_base->buffer_length(),
                                                                          phongs_base->block_id());

  typedef boxm2_data_traits<BOXM2_FLOAT8>::datatype boxm2_float8_datatype;
  boxm2_array_1d<boxm2_float8_datatype> phongs_data_array=phongs_data->data();
  boxm2_float8_datatype model=phongs_data_array[index];

  vcl_cout<<"Index is "<<index<<vcl_endl;
  if (ident == "phongs_model")
    vcl_cout<<"Phongs Model ["<<model[0]<<','
                              <<model[1]<<','
                              <<model[2]<<','
                              <<model[3]<<','
                              <<model[4]<<','
                              <<model[5]<<']'<<vcl_endl;
  else if (ident == "cubic_model")
    vcl_cout<<"Cubic Model ["<<model[0]<<','
                             <<model[1]<<','
                             <<model[2]<<','
                             <<model[3]<<','
                             <<model[4]<<','
                             <<model[5]<<']'<<vcl_endl;

  return true;
}
