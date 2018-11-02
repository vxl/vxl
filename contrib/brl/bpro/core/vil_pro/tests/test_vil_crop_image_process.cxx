//:
// \file
// \brief  Test to crop an image
// \author Ozge C. Ozcanli
// \date   January 05, 2009
//

#include <string>
#include <iostream>
#include <testlib/testlib_test.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <brdb/brdb_value.h>
#include <brdb/brdb_selection.h>

#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_parameters.h>
#include <bprb/bprb_macros.h>
#include <bprb/bprb_func_process.h>

#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>


//: This function sets up input for the process and returns the output of
//  vil_convert_to_n_planes_process
vil_image_view_base_sptr test_process(vil_image_view_base_sptr const &ref_img, unsigned i0, unsigned j0, unsigned ni, unsigned nj)
{
  brdb_value_sptr v0 = new brdb_value_t<vil_image_view_base_sptr>(ref_img);
  brdb_value_sptr v1 = new brdb_value_t<unsigned>(i0);
  brdb_value_sptr v2 = new brdb_value_t<unsigned>(j0);
  brdb_value_sptr v3 = new brdb_value_t<unsigned>(ni);
  brdb_value_sptr v4 = new brdb_value_t<unsigned>(nj);

  bool good = bprb_batch_process_manager::instance()->init_process("vilCropImageProcess");
  good = good && bprb_batch_process_manager::instance()->set_input(0, v0);
  good = good && bprb_batch_process_manager::instance()->set_input(1, v1);
  good = good && bprb_batch_process_manager::instance()->set_input(2, v2);
  good = good && bprb_batch_process_manager::instance()->set_input(3, v3);
  good = good && bprb_batch_process_manager::instance()->set_input(4, v4);
  good = good && bprb_batch_process_manager::instance()->run_process();

  unsigned id_img;
  good = good && bprb_batch_process_manager::instance()->commit_output(0, id_img);
  TEST("run vil_crop_image_process", good ,true);

  brdb_query_aptr Q_img = brdb_query_comp_new("id", brdb_query::EQ, id_img);
  brdb_selection_sptr S_img = DATABASE->select("vil_image_view_base_sptr_data", std::move(Q_img));
  TEST("output image is in db", S_img->size(), 1);

  brdb_value_sptr value_img;
  TEST("output image is in db", S_img->get_value(std::string("value"), value_img), true);
  TEST("output image is non-null", (value_img != nullptr) ,true);

  auto* result =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(value_img.ptr());
  vil_image_view_base_sptr out_img_base = result->value();

  return out_img_base;
}


static void test_vil_crop_image_process()
{
  //Take care of database registration
  DECLARE_FUNC_CONS(vil_crop_image_process);
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_crop_image_process, "vilCropImageProcess");
  REGISTER_DATATYPE(vil_image_view_base_sptr);
  REGISTER_DATATYPE(unsigned);

  //Initialize testing images
  const unsigned n=10, n2 = 5;
  std::cout<<"testing test_crop_image(src,dest):\n";
  vil_image_view<float> f_image(n,n,1);
  f_image.fill(1.0f);
  vil_image_view<float> f_expected(n2,n2,1);
  f_expected.fill(1.0f);

  vil_image_view<bool> bool_image(n,n,1);
  bool_image.fill(true);
  vil_image_view<bool> bool_expected(n2,n2,1);
  bool_expected.fill(true);


  //TEST a floating point case
  vil_image_view_base_sptr f_image_ref = new vil_image_view<float>(f_image);
  vil_image_view_base_sptr f_base = test_process(f_image_ref, 2, 3, n2, n2);
  vil_image_view<float> f_observed(f_base);

  TEST("Float-Image as expected",vil_image_view_deep_equality(f_observed, f_expected), true);

  bprb_batch_process_manager::instance()->clear();

  //TEST a byte case
  vil_image_view_base_sptr bool_image_ref = new vil_image_view<bool>(bool_image);
  vil_image_view_base_sptr bool_base = test_process(bool_image_ref, 2, 3, n2, n2);

  vil_image_view<bool> bool_observed(bool_base);

  TEST("Bool-Image as expected",vil_image_view_deep_equality(bool_observed,bool_expected), true);

  bprb_batch_process_manager::instance()->clear();
}

TESTMAIN(test_vil_crop_image_process);
