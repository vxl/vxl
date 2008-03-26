#include <testlib/testlib_test.h>
#include "../bvxm_change_detection_display_process.h"
#include <bvxm/bvxm_world_params.h>
#include <bvxm/bvxm_voxel_world.h>

#include <vcl_string.h>
#include <vcl_iostream.h>

#include <brdb/brdb_value.h>
#include <brdb/brdb_selection.h>

#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_parameters.h>
#include <bprb/bprb_macros.h>
#include <vil/vil_image_view.h>


MAIN( test_bvxm_change_detection_display_process )
{
  REG_PROCESS(bvxm_change_detection_display_process, bprb_batch_process_manager);
  REGISTER_DATATYPE(vil_image_view_base_sptr);

  vil_image_view<unsigned char> input_img(5,5,1);
  input_img.fill(255);

  vil_image_view<float> prob_img(5,5,1);

  prob_img.fill(0.3f);
  prob_img(2,2)=0.6f;
  prob_img(3,2)=0.6f;
  prob_img(2,3)=0.6f;
  prob_img(3,3)=0.6f;
  
  // set the inputs 
  brdb_value_sptr v0 = new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<unsigned char>(input_img));
  brdb_value_sptr v1 = new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<float>(prob_img));

  bool good = bprb_batch_process_manager::instance()->init_process("bvxmChangeDetectionDisplayProcess");
  good = bprb_batch_process_manager::instance()->set_params("change_display_params.xml");
  good = good && bprb_batch_process_manager::instance()->set_input(0, v0);
  good = good && bprb_batch_process_manager::instance()->set_input(1, v1);
  good = good && bprb_batch_process_manager::instance()->run_process();

  unsigned  id_img;
  good = good && bprb_batch_process_manager::instance()->commit_output(0, id_img);
  TEST("run change display process", good ,true);


  brdb_query_aptr Q_img = brdb_query_comp_new("id", brdb_query::EQ, id_img);
  brdb_selection_sptr S_img = DATABASE->select("vil_image_view_base_sptr_data", Q_img);
  if (S_img->size()!=1){
    vcl_cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
             << " no selections\n";
  }

  brdb_value_sptr value_img;
  if (!S_img->get_value(vcl_string("value"), value_img)) {
    vcl_cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
             << " didn't get value\n";
  }
  bool non_null = (value_img != 0);
  TEST("display output non-null", non_null ,true);

  brdb_value_t<vil_image_view_base_sptr>* result =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(value_img.ptr());
  vil_image_view_base_sptr nitf_roi = result->value();

  SUMMARY();
}
