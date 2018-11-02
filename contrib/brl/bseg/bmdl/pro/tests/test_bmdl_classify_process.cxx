#include <iostream>
#include <string>
#include <testlib/testlib_test.h>
#include "../bmdl_processes.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <brdb/brdb_value.h>
#include <brdb/brdb_selection.h>

#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <bprb/bprb_macros.h>

#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_image_view_base.h>

bool get_image(unsigned int id, vil_image_view_base_sptr& image)
{
  brdb_query_aptr Q_img = brdb_query_comp_new("id", brdb_query::EQ, id);

  brdb_selection_sptr S_img = DATABASE->select("vil_image_view_base_sptr_data", std::move(Q_img) );
  if (S_img->size()!=1){
    std::cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
             << " no selections\n";
    return false;
  }

  brdb_value_sptr value_img;
  if (!S_img->get_value(std::string("value"), value_img)) {
    std::cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
             << " didn't get value\n";
    return false;
  }

  bool non_null = (value_img != nullptr);
  TEST("display output non-null", non_null ,true);

  auto* result =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(value_img.ptr());
  image = result->value();

  return true;
}

static void test_bmdl_classify_process()
{
  REG_PROCESS_FUNC(bprb_func_process, bprb_batch_process_manager, bmdl_classify_process, "bmdlClassifyProcess");
  REGISTER_DATATYPE_LONG_FORM(std::string,vcl_string);
  REGISTER_DATATYPE(vil_image_view_base_sptr);

  vil_image_view_base_sptr first_return = vil_load("first_ret.tif");
  vil_image_view_base_sptr last_return = vil_load("last_ret.tif");
  vil_image_view_base_sptr ground = vil_load("ground.tif");

  brdb_value_sptr v0 = new brdb_value_t<vil_image_view_base_sptr>(first_return);
  brdb_value_sptr v1 = new brdb_value_t<vil_image_view_base_sptr>(last_return);
  brdb_value_sptr v2 = new brdb_value_t<vil_image_view_base_sptr>(ground);

  bool good1 = bprb_batch_process_manager::instance()->init_process("bmdlClassifyProcess")
            && bprb_batch_process_manager::instance()->set_input(0, v0)
            && bprb_batch_process_manager::instance()->set_input(1, v1)
            && bprb_batch_process_manager::instance()->set_input(2, v2)
            && bprb_batch_process_manager::instance()->set_params("classify_params.xml")
            && bprb_batch_process_manager::instance()->run_process();

  unsigned int label_img_id, height_img_id;
  std::string type;
  bool good2 = bprb_batch_process_manager::instance()->commit_output(0, label_img_id, type);
  bool good3 = bprb_batch_process_manager::instance()->commit_output(1, height_img_id, type);
  TEST("run classify process", good1 && good2 && good3, true);

  vil_image_view_base_sptr label_img, height_img;
  bool good4 = get_image(label_img_id, label_img);
  TEST("get label image", good4 ,true);

  bool good5 = get_image(height_img_id, height_img);
  TEST("get height image", good5 ,true);

  bool saved = vil_save(*label_img, "label.tif");
  TEST("LABELS saved", saved, true);

  saved = vil_save(*height_img, "height.tif");
  TEST("HEIGHTS saved", saved, true);
}

TESTMAIN(test_bmdl_classify_process);
