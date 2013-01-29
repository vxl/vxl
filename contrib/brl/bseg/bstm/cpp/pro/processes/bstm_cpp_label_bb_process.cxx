// This is brl/bseg/bstm/cpp/pro/processes/bstm_cpp_label_bb_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process labeling a box in the bstm_scene.
//
// \author Ali Osman Ulusoy
// \date Jan 28, 2013

#include <vcl_fstream.h>
#include <bstm/io/bstm_cache.h>
#include <bstm/io/bstm_lru_cache.h>
#include <bstm/bstm_scene.h>
#include <bstm/bstm_block.h>
#include <bstm/bstm_data_base.h>
//brdb stuff
#include <brdb/brdb_value.h>

#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_intersection.h>

#include <bstm/bstm_util.h>
#include <bstm/cpp/algo/bstm_label_bb_function.h>

namespace bstm_cpp_label_bb_process_globals
{
  const unsigned n_inputs_ =  11;
  const unsigned n_outputs_ = 0;
}

bool bstm_cpp_label_bb_process_cons(bprb_func_process& pro)
{
  using namespace bstm_cpp_label_bb_process_globals;

  //process takes 1 input
  vcl_vector<vcl_string> input_types_(n_inputs_);

  input_types_[0] = "bstm_scene_sptr";
  input_types_[1] = "bstm_cache_sptr";
  input_types_[2] = "float"; //center x
  input_types_[3] = "float"; //center y
  input_types_[4] = "float"; //center z
  input_types_[5] = "float"; //len x
  input_types_[6] = "float"; //len y
  input_types_[7] = "float"; //len z
  input_types_[8] = "unsigned"; //label
  input_types_[9] = "float"; //time
  input_types_[10] = "float"; //p_threshold

  // process has 1 output:
  // output[0]: scene sptr
  vcl_vector<vcl_string>  output_types_(n_outputs_);

  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
  return good;
}

bool bstm_cpp_label_bb_process(bprb_func_process& pro)
{
  using namespace bstm_cpp_label_bb_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }

  //get the inputs
  unsigned i = 0;
  bstm_scene_sptr scene =pro.get_input<bstm_scene_sptr>(i++);
  bstm_cache_sptr cache= pro.get_input<bstm_cache_sptr>(i++);
  float center_x = pro.get_input<float>(i++);
  float center_y = pro.get_input<float>(i++);
  float center_z = pro.get_input<float>(i++);
  float len_x = pro.get_input<float>(i++);
  float len_y = pro.get_input<float>(i++);
  float len_z = pro.get_input<float>(i++);
  unsigned label = pro.get_input<unsigned>(i++);
  float time = pro.get_input<float>(i++);
  float p_threshold = pro.get_input<float>(i++);

  //create vgl box
  const vgl_point_3d<double> center(center_x,center_y,center_z);
  vgl_box_3d<double> box(center,len_x,len_y,len_z, vgl_box_3d<double>::centre);

  //iterate over each block/metadata to check if bbox intersects the input bbox
  vcl_map<bstm_block_id, bstm_block_metadata> blocks = scene->blocks();
  vcl_map<bstm_block_id, bstm_block_metadata> ::const_iterator bstm_iter = blocks.begin();
  for(; bstm_iter != blocks.end() ; bstm_iter++)
  {
    bstm_block_id bstm_id = bstm_iter->first;
    bstm_block_metadata bstm_metadata = bstm_iter->second;
    if(!vgl_intersection<double>( bstm_metadata.bbox(), box).is_empty() ) //if the two boxes intersect
    {
      double local_time;
      if(bstm_metadata.contains_t (time, local_time) ) //if the block box contains the given time
      {
        vcl_cout << "Found intersecting bbox at block " << bstm_id << "..." << vcl_endl;

        bstm_block* blk = cache->get_block(bstm_metadata.id_);
        bstm_time_block* blk_t = cache->get_time_block(bstm_metadata.id_);
        bstm_data_base * alph    = cache->get_data_base(bstm_metadata.id_, bstm_data_traits<BSTM_ALPHA>::prefix());
        bstm_data_base * label_data_base = cache->get_data_base(bstm_metadata.id_, bstm_data_traits<BSTM_LABEL>::prefix(),
                                        alph->buffer_length() / bstm_data_traits<BSTM_ALPHA>::datasize() * bstm_data_traits<BSTM_LABEL>::datasize() );

        bstm_data_traits<BSTM_ALPHA>::datatype * alpha_data = (bstm_data_traits<BSTM_ALPHA>::datatype*) alph->data_buffer();
        bstm_data_traits<BSTM_LABEL>::datatype * label_data = (bstm_data_traits<BSTM_LABEL>::datatype*) label_data_base->data_buffer();

        bstm_label_bb(blk, bstm_metadata, blk_t,label_data, alpha_data, local_time,label, box, p_threshold);

      }
    }
  }

  return true;

}
