//:
// \file
// \brief  Test for converting an image src to a n-planes image using vil_pro
// \author Isabel Restrepo
// \date   August 07, 2008
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
vil_image_view_base_sptr test_process(vil_image_view_base_sptr const &ref_img)
{
  brdb_value_sptr v0 = new brdb_value_t<vil_image_view_base_sptr>(ref_img);
  brdb_value_sptr v1 = new brdb_value_t<unsigned>(3);

  bool good = bprb_batch_process_manager::instance()->init_process("vilConvertToNPlanesProcess");
  good = good && bprb_batch_process_manager::instance()->set_input(0, v0);
  good = good && bprb_batch_process_manager::instance()->set_input(1, v1);
  good = good && bprb_batch_process_manager::instance()->run_process();

  unsigned id_img;
  good = good && bprb_batch_process_manager::instance()->commit_output(0, id_img);
  TEST("run vil_convert_to_n_planes_process", good ,true);

  brdb_query_aptr Q_img = brdb_query_comp_new("id", brdb_query::EQ, id_img);
  brdb_selection_sptr S_img = DATABASE->select("vil_image_view_base_sptr_data", std::move(Q_img) );
  TEST("output image is in db", S_img->size(), 1);

  brdb_value_sptr value_img;
  TEST("output image is in db", S_img->get_value(std::string("value"), value_img), true);
  TEST("output image is non-null", (value_img != nullptr) ,true);

  auto* result =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(value_img.ptr());
  vil_image_view_base_sptr out_img_base = result->value();

  return out_img_base;
}


//: This test verifies that vil_convert_to_n_planes_process is working for a
// floating type image and a vxl_byte image. The images are converted from 4 planes
// to 2 planes

static void test_vil_convert_to_n_planes_process()
{
  //Take care of database registration
  DECLARE_FUNC_CONS(vil_convert_to_n_planes_process);
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vil_convert_to_n_planes_process, "vilConvertToNPlanesProcess");
  REGISTER_DATATYPE(vil_image_view_base_sptr);
  REGISTER_DATATYPE(unsigned);

  //Initialize testing images
  constexpr unsigned n = 10;
  std::cout<<"testing test_convert_to_n_planes(src,dest):\n";
  vil_image_view<float> f_image(n,n,4);
  vil_image_view<vxl_byte> byte_image(n,n,4);
  vil_image_view<float> f_expected(n,n,3);
  vil_image_view<vxl_byte> byte_expected(n,n,3);

  for (unsigned j=0;j<f_image.nj();++j)
  {
    for (unsigned i=0;i<f_image.ni();++i)
    {
      f_image(i,j,0)=1.0f*i+10.0f*j+15.0f;
      f_image(i,j,1)=1.0f*i+10.0f*j+10.0f;
      f_image(i,j,2)=1.0f*i+10.0f*j+5.0f;
      f_image(i,j,3)=1.0f*i+10.0f*j;

      byte_image(i,j,0)=static_cast<vxl_byte>(1*i+10*j+15);
      byte_image(i,j,1)=static_cast<vxl_byte>(1*i+10*j+10);
      byte_image(i,j,2)=static_cast<vxl_byte>(1*i+10*j+5);
      byte_image(i,j,3)=static_cast<vxl_byte>(1*i+10*j);

      f_expected(i,j,0)=1.0f*i+10.0f*j+15.0f;
      f_expected(i,j,1)=1.0f*i+10.0f*j+10.0f;
      f_expected(i,j,2)=1.0f*i+10.0f*j+5.0f;

      byte_expected(i,j,0)=static_cast<vxl_byte>(1*i+10*j+15);
      byte_expected(i,j,1)=static_cast<vxl_byte>(1*i+10*j+10);
      byte_expected(i,j,2)=static_cast<vxl_byte>(1*i+10*j+5);
    }
  }

  //TEST a floating point case
  vil_image_view_base_sptr f_image_ref = new vil_image_view<float>(f_image);
  vil_image_view_base_sptr f_base = test_process(f_image_ref);
  vil_image_view<float> f_observed(f_base);

  TEST("Float-Image as expected",vil_image_view_deep_equality(f_observed, f_expected), true);

  bprb_batch_process_manager::instance()->clear();

  //TEST a byte case
  vil_image_view_base_sptr byte_image_ref = new vil_image_view<vxl_byte>(byte_image);
  vil_image_view_base_sptr byte_base = test_process(byte_image_ref);

  vil_image_view<vxl_byte> byte_observed(byte_base);

  TEST("Byte-Image as expected",vil_image_view_deep_equality(byte_observed,byte_expected), true);

  bprb_batch_process_manager::instance()->clear();
}

TESTMAIN(test_vil_convert_to_n_planes_process);
