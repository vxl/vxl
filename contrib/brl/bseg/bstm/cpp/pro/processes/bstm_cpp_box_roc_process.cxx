// This is brl/bseg/bstm/cpp/pro/processes/bstm_cpp_box_roc_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief
//
// \author Ali Osman Ulusoy
// \date Nov 1, 2013

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
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
#include <bbas_pro/bbas_1d_array_float.h>
#include <boct/boct_bit_tree.h>


namespace bstm_cpp_box_roc_process_globals
{
  constexpr unsigned n_inputs_ = 14;
  constexpr unsigned n_outputs_ = 2;

  typedef unsigned char uchar;
  typedef unsigned short ushort;
  typedef vnl_vector_fixed<uchar, 16> uchar16;
  typedef vnl_vector_fixed<uchar, 8> uchar8;
  typedef vnl_vector_fixed<ushort, 4> ushort4;


}

bool bstm_cpp_box_roc_process_cons(bprb_func_process& pro)
{
  using namespace bstm_cpp_box_roc_process_globals;

  //process takes 1 input
  std::vector<std::string> input_types_(n_inputs_);

  input_types_[0] = "bstm_scene_sptr";
  input_types_[1] = "bstm_cache_sptr";
  //change region
  input_types_[2] = "float"; //center x
  input_types_[3] = "float"; //center y
  input_types_[4] = "float"; //center z
  input_types_[5] = "float"; //len x
  input_types_[6] = "float"; //len y
  input_types_[7] = "float"; //len z

  //box to ignore outside
  input_types_[8] = "float"; //center x
  input_types_[9] = "float"; //center y
  input_types_[10] = "float"; //center z
  input_types_[11] = "float"; //len x
  input_types_[12] = "float"; //len y
  input_types_[13] = "float"; //len z

  // process has 1 output:
  // output[0]: scene sptr
  std::vector<std::string> output_types;

  output_types.emplace_back("bbas_1d_array_float_sptr");  // tpr
  output_types.emplace_back("bbas_1d_array_float_sptr");  // fpr
  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types);
  return good;
}

bool bstm_cpp_box_roc_process(bprb_func_process& pro)
{
  using namespace bstm_cpp_box_roc_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  //get the inputs
  unsigned i = 0;
  bstm_scene_sptr scene =pro.get_input<bstm_scene_sptr>(i++);
  bstm_cache_sptr cache= pro.get_input<bstm_cache_sptr>(i++);
  auto change_region_center_x = pro.get_input<float>(i++);
  auto change_region_center_y = pro.get_input<float>(i++);
  auto change_region_center_z = pro.get_input<float>(i++);
  auto change_region_len_x = pro.get_input<float>(i++);
  auto change_region_len_y = pro.get_input<float>(i++);
  auto change_region_len_z = pro.get_input<float>(i++);

  auto center_x = pro.get_input<float>(i++);
  auto center_y = pro.get_input<float>(i++);
  auto center_z = pro.get_input<float>(i++);
  auto len_x = pro.get_input<float>(i++);
  auto len_y = pro.get_input<float>(i++);
  auto len_z = pro.get_input<float>(i++);
  //create vgl box,
  const vgl_point_3d<double> change_region_center(change_region_center_x,change_region_center_y,change_region_center_z);
  vgl_box_3d<double> change_box(change_region_center,change_region_len_x,change_region_len_y,change_region_len_z, vgl_box_3d<double>::centre);
  vgl_box_3d<double> change_box_insides(change_region_center,change_region_len_x,change_region_len_y,change_region_len_z, vgl_box_3d<double>::centre);
  change_box_insides.scale_about_centroid(0.85);

  //create vgl box,
  const vgl_point_3d<double> center(center_x,center_y,center_z);
  vgl_box_3d<double> box(center,len_x,len_y,len_z, vgl_box_3d<double>::centre);


  //init roc
  unsigned const numPoints = 1000; //num points on ROC
  auto * tp=new bbas_1d_array_float(numPoints);
  auto * tn=new bbas_1d_array_float(numPoints);
  auto * fp=new bbas_1d_array_float(numPoints);
  auto * fn=new bbas_1d_array_float(numPoints);
  for (unsigned int pnt=0; pnt<numPoints; ++pnt) {
    tp->data_array[pnt]=0.0f;
    fp->data_array[pnt]=0.0f;
    tn->data_array[pnt]=0.0f;
    fn->data_array[pnt]=0.0f;
  }


  //iterate over each block/metadata to check if bbox intersects the input bbox
  std::map<bstm_block_id, bstm_block_metadata> blocks = scene->blocks();
  std::map<bstm_block_id, bstm_block_metadata> ::const_iterator bstm_iter = blocks.begin();
  for(; bstm_iter != blocks.end() ; bstm_iter++)
  {
    bstm_block_id bstm_id = bstm_iter->first;
    bstm_block_metadata bstm_metadata = bstm_iter->second;


    bstm_block* blk = cache->get_block(bstm_metadata.id_);
    bstm_time_block* blk_t = cache->get_time_block(bstm_metadata.id_);

    bstm_data_base *change_buffer = cache->get_data_base(bstm_metadata.id_,bstm_data_traits<BSTM_CHANGE>::prefix()  );
    auto* change_array = (bstm_data_traits<BSTM_CHANGE>::datatype*) change_buffer->data_buffer();

    //iterate through each tree
    boxm2_array_3d<uchar16>&  trees = blk->trees();
    for (unsigned int x = 0; x < trees.get_row1_count(); ++x) {
      for (unsigned int y = 0; y < trees.get_row2_count(); ++y) {
       for (unsigned int z = 0; z < trees.get_row3_count(); ++z) {
         //load current block/tree
         uchar16 tree = trees(x, y, z);
         boct_bit_tree bit_tree((unsigned char*) tree.data_block());

         //first check if the tree box is contained in the box,
         vgl_point_3d<double> tree_min_pt(bstm_metadata.local_origin_.x() + x*bstm_metadata.sub_block_dim_.x(),
                                           bstm_metadata.local_origin_.y() + y*bstm_metadata.sub_block_dim_.y(),
                                           bstm_metadata.local_origin_.z() + z*bstm_metadata.sub_block_dim_.z());
         vgl_box_3d<double> tree_box(tree_min_pt,bstm_metadata.sub_block_dim_.x(),
                                                  bstm_metadata.sub_block_dim_.y(),
                                                  bstm_metadata.sub_block_dim_.z(),
                                                  vgl_box_3d<double>::min_pos);
         if(!vgl_intersection<double>(tree_box,box).is_empty() )
         {


           //iterate through leaves of the tree
           std::vector<int> leafBits = bit_tree.get_leaf_bits();
           std::vector<int>::iterator iter;
           for (iter = leafBits.begin(); iter != leafBits.end(); ++iter) {
             int curr_depth = bit_tree.depth_at((*iter));
             double side_len = 1.0 / (double) (1<<curr_depth);
             int data_offset = bit_tree.get_data_index(*iter);

             vgl_point_3d<double> localCenter = bit_tree.cell_center(*iter);
             vgl_point_3d<double> cellCenter(localCenter.x() + x, localCenter.y() + y, localCenter.z() + z);
             vgl_point_3d<double> cellCenter_global(   float(cellCenter.x()*bstm_metadata.sub_block_dim_.x() + bstm_metadata.local_origin_.x()),
                                                        float(cellCenter.y()*bstm_metadata.sub_block_dim_.y() + bstm_metadata.local_origin_.y()),
                                                        float(cellCenter.z()*bstm_metadata.sub_block_dim_.z() + bstm_metadata.local_origin_.z()));

             if(change_box_insides.contains(cellCenter_global))
               continue;

             for (unsigned int pnt=0; pnt<numPoints; ++pnt) {
               float detection_threshold = 1.0f-(float)pnt/(float)numPoints;
               if(change_array[data_offset] >= detection_threshold)
                 if(change_box.contains(cellCenter_global))
                   tp->data_array[pnt]+= side_len;
                 else
                   fp->data_array[pnt]+= side_len;
               else
                 if(change_box.contains(cellCenter_global))
                   fn->data_array[pnt]+= side_len;
                 else
                   tn->data_array[pnt]+= side_len;
             }

           }

         }

       }
      }
    }
  }


  auto * tpr=new bbas_1d_array_float(numPoints);
  auto * fpr=new bbas_1d_array_float(numPoints);

  for (unsigned int pnt=0; pnt<numPoints; ++pnt) {
    tpr->data_array[pnt]= tp->data_array[pnt] / (tp->data_array[pnt] + fn->data_array[pnt]);
    fpr->data_array[pnt]= fp->data_array[pnt] / (fp->data_array[pnt] + tn->data_array[pnt]);
    std::cout << "tpr: " <<tpr->data_array[pnt] << " and fpr: " << fpr->data_array[pnt] << std::endl;
  }

  pro.set_output_val<bbas_1d_array_float_sptr>(0, tpr);
  pro.set_output_val<bbas_1d_array_float_sptr>(1, fpr);
  return true;

}
